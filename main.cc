/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/
//#include "helper.h"
//////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <iostream>

#include <deque>
#include <vector>
#include <semaphore.h>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <time.h>

#include "helper.h"

void *producer(void *id);
void *consumer(void *id);

sem_t empty_count;
sem_t full_count;
sem_t queue_access_mutex; 

int queue_size;
int number_of_jobs_for_each_producer;
int number_of_producers;
int number_of_consumers;

struct timespec ts_consumer;
struct timespec ts_producer;

struct job
{
  job() {}
  job(int id_, int t) : id(id_), duration(t) {}

  int id;
  int duration;
};

std::deque<job> Q;

using namespace std;


int main(int argc, char *argv[])
{

  if (argc < 5)
  {
    cerr << "Insufficient number of input parameters" << endl;
    return 0;
  }

  //std::size_t pos;
  //std::string arg = argv[1];

  queue_size = check_arg (argv[1]);
  number_of_jobs_for_each_producer = check_arg (argv[2]);
  number_of_producers =check_arg (argv[3]);
  number_of_consumers = check_arg (argv[4]);

/*
  try
  {
    queue_size = std::stoi(arg, &pos);
    if (pos < arg.size())
    {
      std::cerr << "Extra characters after number: " << arg << '\n';
    }
  }
  catch (std::invalid_argument const &ex)
  {
    std::cerr << "Invalid number entered: " << arg << '\n';
  }
  catch (std::out_of_range const &ex)
  {
    std::cerr << "Number out of range: " << arg << '\n';
  }

  arg = argv[2]; //cout << "queue size = " << queue_size;

  try
  {
    number_of_jobs_for_each_producer = std::stoi(arg, &pos);
    if (pos < arg.size())
    {
      std::cerr << "Extra characters after number: " << arg << '\n';
    }
  }
  catch (std::invalid_argument const &ex)
  {
    std::cerr << "Invalid number entered: " << arg << '\n';
  }
  catch (std::out_of_range const &ex)
  {
    std::cerr << "Number out of range: " << arg << '\n';
  }


  arg = argv[3];

  try
  {
    number_of_producers = std::stoi(arg, &pos);
    if (pos < arg.size())
    {
      std::cerr << "Extra characters after number: " << arg << '\n';
    }
  }
  catch (std::invalid_argument const &ex)
  {
    std::cerr << "Invalid number entered: " << arg << '\n';
  }
  catch (std::out_of_range const &ex)
  {
    std::cerr << "Number out of range: " << arg << '\n';
  }

  arg = argv[4];

  try
  {
    number_of_consumers = std::stoi(arg, &pos);
    if (pos < arg.size())
    {
      std::cerr << "Extra characters after number: " << arg << '\n';
    }
  }
  catch (std::invalid_argument const &ex)
  {
    std::cerr << "Invalid number entered: " << arg << '\n';
  }
  catch (std::out_of_range const &ex)
  {
    std::cerr << "Number is out of range: " << arg << '\n';
  }
*/

  sem_init(&empty_count, 0, queue_size); // size of buffer
  sem_init(&full_count, 0, 0);           //
  sem_init(&queue_access_mutex, 0, 1);   //

  pthread_t consumer_threads[number_of_consumers];
  pthread_t producer_threads[number_of_producers];

  int *temp;
  int *temp2;

  int i = 0;
  int j = 0;

  temp = new (nothrow) int[number_of_consumers];
  temp2 = new (nothrow) int[number_of_producers];

  cout << "Settings summary: consumers = " << number_of_consumers << " / producers = " << number_of_producers << " queue_size = " << queue_size << " / number_of_jobs_for_each_producer = " << number_of_jobs_for_each_producer << endl;

  while (i < number_of_consumers)
  {
    temp[i] = i;
    pthread_create(&consumer_threads[i], NULL, consumer, (void *)&temp[i]);
    ++i;
  }

  cout << "..Created all consumer threads..";

  while (j < number_of_producers)
  {
    temp2[j] = j;
    pthread_create(&producer_threads[j], NULL, producer, (void *)&temp2[j]);
    ++j;
  }

  cout << "..Created all producer threads..";
  cout << "\n\nCreated all threads!!!";

  for (int i = 0; i < number_of_producers; i++)
  {
    pthread_join(producer_threads[i], NULL); // Line 8
  }

  for (int i = 0; i < number_of_consumers; i++)
  {
    pthread_join(consumer_threads[i], NULL); // Line 7
  }

  cout << "Joined all threads!!!\n";
  cout << "..Cleanup starts..";

  sem_destroy(&empty_count);
  sem_destroy(&full_count);
  sem_destroy(&queue_access_mutex);

  delete[] temp;
  delete[] temp2;

  return 0;
}




void *producer(void *id)
{
  int *producer_id = (int *)id;
  int producer_timer_result = 0;

  cout << "\nStarted producer thread = " << *producer_id << endl;

  for (int p = 0; p < number_of_jobs_for_each_producer; p++)
  {
    int sleep_time = (rand() % 5) + 1;
    int duration = (rand() % 10) + 1; // Duration for each job should be between 1 – 10 seconds.

    sleep(sleep_time);

    int current_number_of_items_in_buffer = Q.size();
    int job_id = p;

    clock_gettime(CLOCK_REALTIME, &ts_producer);
    ts_producer.tv_sec += 20; //i++; //cout << "at t = " << i << endl;

    producer_timer_result = sem_timedwait(&empty_count, &ts_producer);

    if (producer_timer_result == -1)
    {
      cout << "Producer(" << *producer_id << "): Job id " << job_id << " timed out" << endl;
      break;
    } // if timer times out - break

    sem_wait(&queue_access_mutex);

    for (int i = 0; i < current_number_of_items_in_buffer; ++i)
    {
      auto it = find_if(Q.begin(), Q.end(), [&p, &current_number_of_items_in_buffer, &i](const job &obj) { return obj.id == ((p + i) % current_number_of_items_in_buffer); });
      if (it != Q.end())
      {
        job_id = ((p + i) % current_number_of_items_in_buffer);
        break;
      }
    }

    job J = job(job_id, duration);

    Q.push_back(J); // every 5 seconds = already slept
    cout << "Producer(" << *producer_id << "): Job id " << job_id << " sleeping for " << sleep_time << " and produced job with duration = " << duration << endl;

    sem_post(&queue_access_mutex);
    sem_post(&full_count);

   } // for loop ends

  pthread_exit(0);
}



void *consumer(void *id)
{
  int *consumer_id = (int *)id;
  job J_copy;
  int consumer_timer_result = 0;

  cout << "\nStarting consumer thread with id = " << *consumer_id << endl; // *consumer_id;

  while (true)
  {
    clock_gettime(CLOCK_REALTIME, &ts_consumer);
    ts_consumer.tv_sec += 20;

    consumer_timer_result = sem_timedwait(&full_count, &ts_consumer);
    if (consumer_timer_result == -1)
    {
      break;
    }

    sem_wait(&queue_access_mutex); // enter critical section

    if (Q.size() > 0)
    {
      job J = Q.front();
      J_copy = job(J.id, J.duration);
      Q.pop_front();
      cout << "Consumer(" << *consumer_id << "): Job id " << J_copy.id << " grabbed item..about to consume for duration = " << J_copy.duration << "...";
    }

    sem_post(&queue_access_mutex);
    sem_post(&empty_count);

    sleep(J_copy.duration); // Consume
    cout << "Consumer(" << *consumer_id << "): Job id " << J_copy.id << " consumption done.." << endl;
  }

  if (consumer_timer_result == -1)
  {
    cout << "Consumer(" << *consumer_id << "): Job id " << J_copy.id << " timed out waiting" << endl;
  }

  pthread_exit(0);
}
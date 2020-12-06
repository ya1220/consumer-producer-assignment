/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
# include <errno.h>
# include <string.h>
# include <pthread.h>
# include <ctype.h>
# include <iostream>

//#include "helper.h"
//////////////////////////////////////////////

#include <deque>
#include <semaphore.h>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <time.h>

void *producer (void *id);
void *consumer (void *id);

sem_t empty_count;
sem_t full_count;
sem_t queue_access_mutex; // pthread_mutex_T

int queue_size;
int number_of_jobs_for_each_producer;
int number_of_producers;
int number_of_consumers;

struct timespec ts_consumer;
struct timespec ts_producer;

struct job{
  job() {}
  job(int id_, int t) : id(id_),duration(t) {}

  int id;
  int duration;
};

std::deque<job> Q;

//////////////////////////////////////////////

using namespace std;

int main (int argc, char *argv[])
{

if (argc < 5) {
  cerr << "Insufficient number of input parameters" << endl;
  return 0;
}


std::size_t pos;
std::string arg = argv[1];

try {
  queue_size = std::stoi(arg, &pos);
  if (pos < arg.size()) {
    std::cerr << "Extra characters after number: " << arg << '\n';
  }
} catch (std::invalid_argument const &ex) {
  std::cerr << "Invalid number: " << arg << '\n';
} catch (std::out_of_range const &ex) {
  std::cerr << "Number out of range: " << arg << '\n';
}

arg = argv[2]; //cout << "queue size = " << queue_size;
number_of_jobs_for_each_producer = std::stoi(arg, &pos);

arg = argv[3];
number_of_producers = std::stoi(arg, &pos);

arg = argv[4];
number_of_consumers = std::stoi(arg, &pos);



// SHARED OR ZERO?
sem_init(&empty_count, 0, queue_size); // size of buffer
sem_init(&full_count, 0, 0);           // 
sem_init(&queue_access_mutex,0,1);     // 

pthread_t consumer_threads[number_of_consumers];
pthread_t producer_threads[number_of_producers];

vector<int> temp;
   
for(int i = 0; i < number_of_consumers; i++) {
  temp.push_back(i);
  cout << "\nIn main: creating thread in Consumer - id = " << i << endl;
      pthread_create(&consumer_threads[i], NULL, producer, (void *)&temp[i]);
}

for(int i = 0; i < number_of_producers; i++) {
    cout << "\nIn main: creating thread in Producer - id = " << i << endl;
      pthread_create(&producer_threads[i], NULL, consumer, (void *)&temp[i]);
}

for(int i = 0; i < number_of_producers; i++ ) {
      pthread_join(producer_threads[i],NULL); // Line 8
}

for(int i = 0; i < number_of_consumers; i++ ) {
      pthread_join(consumer_threads[i],NULL); // Line 7
}

 pthread_exit(NULL);

 sem_destroy(&empty_count);
 sem_destroy(&full_count);
 sem_destroy(&queue_access_mutex);

  return 0;
}


void *producer (void *id) 
{
/*
(b) Add the required number of jobs to the circular queue, with each job being added once every 1 – 5 seconds. 
If a job is taken (and deleted) by the consumer, then another job can be produced which has the same id. 
If the circular queue is full, block while waiting for an empty slot and if a slot doesn’t become
available after 20 seconds, quit, even though you have not produced all the jobs.
(d) Quit when there are no more jobs left to produce.
*/

  int *producer_id = (int *)id;
  bool wait_within_time_limit = true;

  cout << "\nEntered producer with id = " << *producer_id;
  
  while (wait_within_time_limit){
    for(int p = 0;p < number_of_jobs_for_each_producer;p++){

    int sleep_time = (rand() % 5) + 1;
    int duration = (rand() % 10) + 1; // Duration for each job should be between 1 – 10 seconds. 

    sleep(sleep_time);

    int current_number_of_items_in_buffer = Q.size();
    int job_id = p;   

    for (int i = 0; i < current_number_of_items_in_buffer; ++i){
      auto it = find_if(Q.begin(), Q.end(), [&p,&current_number_of_items_in_buffer,&i](const job& obj) {return obj.id == ((p+i) % current_number_of_items_in_buffer);});
      if (it != Q.end()){job_id = ((p+i) % current_number_of_items_in_buffer);break;}
    }

    job J = job(job_id,duration);
     cout << "..Created job with id = " << job_id << " and duration = " << duration << endl;

    // if queue has space - then add item
    if (current_number_of_items_in_buffer < queue_size){ 
      sem_wait(&empty_count);
      sem_wait(&queue_access_mutex);
      
      Q.push_back(J);     // every 5 seconds = already slept
      
      sem_post(&queue_access_mutex);
      sem_post(&full_count);
  } else{ 
    ts_producer.tv_sec += 20;     cout << "..producer waiting 20 seconds..";
    if (sem_timedwait(&empty_count, &ts_producer)!=-1) {wait_within_time_limit = false; break;}  // 20 seconds
    cout << "..done waiting..";
}

std::ofstream ofs("output2.txt", std::ofstream::out);
  cout << "Producer("<< *producer_id << "): Job id " << job_id << " sleeping for " << sleep_time << " seconds" << endl;
  cout << "Producer("<< *producer_id << "): Job id " << job_id << " duration " << duration << " seconds" << endl;
ofs.close();
      } // for loop ends
  } // while ends

  pthread_exit(0);
}

void *consumer (void *id) 
{ /*
(b) Take a job from the circular queue - ‘sleep’ for the duration specified. 
If the circular queue is empty, block while waiting for jobs and quit if no jobs arrive within 20 seconds.
(d) If there are no jobs left to consume, wait for 20 seconds to check if any new jobs are added, and if not, quit.
*/  
  int *consumer_id = (int *) id;
  bool consumer_wait_within_time_limit = true;
  job J_copy;

  cout << "\nStarting consumer thread with id = " << *consumer_id;

  while(consumer_wait_within_time_limit) {

  /*
                sem_wait(&full_count);
    do {
        clock_gettime(CLOCK_REALTIME, &ts_consumer);
        ts_consumer.tv_sec += 1;
        i++;       //  printf("i=%d\n",i);
        (if Q.size() > 0) {

          sem_wait(&queue_access_mutex);
              job J = Q.front();
              J_copy = job(J.id,J.duration);
              Q.pop_front();
          // lift
          // break
             sleep(J.duration);     // Consume
        }
        else{ // if queue empty - 
          wait for 
        }

        if (i == 20) {sem_post(&empty_count);}
    }
    while (sem_timedwait(&empty_count, &ts_consumer) == -1 );
   
  */

    sem_wait(&full_count);
    sem_wait(&queue_access_mutex);

    if (Q.size() > 0){ // if there is something to take
      //job J = Q.front();
      J_copy = job(Q.front().id,Q.front().duration);
      Q.pop_front();
    } else
    {
      ts_consumer.tv_sec += 20;     cout << "..consumer waiting 20 seconds..";
      if (sem_timedwait(&empty_count, &ts_consumer)!=-1) {wait_within_time_limit = false; break;}  // 20 seconds
      cout << "..done waiting..";
    }

    sem_post(&queue_access_mutex);
    sem_post(&empty_count);

    sleep(J_copy.duration);     // Consume

std::ofstream ofs("output2.txt", std::ofstream::out);
  cout << "Consumer(" << *consumer_id << "): Job id " << J_copy.id << " executing sleep duration " << J_copy.duration << endl;
  cout << "Consumer(" << *consumer_id << "): Job id " << J_copy.id << " completed" << endl;
ofs.close();
  }

  pthread_exit (0);
}
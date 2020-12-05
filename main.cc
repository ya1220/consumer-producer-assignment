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

#include <queue>
#include <semaphore.h>
#include <cstdlib>
#include <fstream>

void *producer (void *id);
void *consumer (void *id);

sem_t empty_count;
sem_t full_count;
sem_t queue_access_mutex; // pthread_mutex_T

int queue_size;
int number_of_jobs_for_each_producer;
int number_of_producers;
int number_of_consumers;

struct timespec ts;

struct job{
  job(int id, int t) : job_id(id),duration(t) {}

  int job_id;
  int duration;
};

std::queue<job> Q;

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

// resize queue
   
for(int i = 0; i < number_of_producers; i++) {
      //int r_p = 
      pthread_create(&consumer_threads[i], NULL, producer, (void *)&i);
}

for(int i = 0; i < number_of_producers; i++) {
      //int r_c = 
      pthread_create(&producer_threads[i], NULL, consumer, (void *)&i);
}

//cout << "main() : successfully created both consumer and producer, " << endl << endl;

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


void *producer (void *parameter) 
{
/*
(b) Add the required number of jobs to the circular queue, with each job being added once every
1 – 5 seconds. 
If a job is taken (and deleted) by the consumer, then another job can be produced which has the same id. 
If the circular queue is full, block while waiting for an empty slot and if a slot doesn’t become
available after 20 seconds, quit, even though you have not produced all the jobs.
(d) Quit when there are no more jobs left to produce.
*/
  cout << "\nEntered producer with id = " << *((int*)(parameter));
  // Fill the buffer initially

  bool wait_within_time_limit = true;
  
  while (wait_within_time_limit){
    for(int p = 0;p < number_of_jobs_for_each_producer;p++){

    int sleep_time = (rand() % 5) + 1;
    int duration = (rand() % 10) + 1; // Duration for each job should be between 1 – 10 seconds. 

    int job_id = p;     // how to set id?
    job J = job(job_id,duration);

    sleep(sleep_time);

// perform down op on semaphore + save timeout status
// timeout = sem_timed_wait (sem_id, space, 20);
// if (sem_timedwait( &sem, &tm) != -1 ) {break;}
// clock_gettime(CLOCK_REALTIME, &tm);
    //

    sem_wait(&empty_count);
    sem_wait(&queue_access_mutex);
    
    Q.push(J);
    
    sem_post(&queue_access_mutex);
    sem_post(&full_count);

// (c) Print the status (example format given in example output.txt).
std::ofstream ofs("output2.txt", std::ofstream::out);
ofs << "writing job id = " << job_id;
ofs.close();
      } // for loop ends
  } // while ends

  pthread_exit(0);
}

void *consumer (void *id) 
{
/*
(b) Take a job from the circular queue 
‘sleep’ for the duration specified. 
If the circular queue is empty, 
  block while waiting for jobs and 
  quit if no jobs arrive within 20 seconds.
(c) Print the status (example format given in example output.txt).
(d) If there are no jobs left to consume, wait for 20 seconds to check if any new jobs are added,
and if not, quit.
*/  


  bool consumer_wait_within_time_limit = true;

  while(consumer_wait_within_time_limit) {
    cout << "\nEntered consumer with id = " << *((int*)(id));
   
    sem_wait(&full_count);
    sem_wait(&queue_access_mutex);

    job J = Q.front();
    Q.pop();

    sem_post(&queue_access_mutex);
    sem_post(&empty_count);

    sleep(J.duration);

    // change the time limit status??
    //sem_timedwait();
  }

  pthread_exit (0);
}
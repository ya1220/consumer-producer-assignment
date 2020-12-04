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

void *producer (void *id);
void *consumer (void *id);

struct job{
  int job_id;
  int execution_time;
};

//////////////////////////////////////////////

int main (int argc, char *argv[])
{

if (argc < 5) {
  cerr << "Insufficient number of input parameters" << endl;
  return 0;
}

int queue_size;
int number_of_jobs_for_each_producer;
int number_of_producers;
int number_of_consumers;

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


sem_t empty_count;
sem_t full_count;
sem_t queue_access_mutex;

sem_init(&empty_count, SHARED, queue_size); // size of buffer
sem_init(&full_count, SHARED, 0);           // zero to start with
sem_init(&queue_access_mutex,SHARED,1);     // 0 or 1?

pthread_t consumer_threads[number_of_consumers];
pthread_t producer_threads[number_of_producers];
std::queue<job> Q;
   
for(int i = 0; i < number_of_producers; i++) {
      int r_p = pthread_create(&consumer_threads[i], NULL, producer, (void *)&i);
}

for(i = 0; i < number_of_producers; i++) {
      int r_c = pthread_create(&producer_threads[i], NULL, consumer, (void *)&i);
}

cout << "main() : successfully created both consumer and producer, " << endl << endl;

for(i = 0; i < NUM_THREADS; i++ ) {
      pthread_join(consumer_threads[i],NULL); // Line 7
      pthread_join(producer_threads[i],NULL); // Line 8
}

  pthread_exit(NULL);

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
  cout << "\nEntered producer with id = " << *parameter;
  // Fill the buffer initially

  bool wait_within_time_limit = true;
  
  while (wait_within_time_limit){
    int sleep_time = (rand() % 5) + 1;
    int job_duration = (rand() % 10) + 1; // Duration for each job should be between 1 – 10 seconds. 

    job J = job(job_duration);

    // sleep not always?
    sleep(sleep_time); 
    // change exit condition
    
    down(empty_count);
    down(queue_access_mutex);
    
    Q.push(J);
    
    up(queue_access_mutex);
    up(full_count);
  }
  // (c) Print the status (example format given in example output.txt).

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
  cout << "\nEntered consumer with id = " << *id;;
   
    down(full_count);
    down(queue_access_mutex);

    // Job J = Q.front();
    // Q.pop();

    up(queue_access_mutex);
    up(empty_count);

    // sleep(J.job_duration);
    // change the time limit status??
  }

  pthread_exit (0);
}
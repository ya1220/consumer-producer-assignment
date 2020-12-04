/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"

void *producer (void *id);
void *consumer (void *id);

//////////////////////////////////////////////

void *PrintHello(void *threadid) {
   long tid;
   tid = (long)threadid;
   cout << "Hello World! Thread ID, " << tid << endl;
   pthread_exit(NULL);
}

//////////////////////////////////////////////

int main (int argc, char *argv[])
{
  // TODO
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
    std::cerr << "Trailing characters after number: " << arg << '\n';
  }
} catch (std::invalid_argument const &ex) {
  std::cerr << "Invalid number: " << arg << '\n';
} catch (std::out_of_range const &ex) {
  std::cerr << "Number out of range: " << arg << '\n';
}

cout << "queue size = " << queue_size;

arg = argv[2];
number_of_jobs_for_each_producer = std::stoi(arg, &pos);

cout << "number of number_of_jobs_for_each_producer = " << number_of_jobs_for_each_producer << endl;
//queue_size = argv[1];
//number_of_jobs_for_each_producer = argv[2];
//number_of_producers = argv[3];
//number_of_consumers = argv[4];

/*
size of the queue, number of jobs to generate for
each producer (each producer will generate the same number of jobs), number of producers,
and number of consumers.
*/


//////////////////////////////////////////////

// sem_t empty, full;
// sem_init(&empty, SHARED, 1);
// sem_init(&full, SHARED, 0);
// sem_init(&sm,SHARED,1);

   // sem_t empty, full,sm;

   const int NUM_THREADS = 7;

   pthread_t consumer_threads[NUM_THREADS];
   pthread_t producer_threads[NUM_THREADS];
   
   int r_p;
   int r_c;
   int i;
   
   for(i = 0; i < NUM_THREADS; i++ ) {
      cout << "main() : creating thread, " << i << endl;
      r_p = pthread_create(&consumer_threads[i], NULL, producer, (void *)i); // MISSING ARGUMENT 
      r_c = pthread_create(&producer_threads[i], NULL, consumer, (void *)i);
      cout << "main() : successfully created both consumer and producer, " << endl << endl;
   }

   pthread_exit(NULL);
//////////////////////////////////////////////

  pthread_t producerid;
  int parameter = 5;

  pthread_create (&producerid, NULL, producer, (void *) &parameter);

  pthread_join (producerid, NULL);

  cout << "Doing some work after the join" << endl;

  return 0;
}

void *producer (void *parameter) 
{
  cout << "\nentered producer!!!";
  // TODO

  int *param = (int *) parameter;

  cout << "Parameter = " << *param << endl;

  sleep (5);

  cout << "\nThat was a good sleep - thank you \n" << endl;

  pthread_exit(0);
}

void *consumer (void *id) 
{
    // TODO 
    cout << "\nentered consumer!!!";

  pthread_exit (0);
}

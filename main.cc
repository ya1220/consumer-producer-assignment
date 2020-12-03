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

int main (int argc, char **argv)
{

  // TODO

//////////////////////////////////////////////
   const int NUM_THREADS = 7;

   pthread_t consumer_threads[NUM_THREADS];
   pthread_t producer_threads[NUM_THREADS];
   
   int r_p,r_c;
   int i;
   
   for( i = 0; i < NUM_THREADS; i++ ) {
      cout << "main() : creating producer thread, " << i << endl;
      r_p = pthread_create(&consumer_threads[i], NULL, producer, (void *)i);
      r_c = pthread_create(&producer_threads[i], NULL, consumer, (void *)i);
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
  cout << "entered producer!!!";
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
    cout << "entered consumer!!!";

  pthread_exit (0);

}

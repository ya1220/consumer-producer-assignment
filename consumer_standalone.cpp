void *consumer (void *id) 
{ 
  int *consumer_id = (int*)id;
  job J_copy;

  int i = 0;

  cout << "\nStarting consumer thread with id = " << *consumer_id << endl; // *consumer_id;

    do {
        clock_gettime(CLOCK_REALTIME, &ts_consumer);
        ts_consumer.tv_sec += 1; i++; //cout << "at t = " << i << endl;


          sem_wait(&queue_access_mutex);      
              job J = Q.front();
              J_copy = job(J.id,J.duration);
              Q.pop_front();
          sem_post(&queue_access_mutex);
          sem_post(&empty_count);

          std::ofstream ofs("output2.txt", std::ofstream::out);
            ofs << "Consumer(" << *consumer_id << "): Job id " << J_copy.id << " executing sleep duration " << J_copy.duration << endl;

          sleep(J.duration);     // Consume

            ofs << "Consumer(" << *consumer_id << "): Job id " << J_copy.id << " completed" << endl;
            ofs.close();


        if (i == 20) {sem_post(&empty_count);}
        
    } while (sem_timedwait( &full_count, &ts_consumer ) == -1 );

    cout << "..CONSUMER TIMED OUT!!..";
          std::ofstream ofs("output2.txt", std::ofstream::out);
            ofs << "Consumer(" << *consumer_id << "): Job id " << J_copy.id << " timed out waiting" << endl;  
          ofs.close();
          pthread_exit(0);
}
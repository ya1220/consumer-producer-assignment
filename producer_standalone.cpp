void *producer (void *id) 
{
  int *producer_id = (int*)id;
  cout << "\nStarted producer thread = " <<  *producer_id << endl; 
  
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

    do {
        clock_gettime(CLOCK_REALTIME, &ts_producer);
        ts_producer.tv_sec += 1; i++; //cout << "at t = " << i << endl;

      //sem_wait(&empty_count); - REPLACE WITH LOOPED VERSION OF TIMEOUT CHECKER
      sem_wait(&queue_access_mutex);
      
      Q.push_back(J);     // every 5 seconds = already slept
      
      sem_post(&queue_access_mutex);
      sem_post(&full_count);

        std::ofstream ofs("output2.txt", std::ofstream::out);
          cout << "Producer("<< *producer_id << "): Job id " << job_id << " sleeping for " << sleep_time << " seconds" << endl;
          cout << "Producer("<< *producer_id << "): Job id " << job_id << " duration " << duration << " seconds" << endl;
        ofs.close();

        if (i == 20) {sem_post(&empty_count);} // BREAK OUT

    } while (sem_timedwait( &empty_count, &ts_consumer ) == -1 );

/*
    ts_producer.tv_sec += 20;     cout << "..producer waiting 20 seconds..";
    if (sem_timedwait(&empty_count, &ts_producer)!=-1) {wait_within_time_limit = false; break;}  // 20 seconds
    cout << "..done waiting..";
*/
      } // for loop ends

  pthread_exit(0);  
}
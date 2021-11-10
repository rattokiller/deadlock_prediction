
#include <iostream>

#include <chrono>
#include <thread>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <sys/time.h>

#include "utils.h"
#include "mutex_detect.h"

using namespace std;

int status[N_max];

bool my_terminate;

void init_thread(periodic_thread *th){

}
void * save_page (void * par) {
	
    mutex_detect& m = mutex_detect::getInstance();
    struct periodic_thread *th = (struct periodic_thread *) par;
    int i = th->index;

    struct sched_attr attr;
	sched_getattr(0, &attr, sizeof(attr), 0);

	attr.sched_policy = SCHED_FIFO;//SCHED_DEADLINE;
	attr.sched_runtime = th->wcet * 1000; //ns	
	attr.sched_deadline = th->period * 1000; //ns
	attr.sched_period = th->period * 1000; //ns
    attr.sched_priority = 50;
	sched_setattr(0, &attr, 0);
	
	

    //cout<<"pid : "<<gettid()<<endl;;
    status[i]=-2;
    m.th_id[i]=get_id();
    
    //int j = (i+1)%N_max;
    int j = rand()%N_max;
    int k = rand()%N_max;
    bool deadlook;
    start_periodic_timer(th,100);//us di offset

    for(int cnt=0;cnt<10;cnt++)
    {
        do
        {
            deadlook=false;
            status[i]=i;
            if(m.my_lock(i)!=0)
                cout<<"Errore - falsa deadlook rilevata"<<endl;
            status[i]=-3;
            
            //cout <<HRED<<gettid()<<" blocco il dato "<<i<<RST<<endl;
            // simulate a long page fetch
            this_thread::sleep_for(std::chrono::microseconds(5+rand()%Rand_max_time));

            status[i]=100*j+i;
            if(m.my_lock(j)>0){
                deadlook=true;
                #ifdef debug_info
                    cout<<HRED<<"\tRisolvo deadlook - 1"<<RST<<endl;
                #endif
                #ifdef debug
                cout<< m;
                #endif
                m.my_unlock(i);
                //this_thread::sleep_until(std::chrono::seconds(1));
                this_thread::sleep_for(std::chrono::milliseconds(1+rand()%Rand_max_time));
            }
            
            this_thread::sleep_for(std::chrono::microseconds(5+rand()%Rand_max_time));

            status[i]=10000*k+100*j+i;
            
            if(!deadlook && m.my_lock(k)>0){
                deadlook=true;
                #ifdef debug_info
                    cout<<HRED<<"\tRisolvo deadlook - 2"<<RST<<endl;
                #endif
                #ifdef debug
                cout<< m;
                #endif
                m.my_unlock(i);
                m.my_unlock(j);
                //this_thread::sleep_until(std::chrono::seconds(1));
                this_thread::sleep_for(std::chrono::milliseconds(1+rand()%Rand_max_time));
            }
            status[i]=-4;
        }
        while(deadlook && my_terminate);
        status[i]=-5;
        //cout<<HRED<<gettid()<<" blocco Dentro  dato "<<i<<"  di "<< j<<RST<<endl;
        // simulate a long page fetch
        //this_thread::sleep_for(std::chrono::microseconds(50+2*rand()%Rand_max_time));
        //wait_next_activation(th);

        
        m.my_unlock(k);
        m.my_unlock(j);
        m.my_unlock(i);
        
        status[i]=-100;

        wait_next_activation(th);
    }
	#ifdef debug
   		cout<<HMAG<<"End Process "<<i<<RST<<endl;
   	#endif
 
}

int main_generate()
{
	int ris = 0;
    mutex_detect& m = mutex_detect::getInstance();
	
    //int i=0;
   	pthread_t t[N_max];
    struct periodic_thread th[N_max];
	

   	my_terminate=true;
   	
   	for(int i=0;i<N_max;i++)
   		status[i]=-1;
   		
   		
    for(int i=0;i<N_max;i++){
        th[i].index=i;
        
        th[i].period = 50000;
        th[i].wcet = 40000;

        pthread_create(&m.t[i], NULL, save_page, &th[i]);   
        //cout<<"th "<<m.t[i]<<endl; 
   		//t[i] = thread(save_page, i);
    }
        
   	
   	
   	
    
    //this_thread::sleep_for(std::chrono::milliseconds(50));
    
        
    void* res;
    for(int i=0;i<N_max;i++)
        pthread_join(m.t[i], &res);
    	//t[i].join();   


    my_terminate=false;
    bool test = true;
    
    #ifdef debug
		cout<< m;
		cout<<"  Status:\n\t -1: no start"<<endl;
		cout<<"\t -2 to -5 :start th"<<endl;
		cout<<"\t -100 :start th"<<endl;
		cout<<"\t  0 to "<<N_max<<" lock to risors i"<<endl<<endl;
	#endif
    
    
    for(int i=0;i<N_max;i++){
    	#ifdef debug
    		cout<<"["<<i<<"]\tth["<<m.th_id[i]<<"]\t"<<status[i]<<endl;
    	#endif
    	test = test && (status[i] == -100);
    }
    
    if(test){
    	#ifdef debug_info
    		cout<<"Processo Terminato BENE"<<endl;
    	#endif
    }
    else 
    {
    	cout<<HRED<<"\t =======\tProcesso Terminato MALE   <<<======="<<RST<<endl;
    	ris=1;
    }

    	
     
    
   // pthread_join(thread_1, &res);
   	
   	
    //this_thread::sleep_for(std::chrono::seconds(1));
    //cout<< m;


    // safe to access g_pages without lock now, as the threads are joined
	return ris;
}


int main(int argc,char **argv)
{
    
	int N=1;
	struct timeval tv;
	srand (time(NULL));
	int fail=0;
	uint64_t t,start;
	cout<<"Start [N_risorse "<<N_max<<"]"<<endl<<endl;
	if(argc > 1)
		N=atoi(argv[1]);
		
	for(int i=0;i<N;i++){
	
			cout<<HCYN<<"Prova N[ "<<i+1<<" ]"<<RST;
			gettimeofday(&tv, NULL);
			start = tv.tv_sec * 1000000ULL + tv.tv_usec;//lo start va in microsecondi altrimenti sto perdendo informazioni

			fail += main_generate();
			gettimeofday(&tv, NULL);
			t = tv.tv_sec * 1000000ULL + tv.tv_usec;
			
			cout<<"\t-\t"<<(t-start)/1000<<" ms"<<endl;
	
	}
	
	cout <<"Numero di processi falliti: " <<fail<<"/"<<N<<endl;

}

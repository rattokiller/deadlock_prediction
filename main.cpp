
#include <iostream>

#include <chrono>
#include <thread>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "mutex_detect.h"

using namespace std;

int status[N_max];
int th_id[N_max];
bool my_terminate;
void save_page(const int i)
{

    //cout<<"pid : "<<gettid()<<endl;;
    status[i]=-2;
    th_id[i]=get_id();
    mutex_detect& m = mutex_detect::getInstance();
    //int j = (i+1)%N_max;
    int j = rand()%N_max;
    int k = rand()%N_max;
    bool deadlook;
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
    this_thread::sleep_for(std::chrono::microseconds(50+2*rand()%Rand_max_time));


	  
	m.my_unlock(k);

	m.my_unlock(j);

	m.my_unlock(i);
	status[i]=-100;
	
	#ifdef debug
   		cout<<HMAG<<"End Process "<<i<<RST<<endl;
   	#endif
 
}

int main_generate()
{
	int ris = 0;
	
    //int i=0;
   	thread t[N_max];
   	my_terminate=true;
   	
   	for(int i=0;i<N_max;i++)
   		status[i]=-1;
   		
   		
    for(int i=0;i<N_max;i++)
   		t[i] = thread(save_page, i);
   	
   	
   	
    mutex_detect& m = mutex_detect::getInstance();
    this_thread::sleep_for(std::chrono::milliseconds(30));
    

    /*
    t1.join();
    t2.join();
    t3.join();
    */
    
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
    		cout<<"["<<i<<"]\tth["<<th_id[i]<<"]\t"<<status[i]<<endl;
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
    for(int i=0;i<N_max;i++)
    	t[i].join();   
    	
    
   	
   	
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


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
    th_id[i]=gettid();
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
        this_thread::sleep_for(std::chrono::milliseconds(2+rand()%Rand_max_time));

		status[i]=j;
        if(m.my_lock(j)>0){
            deadlook=true;
            cout<<HRED<<"\tRisolvo deadlook - 1"<<RST<<endl;
               cout<< m;
            m.my_unlock(i);
            //this_thread::sleep_until(std::chrono::seconds(1));
            this_thread::sleep_for(std::chrono::milliseconds(20+rand()%Rand_max_time));
        }
        status[i]=k;
        
        if(!deadlook && m.my_lock(k)>0){
            deadlook=true;
            cout<<HRED<<"\tRisolvo deadlook - 2"<<RST<<endl;
               cout<< m;
            m.my_unlock(i);
            m.my_unlock(j);
            //this_thread::sleep_until(std::chrono::seconds(1));
            this_thread::sleep_for(std::chrono::milliseconds(20+rand()%Rand_max_time));
        }
        status[i]=-4;
    }
    while(deadlook && my_terminate);
    //cout<<HRED<<gettid()<<" blocco Dentro  dato "<<i<<"  di "<< j<<RST<<endl;
    // simulate a long page fetch
    this_thread::sleep_for(std::chrono::milliseconds(2));


	  
   m.my_unlock(k);
   
   m.my_unlock(j);

   m.my_unlock(i);
   status[i]=-100;
 
}

int main_generate()
{
    //int i=0;
   	thread t[N_max];
   	my_terminate=true;
    for(int i=0;i<N_max;i++)
   		t[i] = thread(save_page, i);
   	for(int i=0;i<N_max;i++)
   		status[i]=-1;
   	
   	
    mutex_detect& m = mutex_detect::getInstance();
    this_thread::sleep_for(std::chrono::seconds(1));
    

    /*
    t1.join();
    t2.join();
    t3.join();
    */
    cout<< m;
    cout<<"status:\n\t-1: no start"<<endl;
    cout<<"\t -2 :start th"<<endl;
    cout<<"\t -3 :start th"<<endl;
    cout<<"\t -4 :start th"<<endl;
    cout<<"\t -100 :start th"<<endl;
    cout<<"\t 0 to "<<N_max<<" lock to risors i"<<endl;
    
    my_terminate=false;
    for(int i=0;i<N_max;i++)
    	cout<<"["<<i<<"]\tth["<<th_id[i]<<"]\t"<<status[i]<<endl;
    
    
    for(int i=0;i<N_max;i++)
    	t[i].join();
    	
    
   	
   	
    //this_thread::sleep_for(std::chrono::seconds(1));
    //cout<< m;


    // safe to access g_pages without lock now, as the threads are joined
	return 0;
}

int main(int argc,char **argv)
{
	int N=1;
	srand (time(NULL));
	
	cout<<"Start [N_risorse "<<N_max<<"]"<<endl<<endl;
	if(argc > 1)
		N=atoi(argv[1]);
		
	for(int i=0;i<N;i++){
	cout<<HCYN<<"Prova N[ "<<i+1<<" ]"<<RST<<endl;
	
			main_generate();
	}

}

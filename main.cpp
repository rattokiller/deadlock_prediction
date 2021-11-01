
#include <iostream>

#include <chrono>
#include <thread>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "mutex_detect.h"

//High intensty text
#define HBLK "\e[0;90m"
#define HRED "\e[0;91m"
#define HGRN "\e[0;92m"
#define HYEL "\e[0;93m"
#define HBLU "\e[0;94m"
#define HMAG "\e[0;95m"
#define HCYN "\e[0;96m"
#define HWHT "\e[0;97m"

//Reset
#define RST "\e[0m"
#define Rand_max_time 10
#define Rand_max_id 5
using namespace std;

void save_page(const int i)
{

    //cout<<"pid : "<<gettid()<<endl;;
    mutex_detect& m = mutex_detect::getInstance();
    int j = (i+1)%N_max;
    j = (i+1+rand()%3)%N_max;
    bool deadlook;
    do
    {
        deadlook=false;
        if(m.my_lock(i)!=0)
            cout<<"Errore - falsa deadlook rilevata"<<endl;
		
		
        //cout <<HRED<<gettid()<<" blocco il dato "<<i<<RST<<endl;
        // simulate a long page fetch
        this_thread::sleep_for(std::chrono::milliseconds(2+rand()%Rand_max_time));


        if(m.my_lock(j)!=0){
            deadlook=true;
            cout<<HRED<<"Risolvo deadlook"<<RST<<endl;
               cout<< m;
            m.my_unlock(i);
            //this_thread::sleep_until(std::chrono::seconds(1));
            this_thread::sleep_for(std::chrono::milliseconds(2+rand()%Rand_max_time));
        }
    }
    while(deadlook);

    //cout<<HRED<<gettid()<<" blocco Dentro  dato "<<i<<"  di "<< j<<RST<<endl;
    // simulate a long page fetch
    this_thread::sleep_for(std::chrono::milliseconds(2));


   m.my_unlock(j);

   m.my_unlock(i);
}

int main_generate()
{
    //int i=0;
   	thread t[N_max];
   	for(int i=0;i<N_max;i++)
   		t[i] = thread(save_page, i);
   	
    /*
    thread t1(save_page, i++);
    thread t2(save_page, i++);
    thread t3(save_page, i++);
    */
    mutex_detect& m = mutex_detect::getInstance();
    this_thread::sleep_for(std::chrono::seconds(1));
    

    /*
    t1.join();
    t2.join();
    t3.join();
    */
    cout<< m;
    for(int i=0;i<N_max;i++)	t[i].join();
   	
   	
    //this_thread::sleep_for(std::chrono::seconds(1));
    //cout<< m;


    // safe to access g_pages without lock now, as the threads are joined
	return 0;
}

int main(int argc,char **argv)
{
	int N=1;
	srand (time(NULL));
	
	cout<<"Start [N="<<N_max<<"]"<<endl<<endl;
	if(argc > 1)
		N=atoi(argv[1]);
		
	for(int i=0;i<N;i++){
	cout<<HCYN<<"Prova N[ "<<i+1<<" ]"<<RST<<endl;
			main_generate();
	}

}

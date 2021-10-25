
#include <iostream>

#include <chrono>
#include <thread>
#include "mutex_detect.h"


using namespace std;

void save_page(const int i)
{

    mutex_detect& m = mutex_detect::getInstance();
    int j = (i+1)%2;
    m.my_lock(i);
    cout <<" - blocco e consumo il dato "<<i<<endl;
    // simulate a long page fetch
    this_thread::sleep_for(std::chrono::seconds(2));



    m.my_lock(j);
    cout<<"blocco Dentro consumo il dato "<<i<<"  di "<< j<<endl;
    // simulate a long page fetch
    this_thread::sleep_for(std::chrono::seconds(2));


   m.my_unlock(j);

   m.my_unlock(i);
}

int main()
{
    int i=0;
    thread t1(save_page, i++);
    thread t2(save_page, i++);
    this_thread::sleep_for(std::chrono::seconds(3));
    mutex_detect& m = mutex_detect::getInstance();

    cout<< m;
    t1.join();
    t2.join();

    // safe to access g_pages without lock now, as the threads are joined

}
#include <iostream>

#include "thread_utils.h" 

#include <thread>
#include <cstdlib>
#include <unistd.h>

using namespace std;
void cosadafare(){
    cout<<"Sono una procedura!"<<endl;
}

class Dummy
{
public :
    void operator()()
    {
        for(int a=0;a<5;a++){
            cout<<"Sono un oggetto, e quindi sono meglio  di una procedura! "<< a <<endl;
            usleep( 1000 * ( rand() % 9 + 1 ));
        }
    }
};

void uccidimi(){
    for(int a=0;a<50;a++){
        cout<<"Io parlo troppo "<< a <<endl;
        usleep( 1000 * ( rand() % 9 + 1 ));
    }
}
/*
int main()
{
    Dummy w;
    std::thread t1(w);
    std::thread t2(cosadafare);
    std::thread t3(uccidimi);

    t1.join();
    t2.join();
    t3.detach();

    return 0;
};
 */
#include <iostream>
#include <map>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>


std::mutex g_pages_mutex[10];

void save_page(const int i)
{
    // simulate a long page fetch
    std::this_thread::sleep_for(std::chrono::seconds(2));


    g_pages_mutex[i].lock();
    cout<<"blocco e consumo il dato "<<i<<endl;

    g_pages_mutex[i].unlock();
}

int main()
{
    int i=0;
    std::thread t1(save_page, i++);
    std::thread t2(save_page, i++);
    t1.join();
    t2.join();

    // safe to access g_pages without lock now, as the threads are joined

}
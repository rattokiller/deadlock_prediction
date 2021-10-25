//
// Created by filip on 19/10/2021.
//

#include "mutex_detect.h"


void mutex_detect::write_lock(int id_risorsa){
    data_mutex.lock();
    getpid();
    stato_risorse[id_risorsa].push_back(std::this_thread::get_id());//sostituire con pid

    data_mutex.unlock();
}

void mutex_detect::write_unlock(int id_risorsa){
    data_mutex.lock();

    stato_risorse[id_risorsa].pop_back();//sostituire con pid

    data_mutex.unlock();
}

int mutex_detect::my_lock(int id_risorsa) {

    g_pages_mutex[id_risorsa].lock();

    write_lock(id_risorsa);

    return -1;
}

int mutex_detect::my_unlock(int id_risorsa) {

    g_pages_mutex[id_risorsa].unlock();

    write_unlock(id_risorsa);

    return -1;
}

std::ostream& operator<<(std::ostream& os, const mutex_detect& dt)
{
    os<<"Dati mutex struct"<<std::endl;
    for(int i=0;i<N_max;i++){

        for(auto i : dt.stato_risorse[i])
            os<<i<<" - ";

        os<<std::endl;
    }

    return os;
}


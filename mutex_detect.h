//
// Created by filip on 19/10/2021.
//

#ifndef DEADLOCK_PREDICTION_MUTEX_DETECT_H
#define DEADLOCK_PREDICTION_MUTEX_DETECT_H

#include <mutex>
#include <vector>
#include <iostream>
#include <thread>

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

#define N_max 20

#define Rand_max_time 10
#define Rand_max_id 5
 class mutex_detect {
private:
  
    std::mutex g_pages_mutex[N_max];
    std::mutex data_mutex;
    std::vector<int> stato_risorse[N_max];

    int  test_deadlock(int id_risorsa);
    void write_lock(int id_risorsa);
    void clear_write_lock(int id_risorsa);
    void write_unlock(int id_risorsa);

public:
    mutex_detect();
    static mutex_detect& getInstance()
    {
        static mutex_detect    instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    int my_lock(int id_risorsa);
    int my_unlock(int id_risorsa);

    friend std::ostream& operator<<(std::ostream& os, const mutex_detect& dt);
};


/*
 * Regole
 * (1) - prima di una lock la my_lock controlla se quella risorsa non è in "attesa" del processo
 * (2) - prima della lock si controlla se è occupata e metto metto in attesa tutte le mie risorse sul pid in attesa della risorsa
 * (3) - Dopo la lock si segna il proprio pid nel attesa di quella risorsa ( la risorsa attende quel processo per essere libera)
 * (4) - si cancella l' attesa del proceso esterno sulle proprie risorse
 * (5) - prima della unlock si cancella il proprio pid dalla lista di attesa della risorsa
 * (6) - dopo la unlook
 */

#endif //DEADLOCK_PREDICTION_MUTEX_DETECT_H

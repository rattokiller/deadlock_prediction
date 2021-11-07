//
// Created by filip on 19/10/2021.
//

#ifndef _DEPENDENCY_STRUCT_H_
#define _DEPENDENCY_STRUCT_H_

#include <mutex>
#include <vector>
#include <iostream>


#include "utils.h"

class dependency_struct
{
	private:
	  
		 
		std::mutex data_mutex;
		
		std::vector<int> stato_risorse[N_max];

		dependency_struct(){
			for(int i=0;i<N_max;i++)
		    		stato_risorse[i].push_back(-1);
		}
		
		

	public:
		

		static dependency_struct& getInstance()
		{
		    static dependency_struct    instance; // Guaranteed to be destroyed.
		    // Instantiated on first use.
		     
		    return instance;
		}
		
		
		int  test_deadlock(int id_risorsa);
		void write_lock(int id_risorsa);
		
		void clear_write_lock(int id_risorsa);
		void write_unlock(int id_risorsa);

		

		friend std::ostream& operator<<(std::ostream& os, const dependency_struct& dt);
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

#endif _DEPENDENCY_STRUCT_H_

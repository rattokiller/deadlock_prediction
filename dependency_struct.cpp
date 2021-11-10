
#include <mutex>
#include <vector>
#include <iostream>

#include "utils.h"
#include "dependency_struct.h"

using namespace std;

	
int  dependency_struct::test_deadlock(int id_risorsa)
{//strudura doc
          
        //std::cout<<"Test... ";
        int id = get_id();//verificare se chiedo la risorsa a me stesso
        if (stato_risorse[id_risorsa][0]==id){
        #ifdef debug_info
        	std::cout<<"["<<id<<"]\tDeadlook Rilevata\t Risorsa["<<id_risorsa<<"] - Gia posseduta !"<<std::endl;
        #endif
                //data_mutex.unlock();
            return  -1;
       	}
        
        for (int i=1;i<stato_risorse[id_risorsa].size();i++) {//si puo saltare il primo cilco
        auto item = stato_risorse[id_risorsa][i];
            //std::cout<<id<<" - "<<item<<" - ";
            
            if (stato_risorse[stato_risorse[id_risorsa][i]][0] == id) {
            //devo controlare con tutte le mie risore
            	#ifdef debug_info
                	std::cout<<"["<<id<<"]\tDeadlook Rilevata\t Risorsa["<<id_risorsa<<"]"<<std::endl;
                #endif
                //data_mutex.unlock();
                return  stato_risorse[id_risorsa][0];//oppure stato_risorse[id_risorsa][i];
                //trovare la risorsa associata crea torppi problemi di gestione, il processo deve rilasciare tutto
            }         
        }
             
    return 0;
}

int dependency_struct::get_pid_risorsa(int id_risorsa)
{
	return stato_risorse[id_risorsa][0];
}


void dependency_struct::clear_write_lock(int id_risorsa)
{//strudura doc
	stato_risorse[id_risorsa][0]=get_id();
	int id = get_id();
	
	for(int i=0;i<N_max;i++)
    	if(stato_risorse[i][0] == id)
    	{
			stato_risorse[i].clear();
			stato_risorse[i].push_back(id);		
		}
		
}

void dependency_struct::write_lock(int id_risorsa)
{//strudura doc
    static int max_size=0;
    int id = get_id();
   	bool continua ;

	//stato_risorse[id_risorsa][0]= gettid();
	
	for(int i=0;i<N_max;i++){
	
    	if(stato_risorse[i][0] == id){
    		stato_risorse[i].push_back(id_risorsa);
    		
    		for(int j=1;j<stato_risorse[id_risorsa].size(); j++)
    			stato_risorse[i].push_back(stato_risorse[id_risorsa][j]);
    	}	
    }
    
    for(int i=0;i<N_max;i++){//sta un problema grave
    	continua = true;
    	
    	//la risorsa posseduta è blocata da quella richiesta piu tutte le risorse che blocano quella richiesta
   
    	
        for(int j=1;j<stato_risorse[i].size() && continua;j++){
	        if (stato_risorse[stato_risorse[i][j]][0] == id)
	            continua = false;
	            if(stato_risorse[i].size()>3*N_max+1)
					if(max_size<stato_risorse[i].size()){
						max_size=stato_risorse[i].size();
						cout<<"CATTIVA OTTIMIZAZIONE  "<<max_size<<endl;
	            //*(int*)(10)=2;//Segmentation fault
	            
					}
	            
	                       	
	    }   
		         
		if(continua == false){
			stato_risorse[i].push_back(id_risorsa);
			for(int j=1;j<stato_risorse[id_risorsa].size(); j++)
    			stato_risorse[i].push_back(stato_risorse[id_risorsa][j]);
		}  
	}   
}


void dependency_struct::write_unlock(int id_risorsa)
{//strudura doc
	//bug
	stato_risorse[id_risorsa].clear();
	stato_risorse[id_risorsa].push_back(-1);
	
	return;
    //stato_risorse[id_risorsa][0]=-1;
    
    
}

int dependency_struct::max_size_threadX(){

	int pid_th = -1;
	int max_size = 0;

	for(int i=0;i<N_max;i++)
		if(max_size < stato_risorse[i].size() && stato_risorse[i][0] != -1){
			max_size = stato_risorse[i].size();
			pid_th = stato_risorse[i][0];
		}
		else if(stato_risorse[i].size() > 1 && stato_risorse[i][0] == -1)
			cout<<"ERRORE STATO INCONSISTENTE!!" <<endl;

	return pid_th;

}

std::ostream& operator<<(std::ostream& os, const dependency_struct& dt)
{
	os<<"["<<get_id()<<"]\tDati mutex struct"<<std::endl<<std::endl;
    os<<"\tRisorsa\t P   - R_lock"<<std::endl;
    for(int i=0;i<N_max;i++){
    
        os<<"\tR["<<i<<"]\t";
        for(auto i : dt.stato_risorse[i])
            os<<i<<" - ";

        os<<std::endl;
    }
    os<<std::endl;

    return os;
}




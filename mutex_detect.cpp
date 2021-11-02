//
// Created by filip on 19/10/2021.
//

#include "mutex_detect.h"
using namespace std;
 mutex_detect::mutex_detect(){
    for(int i=0;i<N_max;i++)
        stato_risorse[i].push_back(-1);
}

int mutex_detect::test_deadlock(int id_risorsa){
    //data_mutex.lock();
    //if(stato_risorse[id_risorsa][0]!=-1){
            
        //std::cout<<"Test... ";
        int id = gettid();//verificare se chiedo la risorsa a me stesso
        if (stato_risorse[id_risorsa][0]==id){
        
        	std::cout<<"["<<gettid()<<"]\tDeadlook Rilevata\t Risorsa["<<id_risorsa<<"] - Gia posseduta !"<<std::endl;
                //data_mutex.unlock();
            return  -1;
       	}
        
        for (int i=1;i<stato_risorse[id_risorsa].size();i++) {//si puo saltare il primo cilco
        auto item = stato_risorse[id_risorsa][i];
            //std::cout<<id<<" - "<<item<<" - ";
            
            if (stato_risorse[stato_risorse[id_risorsa][i]][0] == id) {
            //devo controlare con tutte le mie risore


            
                std::cout<<"["<<gettid()<<"]\tDeadlook Rilevata\t Risorsa["<<id_risorsa<<"]"<<std::endl;
                //data_mutex.unlock();
                return  stato_risorse[id_risorsa][0];
                //trovare la risorsa associata crea torppi problemi di gestione, il processo deve rilasciare tutto
            }
        }
       
        //std::cout<<"ok"<<std::endl;
        
    return 0;
}
//libera la coda di tutte le risorse posedute dal' ogetto
void mutex_detect::clear_write_lock(int id_risorsa){
	int id = gettid();
	
	for(int i=0;i<N_max;i++)
	
    	if(stato_risorse[i][0] == id){
			stato_risorse[i].clear();
			stato_risorse[i].push_back(id);
		
		}
}
void mutex_detect::write_lock(int id_risorsa){
    //data_mutex.lock();
    int id = gettid();
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
	            if(stato_risorse[i].size()>3*N_max+1){
	            cout<<"Errore Critico BUG"<<endl;
	            //*(int*)(10)=2;//Segmentation fault
	            
	            }
	            	
	    }   
		         
		if(continua == false){
			stato_risorse[i].push_back(id_risorsa);
			for(int j=1;j<stato_risorse[id_risorsa].size(); j++)
    			stato_risorse[i].push_back(stato_risorse[id_risorsa][j]);
		}
        
        
	}
    //data_mutex.unlock();
}

void mutex_detect::write_unlock(int id_risorsa){
	//bug
	stato_risorse[id_risorsa].clear();
	stato_risorse[id_risorsa].push_back(-1);
	return;
    //stato_risorse[id_risorsa][0]=-1;
    int id = gettid();
    for(int i=0;i<N_max;i++)
    	for(int j=1;j<stato_risorse[i].size();j++)
    		if (stato_risorse[i][j] == id_risorsa )
                stato_risorse[i].erase(stato_risorse[i].begin()+j);
   
   
}

int mutex_detect::my_lock(int id_risorsa) {

	int ris = 0;
    data_mutex.lock();
    
    if(stato_risorse[id_risorsa][0]==-1){
		stato_risorse[id_risorsa][0]=gettid();
		
		data_mutex.unlock();
		//cout<<"risorsa libera"<<endl;
		
		
		g_pages_mutex[id_risorsa].lock();
		
    }
    else if(stato_risorse[id_risorsa][0]==gettid()){
    	std::cout<<HGRN<<"["<<gettid()<<"]\tDeadlook Rilevata\t Risorsa["<<id_risorsa<<"] - Gia posseduta !"<<RST<<std::endl;
    	data_mutex.unlock();
			
    }
    else
    {
    	cout<<gettid()<<" - Mi blochero su "<<id_risorsa<<endl;
    	
		ris= test_deadlock(id_risorsa);
		if(ris == 0){
			//stato_risorse[id_risorsa][0]=gettid();
			write_lock(id_risorsa);
			data_mutex.unlock();
			g_pages_mutex[id_risorsa].lock();
			
			data_mutex.lock();
			//tutte le mie risorse non sono in attesa di niente
			clear_write_lock(id_risorsa);
			stato_risorse[id_risorsa][0]=gettid();
			data_mutex.unlock();
		}
		else if(ris == -1){
			data_mutex.unlock();
			
		}
		else{
			data_mutex.unlock();
			//deadlock detect
		}
    }
    
    return ris;
}

int mutex_detect::my_unlock(int id_risorsa) {
	data_mutex.lock();
	//cout<<gettid()<<" - libero risorsa "<<id_risorsa<<endl;
	if(stato_risorse[id_risorsa][0]==gettid()){
		write_unlock(id_risorsa);
		stato_risorse[id_risorsa][0]=-1;
		
		data_mutex.unlock();
		
		g_pages_mutex[id_risorsa].unlock();
	}
	else
	{
		cout<<gettid()<<" - Risorsa non posseduta - "<<id_risorsa<<endl;
		data_mutex.unlock();
	}
    
    
	
    return -1;
}

std::ostream& operator<<(std::ostream& os, const mutex_detect& dt)
{
    os<<"["<<gettid()<<"]\tDati mutex struct"<<std::endl<<std::endl;
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


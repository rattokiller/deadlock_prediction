//
// Created by filip on 19/10/2021.
//

#include "mutex_detect.h"

using namespace std;

mutex_detect::mutex_detect(){
    for(int i=0;i<N_max;i++)
        stato_risorse[i].push_back(-1);
}



int mutex_detect::test_deadlock(int id_risorsa)
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
//libera la coda di tutte le risorse posedute dal' ogetto
void mutex_detect::clear_write_lock(int id_risorsa)
{//strudura doc
	int id = get_id();
	
	for(int i=0;i<N_max;i++)
	
    	if(stato_risorse[i][0] == id){
			stato_risorse[i].clear();
			stato_risorse[i].push_back(id);
		
		}
}
void mutex_detect::write_lock(int id_risorsa)
{//strudura doc
    
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
    
    for(int i=0;i<N_max;i++){//sta un problema grave di performance
    	continua = true;
    	
    	//la risorsa posseduta è blocata da quella richiesta piu tutte le risorse che blocano quella richiesta
   
    	
        for(int j=1;j<stato_risorse[i].size() && continua;j++){
	        if (stato_risorse[stato_risorse[i][j]][0] == id)
	            continua = false;
	            if(stato_risorse[i].size()>3*N_max+1){
	            cout<<"Errore Critico BUG "<<stato_risorse[i].size()<<endl;
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


void mutex_detect::write_unlock(int id_risorsa)
{//strudura doc
	//bug
	stato_risorse[id_risorsa].clear();
	stato_risorse[id_risorsa].push_back(-1);
	return;
    //stato_risorse[id_risorsa][0]=-1;
    int id = get_id();
    for(int i=0;i<N_max;i++)
    	for(int j=1;j<stato_risorse[i].size();j++)
    		if (stato_risorse[i][j] == id_risorsa )
                stato_risorse[i].erase(stato_risorse[i].begin()+j);
   
   
}

int mutex_detect::my_lock(int id_risorsa) {

	int ris = 0;
    data_mutex.lock();
    
    if(stato_risorse[id_risorsa][0]==-1){
		
		//cout<<"risorsa libera"<<endl;
		
		//stato_risorse[id_risorsa][0]=get_id();
		#ifdef debug
			cout<<get_id()<<" - Prendo  "<<id_risorsa<<endl;
		#endif
		write_lock(id_risorsa);
		data_mutex.unlock();
		
		g_pages_mutex[id_risorsa].lock();//questa lock non si puo mai blocarsi
		
		data_mutex.lock();
		
		stato_risorse[id_risorsa][0]=get_id();
		clear_write_lock(id_risorsa);
		//post_lock(id_risorsa);

    }
    else if(stato_risorse[id_risorsa][0]==get_id()){
    	#ifdef debug_info
    		std::cout<<HGRN<<"["<<get_id()<<"]\tDeadlook Rilevata\t Risorsa["<<id_risorsa<<"] - Gia posseduta !"<<RST<<std::endl;
    	#endif
    	//data_mutex.unlock();
			
    }
    else
    {
    	#ifdef debug
    		cout<<get_id()<<" - Mi blochero su "<<id_risorsa<<endl;
    	#endif
		ris= test_deadlock(id_risorsa);
		if(ris == 0){
			//stato_risorse[id_risorsa][0]=gettid();
			write_lock(id_risorsa);
			
			data_mutex.unlock();
			g_pages_mutex[id_risorsa].lock();
			
			data_mutex.lock();
			
			//tutte le mie risorse non sono in attesa di niente
			stato_risorse[id_risorsa][0]=get_id();
			clear_write_lock(id_risorsa);
			
			
			
		}
		else if(ris == -1){
			
		}
		else{
			// g_pages_mutex[id_risorsa].lock();//questa lock dovrebbe blocarsi sempre (per sempre)
		
			//deadlock detect
		}
		//data_mutex.unlock();
    }
    data_mutex.unlock();
    return ris;
}

int mutex_detect::my_unlock(int id_risorsa) {
	data_mutex.lock();
	#ifdef debug
		cout<<get_id()<<" - libero risorsa "<<id_risorsa;
	#endif
	if(stato_risorse[id_risorsa][0]==get_id()){
		write_unlock(id_risorsa);
		//stato_risorse[id_risorsa][0]=-1;
		
		
		//data_mutex.unlock();
		g_pages_mutex[id_risorsa].unlock();
	}
	else
	{
		#ifdef debug
			cout<<" --  "<<get_id()<<" - Risorsa non posseduta - "<<id_risorsa;
		#endif
		//data_mutex.unlock();
	}
	#ifdef debug
		cout<<"  ok"<<endl;
	#endif
	
    data_mutex.unlock();
    
	
    return -1;
}


std::ostream& operator<<(std::ostream& os, const mutex_detect& dt)//strudura doc
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




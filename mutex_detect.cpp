//
// Created by filip on 19/10/2021.
//

#include "mutex_detect.h"

using namespace std;

int mutex_detect::my_lock(int id_risorsa) {

	int ris = 0;
    data_mutex.lock();
    
    int pid_risorsa = data.get_pid_risorsa(id_risorsa);
    
    if(pid_risorsa == -1){
		
		//cout<<"risorsa libera"<<endl;
		
		//stato_risorse[id_risorsa][0]=get_id();
		#ifdef debug
			cout<<get_id()<<" - Prendo  "<<id_risorsa<<endl;
		#endif
		data.write_lock(id_risorsa);
		
		data_mutex.unlock();
		
		g_pages_mutex[id_risorsa].lock();//questa lock non dovrebbe mai blocarsi
		
		data_mutex.lock();
				
		data.clear_write_lock(id_risorsa);
		//post_lock(id_risorsa);

    }
    else if( pid_risorsa == get_id()){
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
    	
		ris= data.test_deadlock(id_risorsa);
		if(ris == 0){
			//stato_risorse[id_risorsa][0]=gettid();
			data.write_lock(id_risorsa);
			
			data_mutex.unlock();
			
			g_pages_mutex[id_risorsa].lock();
			
			data_mutex.lock();
			
			//tutte le mie risorse non sono in attesa di niente
			
			data.clear_write_lock(id_risorsa);
		
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
	if(data.get_pid_risorsa(id_risorsa)==get_id()){
		data.write_unlock(id_risorsa);
	
		
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
	os<<dt.data;
	return os;

}




#ifndef _THREAD_UTILS_H_
#define _THREAD_UTILS_H_


#include "utils.h"

#define _GNU_SOURCE
#include <unistd.h>
#include <thread>

int get_id(){
	#ifdef linux
		return gettid();
	#else
		
		return std::this_thread::get_id(); //win
	#endif


}

#endif

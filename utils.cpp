

#include "utils.h"

//#define _GNU_SOURCE
#include <unistd.h>

int get_id(){
	#ifdef my_linux
		return gettid();
	#else
		return std::this_thread::get_id(); //win
	#endif


}



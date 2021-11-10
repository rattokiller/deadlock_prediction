#ifndef _UTILS_H_
#define _UTILS_H_



#include <linux/types.h>
#include <unistd.h>
#include <cstdint>
#include <sys/syscall.h>
#include <pthread.h>


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

#define N_max 30

#define my_linux
//#define debug
//#define debug_info

#define Rand_max_time 8
#define Rand_max_id 5


#define NSEC_PER_SEC 1000000000ULL

#define gettid() syscall(__NR_gettid)

#define SCHED_DEADLINE  6

/* XXX use the proper syscall numbers */
#ifdef __x86_64__
#define __NR_sched_setattr      314
#define __NR_sched_getattr      315
#endif

#ifdef __i386__
#define __NR_sched_setattr      351
#define __NR_sched_getattr      352
#endif

#ifdef __arm__
#define __NR_sched_setattr      380
#define __NR_sched_getattr      381
#endif

static volatile int done;

struct sched_attr {
    __u32 size;

    __u32 sched_policy;
    __u64 sched_flags;

    /* SCHED_NORMAL, SCHED_BATCH */
    __s32 sched_nice;

    /* SCHED_FIFO, SCHED_RR */
    __u32 sched_priority;

    /* SCHED_DEADLINE (nsec) */
    __u64 sched_runtime;
    __u64 sched_deadline;
    __u64 sched_period;
};

struct periodic_thread {
	int index;
	struct timespec r;
	int period;
	int wcet;
	int priority;
};

int sched_setattr(pid_t pid,const struct sched_attr *attr,unsigned int flags);

int sched_getattr(pid_t pid,struct sched_attr *attr,unsigned int size,unsigned int flags);



static inline void timespec_add_us(struct timespec *t, uint64_t d);

void wait_next_activation(struct periodic_thread * thd);

void start_periodic_timer(struct periodic_thread * thd, uint64_t offs);

int get_id();

#endif 

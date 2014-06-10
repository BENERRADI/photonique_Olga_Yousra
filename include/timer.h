#ifndef TIMER_H
#define	TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <time.h>
#include <stdio.h>

#ifndef timer__             
    //#define timer__(data)   for (long blockTime = NULL; (blockTime == NULL ? (blockTime = clock()) != NULL : false); printf(data " : %.3fs\n", (double) (clock() - blockTime) / CLOCKS_PER_SEC))
#define timer__(data)   for (clock_t blockTime = -1; (blockTime == -1 ? (blockTime = clock()) != -1 : false); printf(data " : %.3fs\n", (double) (clock() - blockTime) / CLOCKS_PER_SEC), fflush(stdout))
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_H */


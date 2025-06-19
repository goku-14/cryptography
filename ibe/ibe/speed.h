#pragma once
/* ------------------------------------------------------------------------- */

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))

#define TIMER_NAME "RDTSC instruction"

#define TIMER_VARS                                                            \
  uint32_t start_lo, start_hi;                                                     \
  uint32_t ticks_lo, ticks_hi

#define TIMER_START()                                                         \
  __asm__ volatile                                                            \
    ("\n        rdtsc"                                                        \
     : "=a" (start_lo), "=d" (start_hi))

#define TIMER_STOP()                                                          \
  __asm__ volatile                                                            \
    ("\n        rdtsc"                                                        \
     "\n        subl %2, %%eax"                                               \
     "\n        sbbl %3, %%edx"                                               \
     : "=&a" (ticks_lo), "=&d" (ticks_hi)                                     \
     : "g" (start_lo), "g" (start_hi))

#define TICKS() ((double)ticks_lo + 4294967296.0 * (double)ticks_hi)
#define USEC(ticks) (ticks / cpu_speed)

#define TRIALS 19
#define TARGET_TIME 250.0
#define SHORT_TARGET_TIME 250.0

/* ------------------------------------------------------------------------- */

#elif defined(_MSC_VER) && defined(_M_IX86)

#define TIMER_NAME "RDTSC instruction"
typedef unsigned int u32;
#define TIMER_VARS                                                            \
  u32 start_lo, start_hi;                                                     \
  u32 ticks_lo, ticks_hi
// luoying RDTSC : Read Time Stamp Counter to EDX:EAX
// vs封装为 unsigned __int64 __rdtsc();

#define TIMER_START()                                                         \
  __asm {                                                                     \
    __asm rdtsc                                                               \
    __asm mov start_lo, eax                                                   \
    __asm mov start_hi, edx                                                   \
  }

#define TIMER_STOP()                                                          \
  __asm {                                                                     \
    __asm rdtsc                                                               \
    __asm sub eax, start_lo                                                   \
    __asm sbb edx, start_hi                                                   \
    __asm mov ticks_lo, eax                                                   \
    __asm mov ticks_hi, edx                                                   \
  } 
//unsigned __int64 __rdtsc(void);
#define TICKS() ((double)ticks_lo + 4294967296.0 * (double)ticks_hi)
#define USEC(ticks) (ticks / cpu_speed)//单位：微妙

#define TRIALS 19
#define TARGET_TIME 250.0
#define SHORT_TARGET_TIME 250.0

/* ------------------------------------------------------------------------- */

#elif defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_X64))
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#define TIMER_NAME "RDTSC instruction"

#define TIMER_VARS unsigned __int64 ticks_start, ticks_end                 

#define TIMER_START() ticks_start = __rdtsc()                                                   

#define TIMER_STOP() ticks_end = __rdtsc()                                                     

//unsigned __int64 __rdtsc(void);
#define TICKS() ((double)ticks_end - (double)ticks_start)
#define USEC(ticks) (ticks / cpu_speed)

#define TRIALS 19
#define TARGET_TIME 250.0
#define SHORT_TARGET_TIME 250.0

/* ------------------------------------------------------------------------- */

#elif defined(__GNUC__) && defined(__hppa__)

#define TIMER_NAME "MFCTL instruction"

#define TIMER_VARS                                                            \
  unsigned long start_ul;                                                     \
  unsigned long ticks_ul

#define TIMER_START()                                                         \
  __asm__ volatile                                                            \
    ("\n        mfctl 16, %0"                                                 \
     : "=r" (start_ul))

#define TIMER_STOP()                                                          \
  do {                                                                        \
    __asm__ volatile                                                          \
      ("\n        mfctl 16, %0"                                               \
       : "=r" (ticks_ul));                                                    \
                                                                              \
    ticks_ul -= start_ul;                                                     \
  } while (0)


#define TICKS() ((double)ticks_ul)
#define USEC(ticks) (ticks / cpu_speed)

#define TRIALS 19
#define TARGET_TIME 250.0
#define SHORT_TARGET_TIME 250.0

/* ------------------------------------------------------------------------- */

#elif defined(__hppa)

#define TIMER_NAME "MFCTL instruction"
#include <machine/inline.h>

#define TIMER_VARS                                                            \
  unsigned long start_ul;                                                     \
  unsigned long ticks_ul

#define TIMER_START()                                                         \
  do {                                                                        \
    register unsigned long r;                                                 \
    _MFCTL(16, r);                                                            \
    start_ul = r;                                                             \
  } while (0)

#define TIMER_STOP()                                                          \
  do {                                                                        \
    register unsigned long r;                                                 \
    _MFCTL(16, r);                                                            \
    ticks_ul = r - start_ul;                                                  \
  } while (0)


#define TICKS() ((double)ticks_ul)
#define USEC(ticks) (ticks / cpu_speed)

#define TRIALS 19
#define TARGET_TIME 250.0
#define SHORT_TARGET_TIME 250.0

/* ------------------------------------------------------------------------- */

#elif defined(__GNUC__) && defined(__sparc__)

#define TIMER_NAME "tick register"

#define TIMER_VARS                                                            \
  unsigned long start_ul;                                                     \
  unsigned long ticks_ul

#define TIMER_START()                                                         \
  __asm__ volatile                                                            \
    ("\n        rd %%tick, %0"                                                \
     : "=r" (start_ul))

#define TIMER_STOP()                                                          \
  do {                                                                        \
    __asm__ volatile                                                          \
      ("\n        rd %%tick, %0"                                              \
       : "=r" (ticks_ul));                                                    \
                                                                              \
    ticks_ul -= start_ul;                                                     \
  } while (0)


#define TICKS() ((double)ticks_ul)
#define USEC(ticks) (ticks / cpu_speed)

#define TRIALS 19
#define TARGET_TIME 250.0
#define SHORT_TARGET_TIME 250.0

/* ------------------------------------------------------------------------- */

#elif defined(__sparc)

#define TIMER_NAME "gethrtime()"
#include <sys/time.h>

#define TIMER_VARS                                                            \
  hrtime_t start_ul;                                                          \
  hrtime_t ticks_ul

#define TIMER_START()                                                         \
  start_ul = gethrtime()

#define TIMER_STOP()                                                          \
  ticks_ul = gethrtime() - start_ul


#define TICKS() ((double)ticks_ul)
#define USEC(ticks) (ticks / 1000.0)

#define TRIALS 19
#define TARGET_TIME 250.0
#define SHORT_TARGET_TIME 250.0

/* ------------------------------------------------------------------------- */

#elif defined(__GNUC__) && defined(__alpha__)

#define TIMER_NAME "RPCC instruction"

#define TIMER_VARS                                                            \
  unsigned int start_ul;                                                      \
  unsigned int ticks_ul

#define TIMER_START()                                                         \
  __asm__ volatile                                                            \
    ("\n        rpcc %0"                                                      \
     : "=r" (start_ul))

#define TIMER_STOP()                                                          \
  do {                                                                        \
    __asm__ volatile                                                          \
      ("\n        rpcc %0"                                                    \
       : "=r" (ticks_ul));                                                    \
                                                                              \
    ticks_ul -= start_ul;                                                     \
  } while (0)


#define TICKS() ((double)ticks_ul)
#define USEC(ticks) (ticks / cpu_speed)

#define TRIALS 19
#define TARGET_TIME 250.0
#define SHORT_TARGET_TIME 250.0

/* ------------------------------------------------------------------------- */

#elif defined(__alpha)

#define TIMER_NAME "RPCC instruction"
#include <c_asm.h>

#define TIMER_VARS                                                            \
  unsigned int start_ul;                                                      \
  unsigned int ticks_ul

#define TIMER_START()                                                         \
  start_ul = asm("rpcc %v0")

#define TIMER_STOP()                                                          \
  ticks_ul = asm("rpcc %v0") - start_ul


#define TICKS() ((double)ticks_ul)
#define USEC(ticks) (ticks / cpu_speed)

#define TRIALS 19
#define TARGET_TIME 250.0
#define SHORT_TARGET_TIME 250.0

/* ------------------------------------------------------------------------- */

#elif defined(__GNUC__) && (defined(__powerpc__) || defined(__ppc__))

#include "ppctbcycles.h"

#define TIMER_NAME "MFTB instruction (assuming "                              \
  QUOTE(PPC_CLOCKS_PER_TICK) " clocks per tick)"

#define TIMER_VARS                                                            \
  unsigned int start_ul;                                                      \
  unsigned int ticks_ul

#define TIMER_START()                                                         \
  __asm__ volatile                                                            \
    ("\n        mftb %0"                                                      \
     : "=r" (start_ul))

#define TIMER_STOP()                                                          \
  do {                                                                        \
    __asm__ volatile                                                          \
      ("\n        mftb %0"                                                    \
       : "=r" (ticks_ul));                                                    \
                                                                              \
    ticks_ul -= start_ul;                                                     \
  } while (0)


#define TICKS() ((double)ticks_ul)
#define USEC(ticks) (PPC_CLOCKS_PER_TICK * ticks / cpu_speed)

#define TRIALS 19
#define TARGET_TIME 250.0
#define SHORT_TARGET_TIME 250.0

/* ------------------------------------------------------------------------- */

#else

#define TIMER_NAME "clock() function"
#include <time.h>

#define TIMER_VARS                                                            \
  clock_t start_ul;                                                           \
  clock_t ticks_ul

#define TIMER_START()                                                         \
  start_ul = clock()

#define TIMER_STOP()                                                          \
  ticks_ul = clock() - start_ul


#define TICKS() ((double)ticks_ul)
#define USEC(ticks) (1000000.0 * (double)ticks / (double)CLOCKS_PER_SEC)

#define TRIALS 1
#define TARGET_TIME 3000000.0
#define SHORT_TARGET_TIME 100000.0

/* ------------------------------------------------------------------------- */

#endif

extern double cpu_speed;
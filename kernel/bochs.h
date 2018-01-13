#ifndef _BOCHS_H_
#define _BOCHS_H_

#define BOCHS_DEBUG

#ifdef BOCHS_DEBUG
# define BOCHS_BREAKPOINT  __asm__ __volatile__("xchg %bx, %bx\n");
#else
# define BOCHS_BREAKPOINT
#endif

#endif

#ifndef _ARCH_H_
#define _ARCH_H_

#ifdef x86
# include <x86/arch.h>
#else
# error "Architecture not defined."
#endif

#endif

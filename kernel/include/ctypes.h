#ifndef _CTYPES_H_
#define _CTYPES_H_

#ifndef NULL
# define NULL  (void*)0
#endif

#ifndef false
# define false 0
#endif

#ifndef FALSE
# define FALSE 0
#endif

#ifndef true
# define true  1
#endif

#ifndef TRUE
# define TRUE  1
#endif

typedef char __attribute__((__may_alias__))             int8_t;
typedef short __attribute__((__may_alias__))            int16_t;
typedef int __attribute__((__may_alias__))              int32_t;
typedef unsigned char __attribute__((__may_alias__))    uint8_t;
typedef unsigned short __attribute__((__may_alias__))   uint16_t;
typedef unsigned int __attribute__((__may_alias__))     uint32_t;

#endif

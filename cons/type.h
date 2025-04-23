#ifndef CONS_TYPE_H
#define CONS_TYPE_H

#include <stdint.h>

#ifndef u64
typedef uint64_t u64;
#endif
#ifndef s64
typedef int64_t  s64;
#endif
typedef uint32_t u32;
#ifndef s32
typedef int32_t  s32;
#endif
#ifndef u16
typedef uint16_t u16;
#endif
#ifndef s16
typedef int16_t  s16;
#endif
#ifndef u8
typedef uint8_t  u8;
#endif
#ifndef s8
typedef int8_t   s8;
#endif

#ifndef bool
typedef u8 bool;
#define true 1
#define false 0
#endif

#endif // CONS_TYPE_H

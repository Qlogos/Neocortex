#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef i8   b8;
typedef i32 b32;

typedef float  f32;
typedef double f64;

typedef size_t    usize;
typedef ptrdiff_t isize;

static_assert(sizeof(i8)  == 1, "i8 must be 1 byte");
static_assert(sizeof(i32) == 4, "i32 must be 4 bytes");
static_assert(sizeof(i64) == 8, "i64 must be 8 bytes");
static_assert(sizeof(f32) == 4, "f32 must be 4 bytes");
static_assert(sizeof(f64) == 8, "f64 must be 8 bytes");

#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

#define MIN(a, b)         (((a) < (b)) ? (a) : (b))
#define MAX(a, b)         (((a) > (b)) ? (a) : (b))
#define CLAMP(x, lo, hi)  (MIN(MAX((x), (lo)), (hi)))

#define ALIGN_UP_POW2(n, p)   (((u64)(n) + ((u64)(p) - 1)) & (~((u64)(p) - 1)))
#define ALIGN_DOWN_POW2(n, p) ((u64)(n) & (~((u64)(p) - 1)))

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

#endif // BASE_H
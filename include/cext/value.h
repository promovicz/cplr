#ifndef CPLR_VALUE_H
#define CPLR_VALUE_H

#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "cext/memory.h"

typedef enum {
  /* no type */
  VT_NONE = 0,
  /* type indicates no value */
  VT_VOID = (1 << 1),
  /* classic types */
  VT_BOOL = (1 << 2),
  VT_CHAR = (1 << 3),
  VT_SSHORT = (1 << 4),
  VT_USHORT = (1 << 5),
  VT_SINT = (1 << 6),
  VT_UINT = (1 << 7),
  VT_SLONG = (1 << 8),
  VT_ULONG = (1 << 9),
  VT_SLONGLONG = (1 << 10),
  VT_ULONGLONG = (1 << 11),
  /* standard types */
  VT_I8 = (1 << 12),
  VT_I16 = (1 << 13),
  VT_I32 = (1 << 14),
  VT_I64 = (1 << 15),
  VT_U8 = (1 << 16),
  VT_U16 = (1 << 17),
  VT_U32 = (1 << 18),
  VT_U64 = (1 << 19),
  /* floating point types */
  VT_FLOAT = (1 << 20),
  VT_DOUBLE = (1 << 21),
  /* offsets and sizes */
  VT_OFF = (1 << 22),
  VT_SSIZE = (1 << 23),
  VT_SIZE = (1 << 24),

  /* pointer flag */
  VTF_POINTER = (1 << 31),
  /* array flag */
  VTF_ARRAY = ((1 << 30) | VTF_POINTER),
  /* zero-terminated array flag */
  VTF_ARRAYZ = ((1 << 29) | VTF_POINTER),
  /* ownership flag */
  VTF_OWNED = (1 << 28),
  /* static flag */
  VTF_STATIC = (1 << 27),

  /* strings are char arrayz */
  VT_STRING = (VT_CHAR | VTF_ARRAYZ),
} vtype_t;

typedef struct value {
  vtype_t t;
  union {
    bool b;
    char c;
    short ss;
    unsigned short us;
    int si;
    unsigned int ui;
    long sl;
    unsigned long ul;
    long long sll;
    unsigned long long ull;

    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;

    float f;
    double d;

    off_t   o;
    ssize_t sz;
    size_t  uz;

    void *p;
    char *s;
  };
} value_t;

static inline void value_clear(value_t *v) {
  if(v->t & VTF_POINTER) {
    if(v->t & VTF_OWNED) {
      ptrfree(&v->p);
    }
  }
  memset(v, 0, sizeof(value_t));
}

static inline void value_set_str(value_t *v, char *string) {
  value_clear(v);
  v->s = strdup(string);
  v->t = VT_STRING | VTF_OWNED;
}

static inline void value_set_str_owned(value_t *v, char *string) {
  value_clear(v);
  v->s = string;
  v->t = VT_STRING | VTF_OWNED;
}

static inline void value_set_str_static(value_t *v, const char *string) {
  value_clear(v);
  v->s = (char *)string;
  v->t = VT_STRING | VTF_STATIC;
}

#endif /* !CPLR_VALUE_H */

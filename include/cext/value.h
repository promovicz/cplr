#ifndef CPLR_VALUE_H
#define CPLR_VALUE_H

#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#ifdef CEXT_HAVE_WCHAR
#include <wchar.h>
#endif

#include "cext/memory.h"

typedef enum {
  /* no type */
  VT_NONE = 0,
  /* type indicates no value */
  VT_VOID = 1,
  /* classic types */
  VT_BOOL = 2,
  VT_CHAR = 3,
  VT_SHORT = 4,
  VT_USHORT = 5,
  VT_INT = 6,
  VT_UINT = 7,
  VT_LONG = 8,
  VT_ULONG = 9,
  VT_LONGLONG = 10,
  VT_ULONGLONG = 11,
  /* standard integers */
  VT_I8 = 12,
  VT_I16 = 13,
  VT_I32 = 14,
  VT_I64 = 15,
  VT_U8 = 16,
  VT_U16 = 17,
  VT_U32 = 18,
  VT_U64 = 19,
  /* floating point types */
  VT_FLOAT = 20,
  VT_DOUBLE = 21,
  /* offsets and sizes */
  VT_OFF = 22,
  VT_SIZE = 23,
  VT_SSIZE = 24,
  /* wide characters */
#ifdef CEXT_HAVE_WCHAR
  VT_WCHAR = 25,
#endif

  VT_MASK = 0xFFFF,

  /* pointer flag */
  VTF_POINTER = (1 << 16),
  /* array flag */
  VTF_ARRAY = ((1 << 17) | VTF_POINTER),
  /* zero-terminated array flag */
  VTF_ARRAYZ = ((1 << 18) | VTF_ARRAY),
  /* object flag */
  VTF_OBJECT = ((1 << 19) | VTF_POINTER),

  VTF_MASK = 0xFF0000,

  /* ownership flag */
  VTM_OWNED = (1 << 24),
  /* static flag */
  VTM_STATIC = (1 << 25),
  /* mask for memory flags */
  VTM_ALL = (VTM_OWNED|VTM_STATIC),

  VTM_MASK = 0xFF000000,

  /* simple void pointers */
  VT_POINTER = (VT_VOID | VTF_POINTER),
  /* strings are char arrayz */
  VT_STRING = (VT_CHAR | VTF_ARRAYZ),
  /* wstrings are wchar arrayz */
#ifdef CEXT_HAVE_WCHAR
  VT_WSTRING = (VT_WCHAR | VTF_ARRAYZ),
#endif

  VTEQ_MASK = (VT_MASK|VTF_MASK),
} vtype_t;

typedef struct value {
  vtype_t type;
  union {
    bool b;
    char c;
    short s;
    unsigned short us;
    int i;
    unsigned int ui;
    long l;
    unsigned long ul;
    long long ll;
    unsigned long long ull;

    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;

    float flt;
    double dbl;

    off_t   off;
    size_t  sz;
    ssize_t ssz;

#ifdef CEXT_HAVE_WCHAR
    wchar_t wc;
    wchar_t *wcs;
#endif

    void *ptr;
    char *str;
  };
} value_t;

ATTR_ARG_NONNULL(1)
extern void value_clear(value_t *vp);

ATTR_ARG_NONNULL(1)
extern void value_assert_type(value_t *vp, vtype_t vt);

static inline vtype_t value_get_type(value_t *vp) {
  return vp->type;
}

#define VALUE_SIMPLE_INLINE_GETTER(_name, _type, _field, _vtype)	\
  ATTR_ARG_NONNULL(1)							\
  inline _type value_get_##_name(value_t *vp, _type v) {		\
    value_assert_type(vp, _vtype);					\
    return vp->_field;							\
  }

VALUE_SIMPLE_INLINE_GETTER(bool, bool, b, VT_BOOL);
VALUE_SIMPLE_INLINE_GETTER(char, char, c, VT_CHAR);
VALUE_SIMPLE_INLINE_GETTER(short, short, s, VT_SHORT);
VALUE_SIMPLE_INLINE_GETTER(ushort, unsigned short, us, VT_USHORT);
VALUE_SIMPLE_INLINE_GETTER(int, int, i, VT_INT);
VALUE_SIMPLE_INLINE_GETTER(uint, unsigned int, ui, VT_UINT);
VALUE_SIMPLE_INLINE_GETTER(long, short, l, VT_LONG);
VALUE_SIMPLE_INLINE_GETTER(ulong, unsigned long, ul, VT_ULONG);
VALUE_SIMPLE_INLINE_GETTER(longlong, long long, ll, VT_LONGLONG);
VALUE_SIMPLE_INLINE_GETTER(ulonglong, unsigned long long, ull, VT_ULONGLONG);
VALUE_SIMPLE_INLINE_GETTER(int8,  int8_t, i8, VT_I8);
VALUE_SIMPLE_INLINE_GETTER(int16, int16_t, i16, VT_I16);
VALUE_SIMPLE_INLINE_GETTER(int32, int32_t, i32, VT_I32);
VALUE_SIMPLE_INLINE_GETTER(int64, int64_t, i64, VT_I64);
VALUE_SIMPLE_INLINE_GETTER(uint8,  uint8_t, u8, VT_U8);
VALUE_SIMPLE_INLINE_GETTER(uint16, uint16_t, u16, VT_U16);
VALUE_SIMPLE_INLINE_GETTER(uint32, uint32_t, u32, VT_U32);
VALUE_SIMPLE_INLINE_GETTER(uint64, uint64_t, u64, VT_U64);
VALUE_SIMPLE_INLINE_GETTER(float, float, flt, VT_FLOAT);
VALUE_SIMPLE_INLINE_GETTER(double, double, dbl, VT_DOUBLE);
VALUE_SIMPLE_INLINE_GETTER(off, off_t, off, VT_OFF);
VALUE_SIMPLE_INLINE_GETTER(size, size_t, sz, VT_SIZE);
VALUE_SIMPLE_INLINE_GETTER(ssize, ssize_t, ssz, VT_SSIZE);

#undef VALUE_SIMPLE_INLINE_GETTER

#define VALUE_SIMPLE_DECLARE_SETTER(_name, _type)	\
  ATTR_ARG_NONNULL(1)					\
  void value_set_##_name(value_t *vp, _type v);
#define VALUE_SIMPLE_DEFINE_SETTER(_name, _type, _field, _vtype)	\
  ATTR_ARG_NONNULL(1)							\
  void value_set_##_name(value_t *vp, _type v) {			\
    value_clear(vp);							\
    vp->type = _vtype;							\
    vp->_field = v;							\
  }

VALUE_SIMPLE_DECLARE_SETTER(bool, bool);
VALUE_SIMPLE_DECLARE_SETTER(char, char);
VALUE_SIMPLE_DECLARE_SETTER(short, short);
VALUE_SIMPLE_DECLARE_SETTER(ushort, unsigned short);
VALUE_SIMPLE_DECLARE_SETTER(int, int);
VALUE_SIMPLE_DECLARE_SETTER(uint, unsigned int);
VALUE_SIMPLE_DECLARE_SETTER(long, short);
VALUE_SIMPLE_DECLARE_SETTER(ulong, unsigned long);
VALUE_SIMPLE_DECLARE_SETTER(longlong, long long);
VALUE_SIMPLE_DECLARE_SETTER(ulonglong, unsigned long long);
VALUE_SIMPLE_DECLARE_SETTER(int8,  int8_t);
VALUE_SIMPLE_DECLARE_SETTER(int16, int16_t);
VALUE_SIMPLE_DECLARE_SETTER(int32, int32_t);
VALUE_SIMPLE_DECLARE_SETTER(int64, int64_t);
VALUE_SIMPLE_DECLARE_SETTER(uint8,  uint8_t);
VALUE_SIMPLE_DECLARE_SETTER(uint16, uint16_t);
VALUE_SIMPLE_DECLARE_SETTER(uint32, uint32_t);
VALUE_SIMPLE_DECLARE_SETTER(uint64, uint64_t);
VALUE_SIMPLE_DECLARE_SETTER(float, float);
VALUE_SIMPLE_DECLARE_SETTER(double, double);
VALUE_SIMPLE_DECLARE_SETTER(off, off_t);
VALUE_SIMPLE_DECLARE_SETTER(size, size_t);
VALUE_SIMPLE_DECLARE_SETTER(ssize, ssize_t);

#undef VALUE_SIMPLE_DECLARE_SETTER


#define VALUE_POINTER_DECLARE_GETTERS(_name, _type)		\
  ATTR_ARG_NONNULL(1)						\
  _type *value_get_##_name(value_t *vp);			\
  ATTR_ARG_NONNULL(1)						\
  _type *value_ref_##_name(value_t *vp);
#define VALUE_POINTER_DEFINE_GETTERS(_name, _type, _field, _vtype, _ref) \
  ATTR_ARG_NONNULL(1)							\
  _type *value_get_##_name(value_t *vp) {				\
    value_assert_type(vp, _vtype);					\
    return vp->_field;							\
  }									\
  ATTR_ARG_NONNULL(1)							\
  _type *value_ref_##_name(value_t *vp) {				\
    value_assert_type(vp, _vtype);					\
    return _ref(vp->_field);						\
  }

VALUE_POINTER_DECLARE_GETTERS(ptr, void);
VALUE_POINTER_DECLARE_GETTERS(str, char);

#undef VALUE_POINTER_DECLARE_GETTERS


#define VALUE_POINTER_DECLARE_SETTERS(_name, _type)		\
  ATTR_ARG_NONNULL(1)						\
  void value_set_##_name(value_t *vp, _type *v);		\
  ATTR_ARG_NONNULL(1)						\
  void value_set_##_name##_owned(value_t *vp, _type *v);	\
  ATTR_ARG_NONNULL(1)						\
  void value_set_##_name##_static(value_t *vp, const _type *v)

#define VALUE_POINTER_DEFINE_SETTERS(_name, _type, _field, _vtype, _clone) \
  ATTR_ARG_NONNULL(1)							\
  void value_set_##_name(value_t *vp, _type *v) {			\
    value_clear(vp);							\
    vp->_field = _clone(v);						\
    vp->type = _vtype | VTM_OWNED;					\
  }									\
  ATTR_ARG_NONNULL(1)							\
  void value_set_##_name##_owned(value_t *vp, _type *v) {		\
    value_clear(vp);							\
    vp->_field = v;							\
    vp->type = _vtype | VTM_OWNED;					\
  }									\
  ATTR_ARG_NONNULL(1)							\
  void value_set_##_name##_static(value_t *vp, const _type *v) {	\
    value_clear(vp);							\
    vp->_field = (_type *)v;						\
    vp->type = _vtype | VTM_STATIC;					\
  }

VALUE_POINTER_DECLARE_SETTERS(ptr, void);
VALUE_POINTER_DECLARE_SETTERS(str, char);

#undef VALUE_POINTER_DECLARE_SETTERS

#endif /* !CPLR_VALUE_H */

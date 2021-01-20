/*
 * cplr - Utility for running C code
 *
 * Copyright (C) 2020-2021 Ingo Albrecht <copyright@promovicz.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "cext/abort.h"

#include "cext/value.h"

ATTR_ARG_NONNULL(1)
void value_clear(value_t *vp) {
  if(vp->type & VTF_POINTER) {
    if(vp->type & VTM_OWNED) {
      ptrfree(&vp->ptr);
    }
  }
  memset(vp, 0, sizeof(value_t));
}

VALUE_SIMPLE_DEFINE_SETTER(bool, bool, b, VT_BOOL);
VALUE_SIMPLE_DEFINE_SETTER(char, char, c, VT_CHAR);
VALUE_SIMPLE_DEFINE_SETTER(short, short, s, VT_SHORT);
VALUE_SIMPLE_DEFINE_SETTER(ushort, unsigned short, us, VT_USHORT);
VALUE_SIMPLE_DEFINE_SETTER(int, int, i, VT_INT);
VALUE_SIMPLE_DEFINE_SETTER(uint, unsigned int, ui, VT_UINT);
VALUE_SIMPLE_DEFINE_SETTER(long, short, l, VT_LONG);
VALUE_SIMPLE_DEFINE_SETTER(ulong, unsigned long, ul, VT_ULONG);
VALUE_SIMPLE_DEFINE_SETTER(longlong, long long, ll, VT_LONGLONG);
VALUE_SIMPLE_DEFINE_SETTER(ulonglong, unsigned long long, ull, VT_ULONGLONG);
VALUE_SIMPLE_DEFINE_SETTER(int8,  int8_t, i8, VT_I8);
VALUE_SIMPLE_DEFINE_SETTER(int16, int16_t, i16, VT_I16);
VALUE_SIMPLE_DEFINE_SETTER(int32, int32_t, i32, VT_I32);
VALUE_SIMPLE_DEFINE_SETTER(int64, int64_t, i64, VT_I64);
VALUE_SIMPLE_DEFINE_SETTER(uint8,  uint8_t, u8, VT_U8);
VALUE_SIMPLE_DEFINE_SETTER(uint16, uint16_t, u16, VT_U16);
VALUE_SIMPLE_DEFINE_SETTER(uint32, uint32_t, u32, VT_U32);
VALUE_SIMPLE_DEFINE_SETTER(uint64, uint64_t, u64, VT_U64);
VALUE_SIMPLE_DEFINE_SETTER(float, float, flt, VT_FLOAT);
VALUE_SIMPLE_DEFINE_SETTER(double, double, dbl, VT_DOUBLE);
VALUE_SIMPLE_DEFINE_SETTER(off, off_t, off, VT_OFF);
VALUE_SIMPLE_DEFINE_SETTER(size, size_t, sz, VT_SIZE);
VALUE_SIMPLE_DEFINE_SETTER(ssize, ssize_t, ssz, VT_SSIZE);

static void *ptr_fault(void *p) {
  xabortf("Cannot clone or reference void pointer\n");
}

VALUE_POINTER_DEFINE_GETTERS(ptr, void, ptr, VT_POINTER, ptr_fault);
VALUE_POINTER_DEFINE_GETTERS(str, char, str, VT_STRING, strdup);

VALUE_POINTER_DEFINE_SETTERS(ptr, void, ptr, VT_POINTER, ptr_fault);
VALUE_POINTER_DEFINE_SETTERS(str, char, str, VT_STRING, strdup);

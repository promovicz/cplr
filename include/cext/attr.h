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

#ifndef CPLR_ATTR_H
#define CPLR_ATTR_H

#define ATTR_FUN_ARTIFICIAL			\
  __attribute__((artificial))
#define ATTR_FUN_COLD				\
  __attribute__((cold))
#define ATTR_FUN_CONST				\
  __attribute__((const))
#define ATTR_FUN_PURE				\
  __attribute__((pure))
#define ATTR_FUN_MALLOC				\
  __attribute__((malloc))
#define ATTR_FUN_NORETURN			\
  __attribute__((noreturn))
#define ATTR_FUN_RETURNS_NONNULL		\
  __attribute__((returns_nonnull))
#define ATTR_FUN_RETURNS_TWICE			\
  __attribute__((returns_twice))
#define ATTR_FUN_FORMAT(arch, fmtindex, argindex)       \
  __attribute__((format(arch, fmtindex, argindex)))
#define ATTR_ARG_ALLOC_SIZE(...)		\
  __attribute__((alloc_size(__VA_ARGS__)))
#define ATTR_ARG_FORMAT(...)			\
  __attribute__((format_arg(__VA_ARGS__)))
#define ATTR_ARG_NONNULL(...)			\
  __attribute__((nonnull(__VA_ARGS__)))

#endif /* !CPLR_ATTR_H */

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

#ifndef CPLR_ABORT_H
#define CPLR_ABORT_H

#include <stdarg.h>

#include "attr.h"

ATTR_FUN_COLD
ATTR_FUN_NORETURN
extern void xabortm(const char *msg);

ATTR_FUN_COLD
ATTR_FUN_NORETURN
extern void xabortf(const char *fmt, ...);

#endif /* !CPLR_ABORT_H */

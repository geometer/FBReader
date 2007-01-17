/*
 * Geometric Calculator -- interactive geometry program
 * Copyright (C) 2003-2007 Nikolay Pultsin <geometer@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __POLYNOM_IO_H__
#define __POLYNOM_IO_H__

#include "Polynom.h"
#include <iostream>

std::ostream& operator << (std::ostream &os, const Number &number);
std::ostream& write (std::ostream &os, const Monom &monom, bool withSign);
std::ostream& operator << (std::ostream &os, const Monom &monom);
std::ostream& operator << (std::ostream &os, const Polynom &polynom);

#endif /* __POLYNOM_IO_H__ */

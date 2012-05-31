/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#include "OleUtil.h"

long OleUtil::getLong(const char *buffer, unsigned int offset) {
	const unsigned char* buf = (const unsigned char*)buffer;
	return   (long)buf[offset]         | ((long)buf[offset+1]<<8L) |
			((long)buf[offset+2]<<16L) | ((long)buf[offset+3]<<24L);
}

unsigned long OleUtil::getULong(const char *buffer, unsigned int offset) {
	const unsigned char* buf = (const unsigned char*)buffer;
	return (unsigned long)buf[offset]         | ((unsigned long)buf[offset+1]<<8L) |
		  ((unsigned long)buf[offset+2]<<16L) | ((unsigned long)buf[offset+3]<<24L);
}

unsigned short OleUtil::getUShort(const char *buffer, unsigned int offset) {
   const unsigned char* buf = (const unsigned char*)buffer;
   return (unsigned short)buf[offset] | ((unsigned short)buf[offset+1]<<8);
}

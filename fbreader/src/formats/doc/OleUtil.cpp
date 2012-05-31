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

int32_t OleUtil::getInt32(const char *buffer, int offset) {
	const unsigned char* buf = (const unsigned char*)buffer;
	return   (int32_t)buf[offset]         | ((int32_t)buf[offset+1]<<8L) |
			((int32_t)buf[offset+2]<<16L) | ((int32_t)buf[offset+3]<<24L);
}

uint32_t OleUtil::getUInt32(const char *buffer, int offset) {
	const unsigned char* buf = (const unsigned char*)buffer;
	return (uint32_t)buf[offset]         | ((uint32_t)buf[offset+1]<<8L) |
		  ((uint32_t)buf[offset+2]<<16L) | ((uint32_t)buf[offset+3]<<24L);
}

uint16_t OleUtil::getUInt16(const char *buffer, int offset) {
   const unsigned char* buf = (const unsigned char*)buffer;
   return (uint16_t)buf[offset] | ((uint16_t)buf[offset+1]<<8);
}


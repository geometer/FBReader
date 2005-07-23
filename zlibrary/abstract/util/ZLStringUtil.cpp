/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "ZLStringUtil.h"

bool ZLStringUtil::stringEndsWith(const std::string &str, const std::string &end) {
	return
		(end.length() <= str.length()) &&
#if __GNUC__ == 2
		(str.compare(end, str.length() - end.length(), end.length()) == 0);
#else
		(str.compare(str.length() - end.length(), end.length(), end) == 0);
#endif
}

void ZLStringUtil::appendNumber(std::string &str, unsigned int n) {
	int len = 1;
	unsigned int base = 10;
	for (; base <= n; base *= 10) {
		len++;
	}
	str.reserve(str.length() + len);
	for (base /= 10; base > 0; base /= 10) {
		str += (char)(n / base + '0');
		n %= base;
	}
}

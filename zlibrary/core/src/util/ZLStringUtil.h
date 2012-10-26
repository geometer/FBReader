/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLSTRINGUTIL_H__
#define __ZLSTRINGUTIL_H__

#include <vector>
#include <string>

class ZLStringUtil {

private:
	ZLStringUtil();

public:
	static bool stringStartsWith(const std::string &str, const std::string &start);
	static bool stringEndsWith(const std::string &str, const std::string &end);

	static bool isDigit(char c);
	static bool isLatinLetter(char c);

	static void appendNumber(std::string &str, unsigned int n);
	static void append(std::string &str, const std::vector<std::string> &buffer);
	static void stripWhiteSpaces(std::string &str);

	static std::vector<std::string> split(const std::string &str, const std::string &delimiter);

	static std::string printf(const std::string &format, const std::string &arg0);

	static std::string doubleToString(double value);
	static double stringToDouble(const std::string &value, double defaultValue);
	static int stringToInteger(const std::string &str, int defaultValue);

	static std::string replaceAll(const std::string &str, const std::string& what, const std::string &to);

	template <typename T, typename F>
	static std::string join(T iterable, F functor, std::string separator);
	template <typename T>
	static std::string join(T iterable, std::string separator);

	template <typename T>
	struct self {
		inline T operator()(const T &t) {
			return t;
		}
	};
};

template <typename T, typename F>
std::string ZLStringUtil::join(T iterable, F functor, std::string separator) {
	std::string result;
	for (typename T::const_iterator it=iterable.begin(); it != iterable.end(); ++it) {
		if (it != iterable.begin()) {
			result += separator;
		}
		result += functor(*it);
	}
	return result;
}

template <typename T>
std::string ZLStringUtil::join(T iterable, std::string separator) {
	return join(iterable, ZLStringUtil::self<typename T::value_type>(), separator);
}

#endif /* __ZLSTRINGUTIL_H__ */

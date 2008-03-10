/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __BOOKDESCRIPTIONUTIL_H__
#define __BOOKDESCRIPTIONUTIL_H__

#include <vector>
#include <string>

class ZLFile;

class BookDescriptionUtil {

public:
	static bool checkInfo(const ZLFile &file);
	static void saveInfo(const ZLFile &file);
	static void listZipEntries(const ZLFile &zipFile, std::vector<std::string> &entries);
	static void resetZipInfo(const ZLFile &zipFile);
	static void removeWhiteSpacesFromTag(std::string &tag);
};

#endif /* __BOOKDESCRIPTIONUTIL_H__ */

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

#ifdef USE_ZAURUS_FONTHACK

#include <dirent.h>

#include "ZLZaurusFontHack.h"

std::set<std::string> ZLZaurusFontHack::families() {
	std::set<std::string> fams;

	DIR *fontPath = opendir("/opt/QtPalmtop/lib/fonts");
	const dirent *file;
	while ((file = readdir(fontPath)) != 0) {
		if ((file->d_type == DT_REG) || (file->d_type == DT_LNK)) {
			std::string name = file->d_name;
			if (name.compare(".qpf", name.length() - 4, 4) == 0) {
				int index = name.find('_');
				if ((int)index != -1) {
					fams.insert(name.substr(0, index));
				}
			}
		}
  }
	closedir(fontPath);
	return fams;
}

#endif /* USE_ZAURUS_FONTHACK */

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

#include <gtk/gtk.h>
#include <gio/gio.h>

#include "ZLGtkFSManager.h"

std::string ZLGtkFSManager::convertFilenameToUtf8(const std::string &name) const {
	if (name.empty()) {
		return name;
	}

	char *gtkString = g_locale_to_utf8(name.data(), name.length(), 0, 0, 0);
	if (gtkString == 0) {
		return "";
	}
	std::string convertedName = gtkString;
	g_free(gtkString);
	return convertedName;
}

std::string ZLGtkFSManager::mimeType(const std::string &path) const {
	std::string type;

	GFile *file = g_file_new_for_path(path.c_str());
	if (file != 0) {
		GFileInfo *info = g_file_query_info(
			file,
			"standard::content-type",
			G_FILE_QUERY_INFO_NONE,
			0, 0
		);
		if (info != 0) {
			const char *pType = g_file_info_get_content_type(info);
			if (pType != 0) {
				type = pType;
			}
			g_object_unref(info);
		}
		g_object_unref(file);
	}

	return type;
}

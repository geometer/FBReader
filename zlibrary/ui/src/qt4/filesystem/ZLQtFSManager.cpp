/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QString>

#include <ZLStringUtil.h>

#include "ZLQtFSManager.h"

std::string ZLQtFSManager::convertFilenameToUtf8(const std::string &name) const {
	if (name.empty()) {
		return name;
	}

	QString qString = QString::fromLocal8Bit(name.c_str());
	return (qString == QString::null) ? "" : (const char*)qString.toUtf8();
}

shared_ptr<ZLMimeType> ZLQtFSManager::mimeType(const std::string &path) const {
	// TODO: implement
	return ZLMimeType::EMPTY;
}

void ZLQtFSManager::normalizeRealPath(std::string &path) const {
	if (ZLStringUtil::stringStartsWith(path, "~~/") || path == "~~") {
		static const std::string replacement =
			std::string((const char*)QCoreApplication::applicationDirPath().toUtf8()) + "/..";
	  path = replacement + path.substr(2);
	} else if (ZLStringUtil::stringStartsWith(path, "~/") || path == "~") {
		static const std::string replacement = (const char*)QDir::homePath().toUtf8();
	  path = replacement + path.substr(1);
	}
}

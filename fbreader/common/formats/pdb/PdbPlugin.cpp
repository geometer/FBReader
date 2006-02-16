/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <abstract/ZLFSManager.h>
#include <abstract/ZLInputStream.h>
#include <abstract/ZLOptions.h>

#include "PdbPlugin.h"
#include "../../description/BookDescriptionUtil.h"

std::string PdbPlugin::fileType(const ZLFile &file) const {
	const std::string &extension = file.extension();
	if ((extension != "pdb") && (extension != "PDB") && (extension != "prc") && (extension != "PRC")) {
		return "";
	}

	std::string fileName = file.path();
	int index = fileName.find(':');
	ZLFile baseFile = (index == -1) ? file : ZLFile(fileName.substr(0, index));
	bool upToDate = BookDescriptionUtil::checkInfo(baseFile);

	ZLStringOption palmTypeOption(file.path(), "PalmType", "");
	std::string palmType = palmTypeOption.value();
	if (palmType.empty() || !upToDate) {
		shared_ptr<ZLInputStream> stream = file.inputStream();
		if (stream.isNull() || !stream->open()) {
			return "";
		}
		stream->seek(60);
		char id[8];
		stream->read(id, 8);
		stream->close();
		palmType.append(id, 8);
		if (!upToDate) {
			BookDescriptionUtil::saveInfo(baseFile);
		}
		palmTypeOption.setValue(palmType);
	}
	return palmType;
}

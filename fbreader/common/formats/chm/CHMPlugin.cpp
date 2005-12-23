/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <iostream>

#include <abstract/ZLStringUtil.h>
#include <abstract/ZLFSManager.h>
#include <abstract/ZLInputStream.h>

#include "CHMPlugin.h"
//#include "CHMDescriptionReader.h"
//#include "CHMBookReader.h"
#include "../txt/PlainTextFormat.h"
#include "../../description/BookDescription.h"

bool CHMPlugin::acceptsFile(const ZLFile &file) const {
	const std::string &extension = file.extension();
	return (extension == "CHM") || (extension == "chm");
}

bool CHMPlugin::readDescription(const std::string &path, BookDescription &description) const {
	return true;
}

bool CHMPlugin::readModel(const BookDescription &description, BookModel &model) const {
	shared_ptr<ZLInputStream> stream = ZLFile(description.fileName()).inputStream();
	if (!stream.isNull() && stream->open()) {
		// header start
		char itsf[5];
		stream->read(itsf, 4);
		itsf[4] = '\0';

		unsigned int version;
		unsigned int totalLength;
		stream->read((char*)&version, 4);
		stream->read((char*)&totalLength, 4);
		std::cerr << "itsf = " << itsf << "\n";
		std::cerr << "version = " << version << "\n";
		std::cerr << "totalLength = " << totalLength << "\n";

		stream->seek(44);
		stream->seek(16);
		// header end

		// additional header data start
		unsigned long long offset;
		stream->read((char*)&offset, 8);
		std::cerr << "offset = " << offset << "\n";
		// additional header data end

		// hmm...
		stream->seek(16);

		// header section 0 start
		stream->seek(8);
		unsigned long long fileSize;
		stream->read((char*)&fileSize, 8);
		std::cerr << "fileSize = " << fileSize << "\n";
		stream->seek(8);
		// header section 0 end
		
		// header section 1 start
		// directory header start
		char itsp[5];
		stream->read(itsp, 4);
		itsp[4] = '\0';
		std::cerr << "itsp = " << itsp << "\n";

		unsigned int length;
		stream->read((char*)&version, 4);
		stream->read((char*)&length, 4);
		std::cerr << "version = " << version << "\n";
		std::cerr << "length = " << length << "\n";
		// ...
		// header section 1 end

		stream->close();
	}
	return true;
}

const std::string &CHMPlugin::iconName() const {
	static const std::string ICON_NAME = "html";
	return ICON_NAME;
}

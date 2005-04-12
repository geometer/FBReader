/*
 * FBReader -- electronic book reader
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

#include <abstract/ZLStringUtil.h>
#include <abstract/ZLInputStream.h>

#include "FB2Plugin.h"
#include "FB2DescriptionReader.h"
#include "FB2BookReader.h"
#include "../../description/BookDescription.h"

bool FB2Plugin::acceptsFile(const std::string &fileName) const {
	return ZLStringUtil::stringEndsWith(fileName, ".fb2");
}

bool FB2Plugin::readDescription(const std::string &fileName, BookDescription &description) const {
	FB2DescriptionReader *reader = new FB2DescriptionReader(description);
	ZLInputStream *stream = ZLInputStream::createStream(fileName);
	bool code = reader->readDescription(*stream);
	delete stream;
	delete reader;
	return code;
}

bool FB2Plugin::readModel(const BookDescription &description, BookModel &model) const {
	// this code fixes incorrect config entry created by fbreader of version <= 0.6.1
	// makes no sense if old fbreader was not used
	if (description.encoding() != "auto") {
		BookInfo(description.fileName()).EncodingOption.setValue("auto");
	}

	FB2BookReader *reader = new FB2BookReader(model);
	ZLInputStream *stream = ZLInputStream::createStream(description.fileName());
	reader->readDocument(*stream);
	delete stream;
	delete reader;
	return true;
}

const std::string &FB2Plugin::iconName() const {
	static const std::string ICON_NAME = "FBReader/fb2";
	return ICON_NAME;
}

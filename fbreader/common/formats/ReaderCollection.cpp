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

#include "ReaderCollection.h"

#include "fb2/FB2DescriptionReader.h"
#include "fb2/FB2BookReader.h"
#include "docbook/DocBookDescriptionReader.h"
#include "docbook/DocBookBookReader.h"

#include "../description/BookDescription.h"
#include "../bookmodel/BookModel.h"

DescriptionReader *ReaderCollection::createDescriptionReader(BookDescription &description) {
	const std::string &fileName = description.fileName();
	if (ZLStringUtil::stringEndsWith(fileName, ".fb2")) {
		return new FB2DescriptionReader(description);
	}
	if (ZLStringUtil::stringEndsWith(fileName, ".xml")) {
		return new DocBookDescriptionReader(description);
	}
	return new FB2DescriptionReader(description);
}

BookReader *ReaderCollection::createBookReader(BookModel &model) {
	const std::string &fileName = model.description().fileName();
	if (ZLStringUtil::stringEndsWith(fileName, ".fb2")) {
		return new FB2BookReader(model);
	}
	if (ZLStringUtil::stringEndsWith(fileName, ".xml")) {
		return new DocBookBookReader(model);
	}
	return new FB2BookReader(model);
}

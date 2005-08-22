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

#ifndef __BOOKMODEL_H__
#define __BOOKMODEL_H__

#include <map>
#include <string>

#include "../model/TextModel.h"
#include "../model/Paragraph.h"
#include "../description/BookDescription.h"

class ZLImage;

class BookModel {

public:
	BookModel(const BookDescriptionPtr description) MODEL_SECTION;
	~BookModel() MODEL_SECTION;

	const std::string &fileName() const MODEL_SECTION;

	const TextModel &bookTextModel() const MODEL_SECTION;
	const TextModel &contentsModel() const MODEL_SECTION;
	const TextModel *footnoteModel(const std::string &id) const MODEL_SECTION;

	const ImageMap &imageMap() const MODEL_SECTION;
	int paragraphNumberById(const std::string &id) const MODEL_SECTION;

	const BookDescriptionPtr description() const MODEL_SECTION;

private:
	const BookDescriptionPtr myDescription;
	PlainTextModel myBookTextModel;
	PlainTextModel myContentsModel;
	ImageMap myImages;
	std::map<std::string,PlainTextModel*> myFootnotes;
	std::map<std::string,int> myInternalHyperlinks;

friend class BookReader;
};

inline const TextModel &BookModel::bookTextModel() const { return myBookTextModel; }
inline const TextModel &BookModel::contentsModel() const { return myContentsModel; }
inline const ImageMap &BookModel::imageMap() const { return myImages; }
inline const BookDescriptionPtr BookModel::description() const { return myDescription; }

#endif /* __BOOKMODEL_H__ */

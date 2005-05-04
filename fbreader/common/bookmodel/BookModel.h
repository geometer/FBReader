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

#ifndef __BOOKMODEL_H__
#define __BOOKMODEL_H__

#include <map>
#include <string>

#include "../model/TextModel.h"
#include "../model/Paragraph.h"

class BookDescription;
class Image;

class BookModel {

public:
	BookModel(const BookDescription *description);
	~BookModel();

	const std::string &fileName() const;

	const TextModel &bookTextModel() const { return myBookTextModel; }
	const TextModel &contentsModel() const { return myContentsModel; }
	const TextModel *footnoteModel(const std::string &id) const;

	const ImageMap &imageMap() const { return myImages; }
	int paragraphNumberById(const std::string &id) const;

	const BookDescription &description() const { return *myDescription; }

private:
	const BookDescription *myDescription;
	PlainTextModel myBookTextModel;
	PlainTextModel myContentsModel;
	ImageMap myImages;
	std::map<const std::string,PlainTextModel*> myFootnotes;
	std::map<const std::string,int> myInternalHyperlinks;

friend class BookReader;
};

#endif /* __BOOKMODEL_H__ */

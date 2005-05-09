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

#include "BookModel.h"
#include "BookReader.h"

#include "../formats/FormatPlugin.h"

#include "../model/Image.h"
#include "../description/BookDescription.h"

BookModel::BookModel(const BookDescription *description) {
	myDescription = description;
#ifdef PALM_TEMPORARY
	if (description->fileName() == "$$TEST") {
		BookReader reader(*this);
		reader.setMainTextModel();
		reader.pushKind(REGULAR);
		reader.beginParagraph();
		//reader.addDataToBuffer("test text");
		reader.endParagraph();
		return;
	}
#endif // PALM_TEMPORARY
	FormatPlugin *plugin = PluginCollection::instance().plugin(description->fileName(), false);
	if (plugin != 0) {
		plugin->readModel(*description, *this);
	}
}

BookModel::~BookModel() {
	delete myDescription;

	for (ImageMap::const_iterator it = myImages.begin(); it != myImages.end(); it++) {
		delete (*it).second;
	}
	for (std::map<std::string,PlainTextModel*>::const_iterator it = myFootnotes.begin(); it != myFootnotes.end(); it++) {
		delete (*it).second;
	}
}

const std::string &BookModel::fileName() const {
	return myDescription->fileName();
}

int BookModel::paragraphNumberById(const std::string &id) const {
	std::map<std::string,int>::const_iterator it = myInternalHyperlinks.find(id);
	return (it != myInternalHyperlinks.end()) ? (*it).second : -1;
}

const TextModel *BookModel::footnoteModel(const std::string &id) const {
	std::map<std::string,PlainTextModel*>::const_iterator it = myFootnotes.find(id);
	return (it != myFootnotes.end()) ? (*it).second : 0;
}

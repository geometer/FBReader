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

#ifndef __ORDESCRIPTIONREADER_H__
#define __ORDESCRIPTIONREADER_H__

#include <ZLXMLReader.h>

#include "../../description/BookDescription.h"

class ORDescriptionReader : public ZLXMLReader {

public:
	ORDescriptionReader(BookDescription &description);
	bool readDescription(const std::string &fileName);

private:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);
	void characterDataHandler(const char *text, int len);

	const std::vector<std::string> &externalDTDs() const;

private:
	WritableBookDescription myDescription;

	bool myReadMetaData;
	enum {
		READ_NONE,
		READ_AUTHOR,
		READ_TITLE
	} myReadState;

	std::string myCurrentAuthor;
};

#endif /* __ORDESCRIPTIONREADER_H__ */

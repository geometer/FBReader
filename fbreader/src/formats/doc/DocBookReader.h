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

#ifndef __DOCBOOKREADER_H__
#define __DOCBOOKREADER_H__

#include "../../bookmodel/BookReader.h"
#include <ZLFile.h>

#include "OleStreamReader.h"

class DocBookReader : public OleStreamReader {

public:
    DocBookReader(BookModel &model);
	~DocBookReader();
	bool readBook();

private:
	bool readDocument(shared_ptr<ZLInputStream> stream, size_t streamSize);
	void parapgraphHandler(std::string paragraph);

	/*
	void startElementHandler(int tag, const char **attributes);
	void endElementHandler(int tag);
	void characterDataHandler(const char *text, size_t len);
	*/

private:
	BookReader myModelReader;
};

inline DocBookReader::~DocBookReader() {}

#endif /* __DOCBOOKREADER_H__ */

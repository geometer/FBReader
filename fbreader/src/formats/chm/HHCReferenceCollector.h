/*
 * Copyright (C) 2008-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __HHCREFERENCECOLLECTOR_H__
#define __HHCREFERENCECOLLECTOR_H__

#include <vector>

#include "../html/HtmlReader.h"

class CHMReferenceCollection;

class HHCReferenceCollector : public HtmlReader {

public:
	HHCReferenceCollector(CHMReferenceCollection &collection);

private:
	void startDocumentHandler();
	void endDocumentHandler();

	bool tagHandler(const HtmlTag &tag);
	bool characterDataHandler(const char*, size_t, bool);

private:
	CHMReferenceCollection &myReferenceCollection;
};

#endif /* __HHCREFERENCECOLLECTOR_H__ */

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

#ifndef __HTMLREADER_H__
#define __HTMLREADER_H__

#include <string>

class ZLInputStream;

class HtmlReader {

protected:
	enum TagCode {
		_UNKNOWN,
		_BODY,
		_P,
		_TITLE,
		_H1,
		_H2,
		_H3,
		_H4,
		_H5,
		_H6,
		_TT,
		_STRONG,
		_B,
		_I,
		_CITE,
	};

	struct HtmlTag {
		TagCode Code;
		bool Start;

		HtmlTag(TagCode code, bool start) : Code(code), Start(start) {}
	};

private:
	static HtmlTag tag(std::string &name);

public:
	void readDocument(ZLInputStream &stream);

protected:
	virtual ~HtmlReader() {}

protected:
	virtual void startDocumentHandler() = 0;
	virtual void endDocumentHandler() = 0;

	virtual bool tagHandler(HtmlTag tag) = 0;
	virtual bool characterDataHandler(const char *text, int len) = 0;
};

#endif /* __HTMLREADER_H__ */

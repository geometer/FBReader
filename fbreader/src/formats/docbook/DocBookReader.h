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

#ifndef __DOCBOOKREADER_H__
#define __DOCBOOKREADER_H__

#include <ZLXMLReader.h>

class DocBookReader : public ZLXMLReader {

public:
	static std::string DTDDirectory;

public:
//protected:
	enum TagCode {
		_ARTICLE,
		_TITLE,
		_ARTICLEINFO,
		_AUTHOR,
		_FIRSTNAME,
		_OTHERNAME,
		_SURNAME,
		_AFFILIATION,
		_ORGNAME,
		_ULINK,
		_ADDRESS,
		_EMAIL,
		_PUBDATE,
		_RELEASEINFO,
		_COPYRIGHT,
		_YEAR,
		_HOLDER,
		_LEGALNOTICE,
		_PARA,
		_REVHISTORY,
		_REVISION,
		_REVNUMBER,
		_DATE,
		_AUTHORINITIALS,
		_REVREMARK,
		_ABSTRACT,
		_SECT1,
		_EMPHASIS,
		_BLOCKQUOTE,
		_CITETITLE,
		_LINK,
		_FOREIGNPHRASE,
		_FIRSTTERM,
		_FILENAME,
		_ITEMIZEDLIST,
		_LISTITEM,
		_PART,
		_PREFACE,
		_CHAPTER,
		_UNKNOWN
	};

protected:
	DocBookReader();

public:
	~DocBookReader();
	const Tag *tags() const;

protected:
	const std::vector<std::string> &externalDTDs() const;
};

inline DocBookReader::DocBookReader() {}
inline DocBookReader::~DocBookReader() {}

#endif /* __DOCBOOKREADER_H__ */

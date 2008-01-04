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

#include <ZLStringUtil.h>
#include <ZLFile.h>
#include <ZLDir.h>

#include "DocBookReader.h"

static const ZLXMLReader::Tag TAGS[] = {
	{"article", DocBookReader::_ARTICLE},
	{"title", DocBookReader::_TITLE},
	{"articleinfo", DocBookReader::_ARTICLEINFO},
	{"author", DocBookReader::_AUTHOR},
	{"firstname", DocBookReader::_FIRSTNAME},
	{"othername", DocBookReader::_OTHERNAME},
	{"surname", DocBookReader::_SURNAME},
	{"affiliation", DocBookReader::_AFFILIATION},
	{"orgname", DocBookReader::_ORGNAME},
	{"ulink", DocBookReader::_ULINK},
	{"address", DocBookReader::_ADDRESS},
	{"email", DocBookReader::_EMAIL},
	{"pubdate", DocBookReader::_PUBDATE},
	{"releaseinfo", DocBookReader::_RELEASEINFO},
	{"copyright", DocBookReader::_COPYRIGHT},
	{"year", DocBookReader::_YEAR},
	{"holder", DocBookReader::_HOLDER},
	{"legalnotice", DocBookReader::_LEGALNOTICE},
	{"para", DocBookReader::_PARA},
	{"revhistory", DocBookReader::_REVHISTORY},
	{"revision", DocBookReader::_REVISION},
	{"revnumber", DocBookReader::_REVNUMBER},
	{"date", DocBookReader::_DATE},
	{"authorinitials", DocBookReader::_AUTHORINITIALS},
	{"revremark", DocBookReader::_REVREMARK},
	{"abstract", DocBookReader::_ABSTRACT},
	{"sect1", DocBookReader::_SECT1},
	{"emphasis", DocBookReader::_EMPHASIS},
	{"blockquote", DocBookReader::_BLOCKQUOTE},
	{"citetitle", DocBookReader::_CITETITLE},
	{"link", DocBookReader::_LINK},
	{"foreignphrase", DocBookReader::_FOREIGNPHRASE},
	{"part", DocBookReader::_PART},
	{"preface", DocBookReader::_PREFACE},
	{"chapter", DocBookReader::_CHAPTER},
	{0, DocBookReader::_UNKNOWN}
};

const ZLXMLReader::Tag *DocBookReader::tags() const {
	return TAGS;
}

const std::vector<std::string> &DocBookReader::externalDTDs() const {
	return EntityFilesCollector::instance().externalDTDs("docbook");
}

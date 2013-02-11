/*
 * Copyright (C) 2010-2013 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLMIMETYPE_H__
#define __ZLMIMETYPE_H__

#include <string>
#include <shared_ptr.h>
#include <map>

class ZLMimeType {

private:

	ZLMimeType(std::string& name, std::map<std::string,std::string>& parameters);

private:

	static std::map<std::string,shared_ptr<ZLMimeType> > ourSimpleTypesMap;

public:
	static shared_ptr<ZLMimeType> get(const char *text);
	static shared_ptr<ZLMimeType> get(std::string text);
	static bool isImage(shared_ptr<ZLMimeType> mimeType);

private:

	const std::map<std::string,std::string> myParameters;
	const std::string myName;

public:

	const std::string& getName() const;
	const std::string& getParameter(const std::string &key) const;

	bool weakEquals(const ZLMimeType& t) const;
	bool operator ==(const ZLMimeType& t) const;
	bool operator !=(const ZLMimeType& t) const;

	static const shared_ptr<ZLMimeType> APPLICATION_XML;
	static const shared_ptr<ZLMimeType> APPLICATION_ZIP;
	static const shared_ptr<ZLMimeType> APPLICATION_EPUB_ZIP;
	static const shared_ptr<ZLMimeType> APPLICATION_FB2_ZIP;
	static const shared_ptr<ZLMimeType> APPLICATION_MOBIPOCKET_EBOOK;
	static const shared_ptr<ZLMimeType> APPLICATION_PDF;
	static const shared_ptr<ZLMimeType> APPLICATION_CHM;
	static const shared_ptr<ZLMimeType> APPLICATION_PALM_DB;
	static const shared_ptr<ZLMimeType> APPLICATION_ATOM_XML;
    static const shared_ptr<ZLMimeType> APPLICATION_RSS_XML;
	static const shared_ptr<ZLMimeType> APPLICATION_LITRES_XML;
	static const shared_ptr<ZLMimeType> APPLICATION_LITRES_XML_BOOKS;
	static const shared_ptr<ZLMimeType> APPLICATION_LITRES_XML_AUTHORS;
	static const shared_ptr<ZLMimeType> APPLICATION_LITRES_XML_GENRES;
	static const shared_ptr<ZLMimeType> APPLICATION_OPENSEARCH_XML;
	static const shared_ptr<ZLMimeType> APPLICATION_OR_XML;

	static const shared_ptr<ZLMimeType> APPLICATION_GZIP;
	static const shared_ptr<ZLMimeType> APPLICATION_BZIP2;
	static const shared_ptr<ZLMimeType> APPLICATION_TAR;
	static const shared_ptr<ZLMimeType> APPLICATION_TAR_GZIP;
	static const shared_ptr<ZLMimeType> APPLICATION_TAR_BZIP2;
	static const shared_ptr<ZLMimeType> APPLICATION_TAR_7Z;

	static const shared_ptr<ZLMimeType> IMAGE_PNG;
	static const shared_ptr<ZLMimeType> IMAGE_JPEG;
	static const shared_ptr<ZLMimeType> IMAGE_SVG;
	static const shared_ptr<ZLMimeType> IMAGE_PALM;
	static const shared_ptr<ZLMimeType> IMAGE_AUTO;

	static const shared_ptr<ZLMimeType> TEXT_HTML;
	static const shared_ptr<ZLMimeType> TEXT_PLAIN;
	static const shared_ptr<ZLMimeType> TEXT_XML;

	static const shared_ptr<ZLMimeType> EMPTY;

};

#endif /* __ZLMIMETYPE_H__ */

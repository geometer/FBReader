/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#include "ZLMimeType.h"

std::map<std::string,shared_ptr<ZLMimeType> > ZLMimeType::ourSimpleTypesMap;

const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_XML = ZLMimeType::get("application/xml");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_ZIP = ZLMimeType::get("application/zip");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_EPUB_ZIP = ZLMimeType::get("application/epub+zip");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_FB2_ZIP = ZLMimeType::get("application/fb2+zip");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_MOBIPOCKET_EBOOK = ZLMimeType::get("application/x-mobipocket-ebook");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_PDF = ZLMimeType::get("application/pdf");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_CHM = ZLMimeType::get("application/x-chm");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_PALM_DB = ZLMimeType::get("application/x-palm-database");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_OR_XML = ZLMimeType::get("application/x-orp-bcd1+xml");

const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_ATOM_XML = ZLMimeType::get("application/atom+xml");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_LITRES_XML = ZLMimeType::get("application/litres+xml");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_OPENSEARCH_XML = ZLMimeType::get("application/opensearchdescription+xml");

const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_GZIP = ZLMimeType::get("application/x-gzip");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_BZIP2 = ZLMimeType::get("application/x-bzip");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_TAR = ZLMimeType::get("application/x-tar");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_TAR_GZIP = ZLMimeType::get("application/x-compressed-tar");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_TAR_BZIP2 = ZLMimeType::get("application/x-bzip-compressed-tar");
const shared_ptr<ZLMimeType> ZLMimeType::APPLICATION_TAR_7Z = ZLMimeType::get("application/x-7z-compressed");

const shared_ptr<ZLMimeType> ZLMimeType::IMAGE_PNG = ZLMimeType::get("image/png");
const shared_ptr<ZLMimeType> ZLMimeType::IMAGE_JPEG = ZLMimeType::get("image/jpeg");
const shared_ptr<ZLMimeType> ZLMimeType::IMAGE_SVG = ZLMimeType::get("image/svg+xml");
const shared_ptr<ZLMimeType> ZLMimeType::IMAGE_PALM = ZLMimeType::get("image/palm");
const shared_ptr<ZLMimeType> ZLMimeType::IMAGE_AUTO = ZLMimeType::get("image/auto");

const shared_ptr<ZLMimeType> ZLMimeType::TEXT_HTML = ZLMimeType::get("text/html");
const shared_ptr<ZLMimeType> ZLMimeType::TEXT_PLAIN = ZLMimeType::get("text/plain");
const shared_ptr<ZLMimeType> ZLMimeType::TEXT_XML = ZLMimeType::get("text/xml");

const shared_ptr<ZLMimeType> ZLMimeType::EMPTY = ZLMimeType::get("");

bool ZLMimeType::isImage(shared_ptr<ZLMimeType> mimeType) {
	return
		mimeType == IMAGE_PNG ||
		mimeType == IMAGE_JPEG ||
		mimeType == IMAGE_SVG;
}

shared_ptr<ZLMimeType> ZLMimeType::get(std::string text) {
	if (text == "") {
		return 0;
	}
	std::string name = text.substr(0, text.find(';'));
	if (name == "") {
		return 0;
	}
	std::map<std::string,std::string> parameters;
	bool fin = (text.find(';') == -1);
	if (!fin) {
		std::string rest = text.substr(text.find(';') + 1);
		while (!fin) {
			if (rest.find('=') == -1) {
				return 0;
			}
			std::string key = rest.substr(0, rest.find('='));
			std::string value = rest.substr(rest.find('=') + 1, rest.find(';'));
			parameters[key] = value;
			fin = (rest.find(';') == -1);
			rest = rest.substr(rest.find(';') + 1);
		}
	}
	if (parameters.empty()) {
		for (std::map<std::string,shared_ptr<ZLMimeType> >::iterator it = ourSimpleTypesMap.begin(); it != ourSimpleTypesMap.end(); ++it) {
			if (it->second->myName == name) {
				return it->second;
			}
		}
		shared_ptr<ZLMimeType> type = new ZLMimeType(name, parameters);
		ourSimpleTypesMap[name] = type;
		return type;
	}
	return new ZLMimeType(name, parameters);
}

bool ZLMimeType::operator ==(const ZLMimeType& t) const {
	return myName == t.myName;
}

ZLMimeType::ZLMimeType(std::string& name, std::map<std::string,std::string>& parameters) : myParameters(parameters), myName(name) {
}

const std::string& ZLMimeType::getName() const {
	return myName;
}

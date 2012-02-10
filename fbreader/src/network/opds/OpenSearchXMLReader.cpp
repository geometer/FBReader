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

#include "OpenSearchXMLReader.h"
#include <ZLMimeType.h>

static const std::string TAG_URL = "Url";

std::string OpenSearchXMLReader::convertOpenSearchURL(const std::string& raws) { //TODO
	size_t pos = raws.find('{');
	return raws.substr(0, pos) + "%s";
}

void OpenSearchXMLReader::startElementHandler(const char *tag, const char **attributes) {
	if (TAG_URL == tag) {
		const char *type = attributeValue(attributes, "type");
		if (*ZLMimeType::get(std::string(type)) == *ZLMimeType::APPLICATION_ATOM_XML) {
			const char *templ = attributeValue(attributes, "template");
			if (templ != 0) {
				myTemplateURL = convertOpenSearchURL(templ);
			}
		}
	}
}

void OpenSearchXMLReader::endElementHandler(const char *tag) {
    (void)tag;
}

void OpenSearchXMLReader::characterDataHandler(const char *text, size_t len) {
    (void)text;
    (void)len;
}

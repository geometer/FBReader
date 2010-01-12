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

#include <ZLUnicodeUtil.h>

#include "HHCReferenceCollector.h"
#include "CHMReferenceCollection.h"

HHCReferenceCollector::HHCReferenceCollector(CHMReferenceCollection &collection) : HtmlReader("US-ASCII"), myReferenceCollection(collection) {
}

void HHCReferenceCollector::startDocumentHandler() {
}

void HHCReferenceCollector::endDocumentHandler() {
}

static const std::string PARAM = "PARAM";
static const std::string NAME = "NAME";
static const std::string VALUE = "VALUE";
static const std::string NAME_VALUE = "Name";
static const std::string LOCAL_VALUE = "Local";

bool HHCReferenceCollector::tagHandler(const HtmlTag &tag) {
	if (tag.Start) {
		if (tag.Name == PARAM) {
			std::string name;
			std::string value;
			for (std::vector<HtmlAttribute>::const_iterator it = tag.Attributes.begin(); it != tag.Attributes.end(); ++it) {
				if (it->Name == NAME) {
					name = it->Value;
				} else if (it->Name == VALUE) {
					value = it->Value;
				}
			}
			if (name == LOCAL_VALUE) {
				myReferenceCollection.addReference(value, true);
			}
		}
	}
	return true;
}

bool HHCReferenceCollector::characterDataHandler(const char*, size_t, bool) {
	return true;
}

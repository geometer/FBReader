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

#include "XMLConfigReader.h"
#include "XMLConfig.h"
#include "XMLConfigDelta.h"
#include "AsciiEncoder.h"

XMLConfigReader::XMLConfigReader(XMLConfig &config, const std::string &category) : myCategory(category), myConfig(config), myGroup(0) {
}

XMLConfigReader::~XMLConfigReader() {
}

void XMLConfigReader::startElementHandler(const char *tag, const char **attributes) {
	static const std::string GROUP = "group";
	static const std::string OPTION = "option";
	static const char *NAME = "name";
	static const char *VALUE = "value";
	static const char *CATEGORY = "category";

	const char *name = attributeValue(attributes, NAME);
	if (name == 0) {
		return;
	}
	if (GROUP == tag) {
		myGroupName = AsciiEncoder::decode(name);
		myGroup = myConfig.getGroup(myGroupName, true);
	} else if ((myGroup != 0) && (OPTION == tag)) {
		const char *value = attributeValue(attributes, VALUE);
		const char *category = attributeValue(attributes, CATEGORY);
		const std::string decodedName = AsciiEncoder::decode(name);
		if (value != 0) {
			const std::string decodedValue = AsciiEncoder::decode(value);
			myGroup->setValue(
				decodedName,
				decodedValue,
				(category == 0) ? myCategory : category
			);
			if (myConfig.myDelta != 0) {
				myConfig.myDelta->setValue(
					myGroupName,
					decodedName,
					decodedValue,
					(category == 0) ? myCategory : category
				);
			}
		} else {
			myGroup->unsetValue(decodedName);
			if (myConfig.myDelta != 0) {
				myConfig.myDelta->unsetValue(myGroupName, decodedName);
			}
		}
	}
}

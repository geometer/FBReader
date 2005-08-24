/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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
#include "AsciiEncoder.h"

enum TagCode {
	_CONFIG,
	_GROUP,
	_OPTION,
	_UNKNOWN
};

static const ZLXMLReader::Tag TAGS[] = {
	{ "config", _CONFIG },
	{ "group", _GROUP },
	{ "option", _OPTION },
	{ 0, _UNKNOWN }
};

const ZLXMLReader::Tag *XMLConfigReader::tags() const {
	return TAGS;
}

void XMLConfigReader::startElementHandler(int tag, const char **attributes) {
	switch (tag) {
		case _CONFIG:
			break;
		case _GROUP:
		{
			bool correct = true;
			for (int i = 0; i < 2; i++) {
				if (attributes[i] == 0) {
					correct = false;
					break;
				}
			}
			if (correct && (strcmp(attributes[0], "name") == 0)) {
				myGroup = new XMLConfigGroup();
				myConfig.myGroups[AsciiEncoder::decode(attributes[1])] = myGroup; 
			}
			break;
		}
		case _OPTION:
			if (myGroup != 0) {
				bool correct = true;
				for (int i = 0; i < 4; i++) {
					if (attributes[i] == 0) {
						correct = false;
						break;
					}
				}
				if (correct && (strcmp(attributes[0], "name") == 0) && (strcmp(attributes[2], "value") == 0)) {
   				myGroup->setValue(AsciiEncoder::decode(attributes[1]), AsciiEncoder::decode(attributes[3])); 
				}
			}
			break;
	}
}

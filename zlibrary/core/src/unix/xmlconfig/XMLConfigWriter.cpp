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

#include "XMLConfigWriter.h"
#include "XMLConfig.h"
#include "AsciiEncoder.h"

const std::string CONFIG = "config";
const std::string GROUP = "group";
const std::string NAME = "name";
const std::string OPTION = "option";
const std::string VALUE = "value";

void XMLConfigWriter::write() {
	addTag(CONFIG, false);
	for (std::map<std::string,XMLConfigGroup*>::const_iterator it = myConfig.myGroups.begin(); it != myConfig.myGroups.end(); ++it) {
		const std::map<std::string,XMLConfigValue> &values = it->second->myValues;
		if (!values.empty()) {
			bool emptyFlag = true;
			for (std::map<std::string,XMLConfigValue>::const_iterator jt = values.begin(); jt != values.end(); ++jt) {
				if (jt->second.Category == myCategory) {
					if (emptyFlag) { 
						addTag(GROUP, false);
						addAttribute(NAME, AsciiEncoder::encode(it->first));
						emptyFlag = false;
					}
					addTag(OPTION, true);
					addAttribute(NAME, AsciiEncoder::encode(jt->first));
					addAttribute(VALUE, AsciiEncoder::encode(jt->second.Value));
				}
			}
			if (!emptyFlag) {
				closeTag();
			}
		}
	}
	closeTag();
}

/*
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

#include "XMLConfigWriter.h"
#include "XMLConfig.h"
#include "AsciiEncoder.h"

void XMLConfigWriter::write() {
	addTag("config", false);
	for (std::map<std::string,XMLConfigGroup*>::const_iterator it = myConfig.myGroups.begin(); it != myConfig.myGroups.end(); it++) {
		const std::map<std::string,std::string> &values = it->second->values();
		if (!values.empty()) {
			addTag("group", false);
			addAttribute("name", AsciiEncoder::encode(it->first));
			for (std::map<std::string,std::string>::const_iterator jt = values.begin(); jt != values.end(); jt++) {
				addTag("option", true);
				addAttribute("name", AsciiEncoder::encode(jt->first));
				addAttribute("value", AsciiEncoder::encode(jt->second));
			}
			closeTag();
		}
	}
	closeTag();
}

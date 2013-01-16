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

#include <ZLNetworkUtil.h>

#include "URLRewritingRule.h"

URLRewritingRule::URLRewritingRule(const std::map<std::string,std::string> &map) : myType(UNKNOWN), myApply(ALWAYS) {
	for (std::map<std::string, std::string>::const_iterator it = map.begin(); it != map.end(); ++it) {
		std::string key = (*it).first;
		std::string value = (*it).second;

		if (key == "type") {
			if (value == "addUrlParameter") {
				myType = ADD_URL_PARAMETER;
			} else if (value == "rewrite") {
				myType = REWRITE;
			}
		} else if (key == "apply") {
			if (value == "internal") {
				myApply = INTERNAL;
			} else if (value == "external") {
				myApply = EXTERNAL;
			}
		} else {
			myParameters.insert(std::make_pair(key, value));
		}

	}
}

std::string URLRewritingRule::apply(const std::string &url) const {
	std::string appliedUrl = url;
	switch (myType) {
		case ADD_URL_PARAMETER:
			{
				std::string name, value;
				std::map<std::string, std::string>::const_iterator it;
				it = myParameters.find("name");
				if (it != myParameters.end()) {
					name = (*it).second;
				}
				it = myParameters.find("value");
				if (it != myParameters.end()) {
					value = (*it).second;
				}
				if (name.empty() || value.empty()) {
					break;
				}
				ZLNetworkUtil::appendParameter(appliedUrl, name, value);
			}
		case REWRITE: //TODO implement (regular expressions should be used here)
		default:
			break;
	}
	return appliedUrl;
}

URLRewritingRule::RuleApply URLRewritingRule::whereToApply() const {
	return myApply;
}

/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <iconv.h>

#include <ZLFile.h>
#include <ZLApplication.h>
#include <ZLUnicodeUtil.h>

#include "../xml/ZLXMLReader.h"

#include "ZLEncodingConverter.h"

std::vector<ZLEncodingConverterInfoPtr> ZLEncodingCollection::ourInfos;
std::map<std::string,ZLEncodingConverterInfoPtr> ZLEncodingCollection::ourInfosByName;

class EncodingCollectionReader : public ZLXMLReader {

public:
	void startElementHandler(const char *tag, const char **attributes);
	void endElementHandler(const char *tag);

private:
	ZLEncodingConverterInfoPtr myCurrentInfo;
	std::vector<std::string> myNames;
};

static const std::string ENCODING = "encoding";
static const std::string NAME = "name";
static const std::string ALIAS = "alias";
static const std::string CODE = "code";
static const std::string NUMBER = "number";

void EncodingCollectionReader::startElementHandler(const char *tag, const char **attributes) {
	if (ENCODING == tag) {
		const char *name = attributeValue(attributes, NAME.c_str());
		if (name != 0) {
			const std::string sName = name;
			myCurrentInfo = new ZLEncodingConverterInfo(sName);
			myNames.push_back(sName);
		}
	} else if (myCurrentInfo != 0) {
		if (CODE == tag) {
			const char *name = attributeValue(attributes, NUMBER.c_str());
			if (name != 0) {
				myNames.push_back(name);
			}
		} else if (ALIAS == tag) {
			const char *name = attributeValue(attributes, NAME.c_str());
			if (name != 0) {
				const std::string sName = name;
				myCurrentInfo->addAlias(sName);
				myNames.push_back(sName);
			}
		}
	}
}

void EncodingCollectionReader::endElementHandler(const char *tag) {
	if (!myCurrentInfo.isNull() && (ENCODING == tag)) {
		if (myCurrentInfo->canCreateConverter()) {
			ZLEncodingCollection::ourInfos.push_back(myCurrentInfo);
			for (std::vector<std::string>::const_iterator it = myNames.begin(); it != myNames.end(); ++it) {
				ZLEncodingCollection::ourInfosByName[ZLUnicodeUtil::toLower(*it)] = myCurrentInfo;
			}
		}
		myCurrentInfo = 0;
		myNames.clear();
	}
}

std::vector<ZLEncodingConverterInfoPtr> &ZLEncodingCollection::infos() {
	if (ourInfos.empty()) {
		const std::string prefix = encodingDescriptionPath() + ZLApplication::FileNameDelimiter;
		EncodingCollectionReader().readDocument(prefix + "Encodings.xml");
		if (ourInfosByName["utf-8"].isNull()) {
			ZLEncodingConverterInfoPtr info = new ZLEncodingConverterInfo("UTF-8");
			ourInfos.push_back(info);
			ourInfosByName["utf-8"] = info;
		}
		if (ourInfosByName["us-ascii"].isNull()) {
			ZLEncodingConverterInfoPtr info = new ZLEncodingConverterInfo("US-ASCII");
			ourInfos.push_back(info);
			ourInfosByName["us-ascii"] = info;
		}
	}
	return ourInfos;
}

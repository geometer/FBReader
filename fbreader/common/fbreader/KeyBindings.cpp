/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <abstract/ZLXMLReader.h>
#include <abstract/ZLStringUtil.h>
#include <abstract/ZLInputStream.h>
#include <abstract/ZLFSManager.h>

#include "FBReader.h"
#include "../Files.h"

static const std::string BINDINGS_GROUP = "Keys";
static const std::string BINDINGS_NUMBER = "Number";
static const std::string BINDED_KEY = "Key";
static const std::string BINDED_ACTION = "Action";

class KeyBindingsReader : public ZLXMLReader {

public:
	KeyBindingsReader(std::map<std::string,ActionCode> &keymap) : myKeymap(keymap) {}

	void startElementHandler(const char *tag, const char **attributes);

private:
	std::map<std::string,ActionCode> &myKeymap;
};

void KeyBindingsReader::startElementHandler(const char *tag, const char **attributes) {
	static const std::string BINDING = "binding";

	if (BINDING == tag) {
		const char *key = attributeValue(attributes, "key");
		const char *action = attributeValue(attributes, "action");
		if ((key != 0) && (action != 0)) {
			myKeymap[key] = (ActionCode)atoi(action);
		}
	}
}

static const std::string KeymapFile = "keymap.xml";

KeyBindings::KeyBindings() {
	shared_ptr<ZLInputStream> stream = ZLFile(Files::DefaultFilesPathPrefix() + KeymapFile).inputStream();
	if (!stream.isNull() && stream->open()) {
		std::map<std::string,ActionCode> keymap;
		KeyBindingsReader(keymap).readDocument(stream);
		stream->close();
		for (std::map<std::string,ActionCode>::const_iterator it = keymap.begin(); it != keymap.end(); it++) {
			bindKey(it->first, it->second);
		}
	}

	int size = ZLIntegerRangeOption(BINDINGS_GROUP, BINDINGS_NUMBER, 0, 256, 0).value();
	for (int i = 0; i < size; i++) {
		std::string key = BINDED_KEY;
		ZLStringUtil::appendNumber(key, i);
		std::string keyValue = ZLStringOption(BINDINGS_GROUP, key, "").value();
		if (!keyValue.empty()) {
			std::string action = BINDED_ACTION;
			ZLStringUtil::appendNumber(action, i);
			int actionValue = ZLIntegerOption(BINDINGS_GROUP, action, -1).value();
			if (actionValue != -1) {
				bindKey(keyValue, (ActionCode)actionValue);
			}
		}
	}
}

KeyBindings::~KeyBindings() {
	std::map<std::string,ActionCode> keymap;
	shared_ptr<ZLInputStream> stream = ZLFile(Files::DefaultFilesPathPrefix() + KeymapFile).inputStream();
	if (!stream.isNull() && stream->open()) {
		KeyBindingsReader(keymap).readDocument(stream);
		stream->close();
	}

	ZLOption::clearGroup(BINDINGS_GROUP);
	int counter = 0;
	for (std::map<std::string,ActionCode>::const_iterator it = myBindingsMap.begin(); it != myBindingsMap.end(); it++) {
		std::map<std::string,ActionCode>::const_iterator original = keymap.find(it->first);
		ActionCode defaultAction = (original == keymap.end()) ? NO_ACTION : original->second;
		if (defaultAction != it->second) {
			std::string key = BINDED_KEY;
			ZLStringUtil::appendNumber(key, counter);
			std::string action = BINDED_ACTION;
			ZLStringUtil::appendNumber(action, counter);
			ZLStringOption(BINDINGS_GROUP, key, "").setValue(it->first);
			ZLIntegerOption(BINDINGS_GROUP, action, -1).setValue(it->second);
			counter++;
		}
	}
	ZLIntegerRangeOption(BINDINGS_GROUP, BINDINGS_NUMBER, 0, 256, 0).setValue(counter);
}

void KeyBindings::bindKey(const std::string &key, ActionCode code) {
	myBindingsMap[key] = code;
}

ActionCode KeyBindings::getBinding(const std::string &key) {
	std::map<std::string,ActionCode>::const_iterator it = myBindingsMap.find(key);
	return (it != myBindingsMap.end()) ? it->second : NO_ACTION;
}

/*
 * FBReader -- electronic book reader
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

#include <ZLXMLReader.h>
#include <ZLApplication.h>

#include "Dictionary.h"

class DictionaryCollectionBuilder : public ZLXMLReader {

public:
	DictionaryCollectionBuilder(DictionaryCollection &collection);

private:
	void startElementHandler(const char *tag, const char **attributes);	
	void endElementHandler(const char *tag);	

private:
	DictionaryCollection &myCollection;
	shared_ptr<Dictionary> myCurrentDictionary;
};

static const std::string DICTIONARY = "dictionary";
static const std::string SHOW_WORD_COMMAND = "showWord";

DictionaryCollectionBuilder::DictionaryCollectionBuilder(DictionaryCollection &collection) :
	myCollection(collection),
	myCurrentDictionary(0) {
}

void DictionaryCollectionBuilder::startElementHandler(const char *tag, const char **attributes) {
	if (DICTIONARY == tag) {
		const char *name = attributeValue(attributes, "name");
		const char *protocol = attributeValue(attributes, "protocol");
		const char *testFile = attributeValue(attributes, "testFile");
		if ((name != 0) && (protocol != 0)) {
			shared_ptr<ZLCommunicator> communicator =
				ZLCommunicationManager::instance().createCommunicator(protocol, (testFile != 0) ? testFile : "");
			if (!communicator.isNull()) {
				std::string sName = name;
				myCurrentDictionary = new Dictionary(communicator);
				myCollection.myNames.push_back(sName);
				myCollection.myDictionaries[sName] = myCurrentDictionary;
			}
		}
	} else if (!myCurrentDictionary.isNull() && (SHOW_WORD_COMMAND == tag)) {
		for (const char **it = attributes; (*it != 0) && (*(it + 1) != 0); it += 2) {
			myCurrentDictionary->myShowWordData[*it] = *(it + 1);
		}
	}
}

void DictionaryCollectionBuilder::endElementHandler(const char *tag) {
	if (DICTIONARY == tag) {
		myCurrentDictionary = 0;
	}
}

static const std::string DICTIONARY_GROUP = "Dictionary";

DictionaryCollection::DictionaryCollection() :
	EnableIntegrationOption(ZLOption::CONFIG_CATEGORY, DICTIONARY_GROUP, "Enabled", true),
	CurrentNameOption(ZLOption::CONFIG_CATEGORY, DICTIONARY_GROUP, "Name", "") {
	DictionaryCollectionBuilder builder(*this);
	builder.readDocument(ZLApplication::ApplicationDirectory() + ZLApplication::FileNameDelimiter + "dictionary-integration.xml");
	if (!myNames.empty() && (myDictionaries.find(CurrentNameOption.value()) == myDictionaries.end())) {
		CurrentNameOption.setValue(myNames[0]);
	}
}

const std::vector<std::string> &DictionaryCollection::names() const {
	return myNames;
}

shared_ptr<Dictionary> DictionaryCollection::currentDictionary() const {
	if (!EnableIntegrationOption.value()) {
		return 0;
	}
	std::map<std::string,shared_ptr<Dictionary> >::const_iterator it = myDictionaries.find(CurrentNameOption.value());
	return (it != myDictionaries.end()) ? it->second : 0;
}

Dictionary::Dictionary(shared_ptr<ZLCommunicator> communicator) : myCommunicator(communicator) {
}

void Dictionary::openInDictionary(const std::string &word) const {
	if (!myCommunicator.isNull()) {
		shared_ptr<ZLMessageSender> sender = myCommunicator->createSender(myShowWordData);
		if (!sender.isNull()) {
			sender->sendStringMessage(word);
		}
	}
}

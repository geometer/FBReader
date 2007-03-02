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

#include <algorithm>

#include <ZLXMLReader.h>
#include <ZLApplication.h>

#include "ProgramCollection.h"

class ProgramCollectionBuilder : public ZLXMLReader {

public:
	ProgramCollectionBuilder(ProgramCollectionMap &collectionMap);

private:
	void startElementHandler(const char *tag, const char **attributes);	
	void endElementHandler(const char *tag);	

private:
	ProgramCollectionMap &myCollectionMap;
	shared_ptr<ProgramCollection> myCurrentCollection;
	shared_ptr<Program> myCurrentProgram;
};

static const std::string SECTION = "section";
static const std::string PROGRAM = "program";
static const std::string ACTION = "action";

ProgramCollectionBuilder::ProgramCollectionBuilder(ProgramCollectionMap &collectionMap) : myCollectionMap(collectionMap) {
}

void ProgramCollectionBuilder::startElementHandler(const char *tag, const char **attributes) {
	if (SECTION == tag) {
		const char *name = attributeValue(attributes, "name");
		if (name != 0) {
			myCurrentCollection = myCollectionMap.myMap[name];
			if (myCurrentCollection.isNull()) {
				myCurrentCollection = new ProgramCollection(name);
				myCollectionMap.myMap[name] = myCurrentCollection;
			}
		}
	} else if (!myCurrentCollection.isNull() && (PROGRAM == tag)) {
		const char *name = attributeValue(attributes, "name");
		const char *protocol = attributeValue(attributes, "protocol");
		const char *testFile = attributeValue(attributes, "testFile");
		if ((name != 0) && (protocol != 0)) {
			shared_ptr<ZLCommunicator> communicator =
				ZLCommunicationManager::instance().createCommunicator(protocol, (testFile != 0) ? testFile : "");
			if (!communicator.isNull()) {
				std::string sName = name;
				myCurrentProgram = new Program(communicator);
				myCurrentCollection->myNames.push_back(sName);
				myCurrentCollection->myDictionaries[sName] = myCurrentProgram;
			}
		}
	} else if (!myCurrentProgram.isNull() && (ACTION == tag)) {
		const char *name = attributeValue(attributes, "name");
		if (name != 0) {
			ZLCommunicationManager::Data &data = myCurrentProgram->myCommandData[name];
			for (const char **it = attributes; (*it != 0) && (*(it + 1) != 0); it += 2) {
				if (*it != "name") {
					data[*it] = *(it + 1);
				}
			}
		}
	}
}

void ProgramCollectionBuilder::endElementHandler(const char *tag) {
	if (SECTION == tag) {
		if (!myCurrentCollection.isNull()) {
			const std::vector<std::string> &names = myCurrentCollection->names();
			ZLStringOption &nameOption = myCurrentCollection->CurrentNameOption;
			if (!names.empty() && (std::find(names.begin(), names.end(), nameOption.value()) == names.end())) {
				nameOption.setValue(names.front());
			}
		}
		myCurrentCollection = 0;
		myCurrentProgram = 0;
	} else if (PROGRAM == tag) {
		myCurrentProgram = 0;
	}
}

ProgramCollectionMap::ProgramCollectionMap() {
	ProgramCollectionBuilder builder(*this);
	builder.readDocument(ZLApplication::ApplicationDirectory() + ZLApplication::FileNameDelimiter + "external.xml");
}

shared_ptr<ProgramCollection> ProgramCollectionMap::collection(const std::string &name) const {
	std::map<std::string,shared_ptr<ProgramCollection> >::const_iterator it = myMap.find(name);
	return (it != myMap.end()) ? it->second : 0;
}

ProgramCollection::ProgramCollection(const std::string &name) :
	EnableCollectionOption(ZLOption::CONFIG_CATEGORY, name, "Enabled", true),
	CurrentNameOption(ZLOption::CONFIG_CATEGORY, name, "Name", "") {
}

const std::vector<std::string> &ProgramCollection::names() const {
	return myNames;
}

shared_ptr<Program> ProgramCollection::currentProgram() const {
	if (!EnableCollectionOption.value()) {
		return 0;
	}
	std::map<std::string,shared_ptr<Program> >::const_iterator it = myDictionaries.find(CurrentNameOption.value());
	return (it != myDictionaries.end()) ? it->second : 0;
}

Program::Program(shared_ptr<ZLCommunicator> communicator) : myCommunicator(communicator) {
}

void Program::run(const std::string &command, const std::string &parameter) const {
	if (!myCommunicator.isNull()) {
		std::map<std::string,ZLCommunicationManager::Data>::const_iterator it = myCommandData.find(command);
		if (it != myCommandData.end()) {
			shared_ptr<ZLMessageSender> sender = myCommunicator->createSender(it->second);
			if (!sender.isNull()) {
				sender->sendStringMessage(parameter);
			}
		}
	}
}

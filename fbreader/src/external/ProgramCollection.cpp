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

#include <algorithm>

#include <ZLXMLReader.h>
#include <ZLibrary.h>
#include <ZLResource.h>

#include "ProgramCollection.h"
#include "../options/FBOptions.h"

class ProgramCollectionBuilder : public ZLXMLReader {

public:
	ProgramCollectionBuilder(ProgramCollectionMap &collectionMap);
	~ProgramCollectionBuilder();

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
static const std::string OPTION = "option";

ProgramCollectionBuilder::ProgramCollectionBuilder(ProgramCollectionMap &collectionMap) : myCollectionMap(collectionMap) {
}

ProgramCollectionBuilder::~ProgramCollectionBuilder() {
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
			shared_ptr<ZLMessageOutputChannel> channel =
				ZLCommunicationManager::instance().createMessageOutputChannel(protocol, (testFile != 0) ? testFile : "");
			if (!channel.isNull()) {
				std::string sName = name;
				if (!sName.empty()) {
					if (sName[0] == '%') {
						sName = ZLResource::resource("external")[sName.substr(1)].value();
					}
					myCurrentProgram = new Program(sName, channel);
					myCurrentCollection->myNames.push_back(sName);
					myCurrentCollection->myPrograms[sName] = myCurrentProgram;
				}
			}
		}
	} else if (!myCurrentProgram.isNull() && (ACTION == tag)) {
		const char *name = attributeValue(attributes, "name");
		if (name != 0) {
			static const std::string NAME = "name";
			ZLCommunicationManager::Data &data = myCurrentProgram->myCommandData[name];
			for (const char **it = attributes; (*it != 0) && (*(it + 1) != 0); it += 2) {
				if (NAME != *it) {
					data[*it] = *(it + 1);
				}
			}
		}
	} else if (!myCurrentProgram.isNull() && (OPTION == tag)) {
		const char *name = attributeValue(attributes, "name");
		if (name != 0) {
			const char *defaultValue = attributeValue(attributes, "defaultValue");
			const std::string sName = name;
			const std::string sDefaultValue = (defaultValue != 0) ? defaultValue : std::string();
			myCurrentProgram->myOptions.push_back(Program::OptionDescription(sName, sDefaultValue));
			myCurrentProgram->myDefaultValues[sName] = sDefaultValue;
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
	builder.readDocument(ZLibrary::DefaultFilesPathPrefix() + "external.xml");
}

shared_ptr<ProgramCollection> ProgramCollectionMap::collection(const std::string &name) const {
	std::map<std::string,shared_ptr<ProgramCollection> >::const_iterator it = myMap.find(name);
	return (it != myMap.end()) ? it->second : 0;
}

ProgramCollection::ProgramCollection(const std::string &name) :
	EnableCollectionOption(ZLCategoryKey::CONFIG, name, "Enabled", true),
	CurrentNameOption(ZLCategoryKey::CONFIG, name, "Name", "") {
}

const std::vector<std::string> &ProgramCollection::names() const {
	return myNames;
}

shared_ptr<Program> ProgramCollection::program(const std::string &name) const {
	std::map<std::string,shared_ptr<Program> >::const_iterator it = myPrograms.find(name);
	return (it != myPrograms.end()) ? it->second : 0;
}

shared_ptr<Program> ProgramCollection::currentProgram() const {
	if (!EnableCollectionOption.value()) {
		return 0;
	}
	return program(CurrentNameOption.value());
}

Program::Program(const std::string &name, shared_ptr<ZLMessageOutputChannel> channel) : myName(name), myChannel(channel) {
}

void Program::run(const std::string &command, const std::string &parameter) const {
	if (!myChannel.isNull()) {
		std::map<std::string,ZLCommunicationManager::Data>::const_iterator it = myCommandData.find(command);
		if (it != myCommandData.end()) {
			ZLCommunicationManager::Data data = it->second;
			for (ZLCommunicationManager::Data::iterator jt = data.begin(); jt != data.end(); ++jt) {
				if (!jt->second.empty() && jt->second[0] == '%') {
					const std::string optionName = jt->second.substr(1);
					std::map<std::string,std::string>::const_iterator st = myDefaultValues.find(optionName);
					jt->second = ZLStringOption(
						FBCategoryKey::EXTERNAL,
						myName,
						optionName,
						(st != myDefaultValues.end()) ? st->second : "").value();
				}
			}
			shared_ptr<ZLMessageSender> sender = myChannel->createSender(data);
			if (!sender.isNull()) {
				sender->sendStringMessage(parameter);
			}
		}
	}
}

const std::vector<Program::OptionDescription> &Program::options() const {
	return myOptions;
}

Program::OptionDescription::OptionDescription(const std::string &name, const std::string &defaultValue) : OptionName(name), DefaultValue(defaultValue) {
}

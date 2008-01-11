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

#include <ZLXMLReader.h>
#include <ZLibrary.h>

#include "ZLMessage.h"

class ZLDummyCommunicationManager : public ZLCommunicationManager {

public:
	shared_ptr<ZLMessageOutputChannel> createMessageOutputChannel(const std::string&, const std::string&) { return 0; }
	void addInputMessageDescription(const std::string&, const std::string&, const Data&) {}
};

ZLCommunicationManager *ZLCommunicationManager::ourInstance = 0;

ZLCommunicationManager &ZLCommunicationManager::instance() {
	if (ourInstance == 0) {
		ourInstance = new ZLDummyCommunicationManager();
	}
	if (!ourInstance->myInitialized) {
		ourInstance->init();
	}
	return *ourInstance;
}

void ZLCommunicationManager::deleteInstance() {
	if (ourInstance != 0) {
		delete ourInstance;
	}
	ourInstance = 0;
}

ZLCommunicationManager::ZLCommunicationManager() : myInitialized(false) {
}

ZLCommunicationManager::~ZLCommunicationManager() {
}

class ZLMessageDescriptionReader : public ZLXMLReader {

public:
	ZLMessageDescriptionReader(ZLCommunicationManager &manager);

private:
	void startElementHandler(const char *tag, const char **attributes);	

private:
	ZLCommunicationManager &myManager;
};

ZLMessageDescriptionReader::ZLMessageDescriptionReader(ZLCommunicationManager &manager) : myManager(manager) {
}

void ZLMessageDescriptionReader::startElementHandler(const char *tag, const char **attributes) {
	if ((std::string("message") == tag) && (attributes != 0)) {
		std::string command;
		std::string protocol;
		ZLCommunicationManager::Data data;
		const std::string COMMAND_ATTRIBUTE = "command";
		const std::string PROTOCOL_ATTRIBUTE = "protocol";
		for (; (*attributes != 0) && (*(attributes + 1) != 0); attributes += 2) {
			if (COMMAND_ATTRIBUTE == *attributes) {
				command = *(attributes + 1);
			} else if (PROTOCOL_ATTRIBUTE == *attributes) {
				protocol = *(attributes + 1);
			} else {
				data[*attributes] = *(attributes + 1);
			}
		}
		if (!command.empty() && !protocol.empty()) {
			myManager.addInputMessageDescription(command, protocol, data);
		}
	}
}

void ZLCommunicationManager::init() {
	ZLMessageDescriptionReader(*this).readDocument(ZLibrary::ZLibraryDirectory() + ZLibrary::FileNameDelimiter + "messages.xml");
	ZLMessageDescriptionReader(*this).readDocument(ZLibrary::DefaultFilesPathPrefix() + "messages.xml");
	myInitialized = true;
}

void ZLCommunicationManager::registerHandler(const std::string &command, shared_ptr<ZLMessageHandler> receiver) {
	myRegisteredHandlers[command] = receiver;
}

shared_ptr<ZLMessageHandler> ZLCommunicationManager::handler(const std::string &command) {
	std::map<std::string,weak_ptr<ZLMessageHandler> >::iterator it = myRegisteredHandlers.find(command);
	if (it == myRegisteredHandlers.end()) {
		return 0;
	}
	if (it->second.isNull()) {
		myRegisteredHandlers.erase(it);
		return 0;
	}
	return it->second;
}

void ZLCommunicationManager::onMessageReceived(const std::string &command, const std::vector<std::string> &arguments) {
	shared_ptr<ZLMessageHandler> h = handler(command);
	if (!h.isNull()) {
		h->onMessageReceived(arguments);
	}
}

ZLMessageOutputChannel::~ZLMessageOutputChannel() {
}

ZLMessageSender::~ZLMessageSender() {
}

ZLMessageHandler::~ZLMessageHandler() {
}

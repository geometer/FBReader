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

#include "ZLMessage.h"

class ZLDummyCommunicationManager : public ZLCommunicationManager {

public:
	shared_ptr<ZLCommunicator> createCommunicator(const std::string&, const std::string&) { return 0; }
};

ZLCommunicationManager *ZLCommunicationManager::ourInstance = 0;

ZLCommunicationManager &ZLCommunicationManager::instance() {
	if (ourInstance == 0) {
		ourInstance = new ZLDummyCommunicationManager();
	}
	return *ourInstance;
}

void ZLCommunicationManager::deleteInstance() {
	if (ourInstance != 0) {
		delete ourInstance;
	}
	ourInstance = 0;
}

ZLCommunicationManager::ZLCommunicationManager() {
}

ZLCommunicationManager::~ZLCommunicationManager() {
}

ZLCommunicator::~ZLCommunicator() {
}

ZLMessageSender::~ZLMessageSender() {
}

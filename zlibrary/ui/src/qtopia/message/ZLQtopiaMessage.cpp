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

#include <qpe/qcopenvelope_qws.h>

#include <ZLFile.h>

#include "ZLQtopiaMessage.h"

ZLQtopiaCommunicationManager::ZLQtopiaCommunicationManager() {
}

void ZLQtopiaCommunicationManager::createInstance() {
	if (ourInstance == 0) {
		ourInstance = new ZLQtopiaCommunicationManager();
	}
}

shared_ptr<ZLMessageOutputChannel> ZLQtopiaCommunicationManager::createMessageOutputChannel(const std::string &protocol, const std::string &testFile) {
	if (protocol != "QCop") {
		return 0;
	}

	if (!testFile.empty() && !ZLFile(testFile).exists()) {
		return 0;
	}

	return new ZLQCopMessageOutputChannel();
}

shared_ptr<ZLMessageSender> ZLQCopMessageOutputChannel::createSender(const ZLCommunicationManager::Data &data) {
	ZLCommunicationManager::Data::const_iterator it = data.find("channel");
	if (it == data.end()) {
		return 0;
	}
	const std::string &channel = it->second;
	it = data.find("method");
	if (it == data.end()) {
		return 0;
	}
	const std::string &method = it->second;
	return (!channel.empty() && !method.empty()) ? new ZLQCopMessageSender(channel, method) : 0;
}

ZLQCopMessageSender::ZLQCopMessageSender(const std::string &channel, const std::string &method) : myChannel(channel.c_str()), myMethod(method.c_str()) {
}

void ZLQCopMessageSender::sendStringMessage(const std::string &message) {
	QCopEnvelope env(myChannel, myMethod);
	env << QString::fromUtf8(message.c_str());
}

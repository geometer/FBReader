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

#include <ZLFile.h>
#include <ZLApplication.h>

#include "ZLMaemoMessage.h"

ZLMaemoCommunicationManager::ZLMaemoCommunicationManager() : myContext(0) {
}

void ZLMaemoCommunicationManager::init() {
	myContext = osso_initialize(ZLApplication::ApplicationName().c_str(), "0.0", false, 0);
}

void ZLMaemoCommunicationManager::shutdown() {
	osso_deinitialize(myContext);
	myContext = 0;
}

void ZLMaemoCommunicationManager::createInstance() {
	if (ourInstance == 0) {
		ourInstance = new ZLMaemoCommunicationManager();
	}
}

shared_ptr<ZLCommunicator> ZLMaemoCommunicationManager::createCommunicator(const std::string &protocol, const std::string &testFile) {
	if (protocol != "osso-rpc") {
		return 0;
	}

	if (!testFile.empty() && !ZLFile(testFile).exists()) {
		return 0;
	}

	return new ZLMaemoRpcCommunicator(myContext);
}

ZLMaemoRpcCommunicator::ZLMaemoRpcCommunicator(osso_context_t *&context) : myContext(context) {
}

shared_ptr<ZLMessageSender> ZLMaemoRpcCommunicator::createSender(const ZLCommunicationManager::Data &data) {
	ZLCommunicationManager::Data::const_iterator it = data.find("service");
	if (it == data.end()) {
		return 0;
	}
	const std::string &service = it->second;

	ZLCommunicationManager::Data::const_iterator jt = data.find("command");
	if (jt == data.end()) {
		return 0;
	}
	const std::string &command = jt->second;

	return (!service.empty() && !command.empty()) ? new ZLMaemoRpcMessageSender(myContext, service, command) : 0;
}

ZLMaemoRpcMessageSender::ZLMaemoRpcMessageSender(osso_context_t *&context, const std::string &service, const std::string &command) : myContext(context), myService(service), myCommand(command) {
}

void ZLMaemoRpcMessageSender::sendStringMessage(const std::string &message) {
	osso_rpc_run_with_defaults(myContext, myService.c_str(), myCommand.c_str(), 0, DBUS_TYPE_STRING, message.c_str(), DBUS_TYPE_INVALID);
}

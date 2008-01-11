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

#include <ZLFile.h>
#include <ZLibrary.h>

#include "ZLMaemoMessage.h"

ZLMaemoCommunicationManager::ZLMaemoCommunicationManager() : myContext(0) {
}

static gint onMessageReceived(const gchar *interface, const gchar *method, GArray *arguments, gpointer data, osso_rpc_t *retval) {
	return ((ZLMaemoCommunicationManager*)data)->onMessageReceived(interface, method, arguments, retval);
}

gint ZLMaemoCommunicationManager::onMessageReceived(const gchar*, const gchar *method, GArray *arguments, osso_rpc_t*) {
	if (method != 0) {
		std::map<std::string,std::string>::const_iterator it = myOssoMethodToCommand.find(method);
		if (it != myOssoMethodToCommand.end()) {
			const std::string &command = it->second;
			std::map<std::string,std::string>::const_iterator jt = myOssoMethodToConverter.find(method);
			bool convertUrlToFileName = (jt != myOssoMethodToConverter.end()) && (jt->second == "UrlToFilename");
			std::vector<std::string> vArguments;
			if (arguments != 0) {
				for (guint i = 0; i < arguments->len; ++i) {
					if (g_array_index(arguments, osso_rpc_t, 0).type == DBUS_TYPE_STRING) {
						std::string arg = g_array_index(arguments, osso_rpc_t, 0).value.s;
						if (convertUrlToFileName) {
							vArguments.push_back(g_filename_from_uri(arg.c_str(), 0, 0));
						} else {
							vArguments.push_back(arg);
						}
					}
				}
			}
			ZLCommunicationManager::onMessageReceived(command, vArguments);
		}
	}
	return OSSO_OK;
}

void ZLMaemoCommunicationManager::init() {
	myContext = osso_initialize(ZLibrary::ApplicationName().c_str(), "0.0", false, 0);
	osso_rpc_set_cb_f_with_free(
		myContext,
		("com.nokia." + ZLibrary::ApplicationName()).c_str(),
		("com/nokia/" + ZLibrary::ApplicationName()).c_str(),
		("com.nokia." + ZLibrary::ApplicationName()).c_str(),
		::onMessageReceived,
		this,
		(osso_rpc_retval_free_f*)osso_rpc_free_val
	);
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

shared_ptr<ZLMessageOutputChannel> ZLMaemoCommunicationManager::createMessageOutputChannel(const std::string &protocol, const std::string &testFile) {
	if (!testFile.empty() && !ZLFile(testFile).exists()) {
		return 0;
	}

	if (protocol == "osso-rpc") {
		return new ZLMaemoRpcMessageOutputChannel(myContext);
	}

	if (protocol == "present") {
		return new ZLMaemoPresentMessageOutputChannel(myContext);
	}

	return 0;
}

void ZLMaemoCommunicationManager::addInputMessageDescription(const std::string &command, const std::string &protocol, const Data &data) {
	if (protocol == "osso-rpc") {
		Data::const_iterator it = data.find("method");
		if ((it != data.end()) && !it->second.empty()) {
			myOssoMethodToCommand[it->second] = command;
			Data::const_iterator jt = data.find("converter");
			if (jt != data.end()) {
				myOssoMethodToConverter[it->second] = jt->second;
			}
		}
	}
}

ZLMaemoRpcMessageOutputChannel::ZLMaemoRpcMessageOutputChannel(osso_context_t *&context) : myContext(context) {
}

shared_ptr<ZLMessageSender> ZLMaemoRpcMessageOutputChannel::createSender(const ZLCommunicationManager::Data &data) {
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

ZLMaemoPresentMessageOutputChannel::ZLMaemoPresentMessageOutputChannel(osso_context_t *&context) : ZLMaemoRpcMessageOutputChannel(context) {
}

shared_ptr<ZLMessageSender> ZLMaemoPresentMessageOutputChannel::createSender(const ZLCommunicationManager::Data &data) {
	ZLCommunicationManager::Data copy = data;
	copy["service"] = copy["toPresent"];
	copy["command"] = "present";
	return ZLMaemoRpcMessageOutputChannel::createSender(copy);
}

ZLMaemoRpcMessageSender::ZLMaemoRpcMessageSender(osso_context_t *&context, const std::string &service, const std::string &command) : myContext(context), myService(service), myCommand(command) {
}

void ZLMaemoRpcMessageSender::sendStringMessage(const std::string &message) {
	osso_rpc_run_with_defaults(myContext, myService.c_str(), myCommand.c_str(), 0, DBUS_TYPE_STRING, message.c_str(), DBUS_TYPE_INVALID);
}

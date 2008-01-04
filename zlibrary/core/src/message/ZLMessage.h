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

#ifndef __ZLMESSAGE_H__
#define __ZLMESSAGE_H__

#include <vector>
#include <map>
#include <string>

#include <shared_ptr.h>

class ZLMessageOutputChannel;
class ZLMessageSender;

class ZLMessageHandler;

class ZLCommunicationManager {

public:
	static ZLCommunicationManager &instance();
	static void deleteInstance();

protected:
	static ZLCommunicationManager *ourInstance;

public:
	typedef std::map<std::string,std::string> Data;

protected:
	ZLCommunicationManager();
	virtual ~ZLCommunicationManager();
	void init();
	void onMessageReceived(const std::string &command, const std::vector<std::string> &arguments);

public:
	virtual shared_ptr<ZLMessageOutputChannel> createMessageOutputChannel(const std::string &protocol, const std::string &testFile) = 0;
	virtual void addInputMessageDescription(const std::string &command, const std::string &protocol, const Data &data) = 0;
	void registerHandler(const std::string &command, shared_ptr<ZLMessageHandler> receiver);

private:
	shared_ptr<ZLMessageHandler> handler(const std::string &command);

private:
	std::map<std::string,weak_ptr<ZLMessageHandler> >	myRegisteredHandlers;
	bool myInitialized;

private:
	ZLCommunicationManager(const ZLCommunicationManager&);
	const ZLCommunicationManager &operator = (const ZLCommunicationManager&);
};

class ZLMessageOutputChannel {

public:
	virtual ~ZLMessageOutputChannel();

	virtual shared_ptr<ZLMessageSender> createSender(const ZLCommunicationManager::Data &data) = 0;
};

class ZLMessageSender {

public:
	virtual ~ZLMessageSender();

	virtual void sendStringMessage(const std::string &message) = 0;
};

class ZLMessageHandler {

public:
	virtual ~ZLMessageHandler();
	virtual void onMessageReceived(const std::vector<std::string> &arguments) = 0;
};

#endif /* __ZLMESSAGE_H__ */

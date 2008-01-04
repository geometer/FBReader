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

#ifndef __ZLQTOPIAMESSAGE_H__
#define __ZLQTOPIAMESSAGE_H__

#include <qstring.h>

#include <ZLMessage.h>

class ZLQtopiaCommunicationManager : public ZLCommunicationManager {

public:
	static void createInstance();

	shared_ptr<ZLMessageOutputChannel> createMessageOutputChannel(const std::string &protocol, const std::string &testFile);
	void addInputMessageDescription(const std::string&, const std::string&, const Data&) {}

private:
	ZLQtopiaCommunicationManager();
};

class ZLQCopMessageOutputChannel : public ZLMessageOutputChannel {

public:
	shared_ptr<ZLMessageSender> createSender(const ZLCommunicationManager::Data &data);
};

class ZLQCopMessageSender : public ZLMessageSender {

private:
	ZLQCopMessageSender(const std::string &channel, const std::string &method);

public:
	void sendStringMessage(const std::string &message);

private:
	const QCString myChannel;
	const QCString myMethod;

friend class ZLQCopMessageOutputChannel;
};

#endif /* __ZLQTOPIAMESSAGE_H__ */

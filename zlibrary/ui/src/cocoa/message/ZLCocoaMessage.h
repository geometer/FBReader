/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLCOCOAMESSAGE_H__
#define __ZLCOCOAMESSAGE_H__

#include <string>

#include <ZLMessage.h>

class ZLCocoaCommunicationManager : public ZLCommunicationManager {

public:
	static void createInstance();

	shared_ptr<ZLMessageOutputChannel> createMessageOutputChannel(const std::string &protocol, const std::string &testFile);
	void addInputMessageDescription(const std::string&, const std::string&, const Data&) {}

private:
	ZLCocoaCommunicationManager();
};

class ZLCocoaOpenURLMessageOutputChannel : public ZLMessageOutputChannel {

public:
	shared_ptr<ZLMessageSender> createSender(const ZLCommunicationManager::Data &data);
};

class ZLCocoaOpenURLMessageSender : public ZLMessageSender {

private:
	ZLCocoaOpenURLMessageSender();

public:
	void sendStringMessage(const std::string &message);

friend class ZLCocoaOpenURLMessageOutputChannel;
};

#endif /* __ZLCOCOAMESSAGE_H__ */

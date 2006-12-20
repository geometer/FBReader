/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLMESSAGE_H__
#define __ZLMESSAGE_H__

#include <shared_ptr.h>
#include <map>
#include <string>

class ZLCommunicator;
class ZLMessageSender;

class ZLCommunicationManager {

public:
	static ZLCommunicationManager &instance();
	static void deleteInstance();

public:
	typedef std::map<std::string,std::string> Data;

	virtual shared_ptr<ZLCommunicator> createCommunicator(const std::string &protocol, const std::string &testFile) = 0;

protected:
	ZLCommunicationManager();
	virtual ~ZLCommunicationManager();

protected:
	static ZLCommunicationManager *ourInstance;

private:
	ZLCommunicationManager(const ZLCommunicationManager&);
	const ZLCommunicationManager &operator = (const ZLCommunicationManager&);
};

class ZLCommunicator {

public:
	virtual ~ZLCommunicator();

	virtual shared_ptr<ZLMessageSender> createSender(const ZLCommunicationManager::Data &data) = 0;
};

class ZLMessageSender {

public:
	virtual ~ZLMessageSender();

	virtual void sendStringMessage(const std::string &message) = 0;
};

#endif /* __ZLMESSAGE_H__ */

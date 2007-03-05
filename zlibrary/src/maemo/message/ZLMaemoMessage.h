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

#ifndef __ZLMAEMOMESSAGE_H__
#define __ZLMAEMOMESSAGE_H__

#include <libosso.h>

#include <ZLMessage.h>

class ZLMaemoCommunicationManager : public ZLCommunicationManager {

public:
	static void createInstance();

	shared_ptr<ZLCommunicator> createCommunicator(const std::string &protocol, const std::string &testFile);

	void init();
	void shutdown();

private:
	ZLMaemoCommunicationManager();

private:
	osso_context_t *myContext;
};

class ZLMaemoRpcCommunicator : public ZLCommunicator {

public:
	ZLMaemoRpcCommunicator(osso_context_t *&context);
	shared_ptr<ZLMessageSender> createSender(const ZLCommunicationManager::Data &data);

private:
	osso_context_t *&myContext;
};

class ZLMaemoRpcMessageSender : public ZLMessageSender {

private:
	ZLMaemoRpcMessageSender(osso_context_t *&context, const std::string &service, const std::string &command);

public:
	void sendStringMessage(const std::string &message);

private:
	osso_context_t *&myContext;
	const std::string myService;
	const std::string myCommand;

friend class ZLMaemoRpcCommunicator;
};

#endif /* __ZLMAEMOMESSAGE_H__ */

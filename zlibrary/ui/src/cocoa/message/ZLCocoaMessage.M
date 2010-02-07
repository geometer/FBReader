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

#import <Cocoa/Cocoa.h>

#include "ZLCocoaMessage.h"

ZLCocoaCommunicationManager::ZLCocoaCommunicationManager() {
}

void ZLCocoaCommunicationManager::createInstance() {
	if (ourInstance == 0) {
		ourInstance = new ZLCocoaCommunicationManager();
	}
}

shared_ptr<ZLMessageOutputChannel> ZLCocoaCommunicationManager::createMessageOutputChannel(const std::string &protocol, const std::string &testFile) {
	if (protocol == "openURL") {
		return new ZLCocoaOpenURLMessageOutputChannel();
	}
	return 0;
}

shared_ptr<ZLMessageSender> ZLCocoaOpenURLMessageOutputChannel::createSender(const ZLCommunicationManager::Data &data) {
	return new ZLCocoaOpenURLMessageSender();
}

ZLCocoaOpenURLMessageSender::ZLCocoaOpenURLMessageSender() {
}

void ZLCocoaOpenURLMessageSender::sendStringMessage(const std::string &message) {
	[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:message.c_str()]]];
}

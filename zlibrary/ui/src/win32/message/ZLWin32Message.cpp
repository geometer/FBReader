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

#include <windows.h>

#include <ZLFile.h>

#include "ZLWin32Message.h"
#include "../../../../core/src/win32/util/W32WCHARUtil.h"

ZLWin32CommunicationManager::ZLWin32CommunicationManager() {
}

void ZLWin32CommunicationManager::createInstance() {
	if (ourInstance == 0) {
		ourInstance = new ZLWin32CommunicationManager();
	}
}

shared_ptr<ZLMessageOutputChannel> ZLWin32CommunicationManager::createMessageOutputChannel(const std::string &protocol, const std::string &testFile) {
	if (protocol != "execute") {
		return 0;
	}

	if (!testFile.empty() && !ZLFile(testFile).exists()) {
		return 0;
	}

	return new ZLWin32ExecMessageOutputChannel();
}

shared_ptr<ZLMessageSender> ZLWin32ExecMessageOutputChannel::createSender(const ZLCommunicationManager::Data &data) {
	ZLCommunicationManager::Data::const_iterator it = data.find("program");
	if (it == data.end()) {
		return 0;
	}
	const std::string &program = it->second;
	return (!program.empty()) ? new ZLWin32ExecMessageSender(program) : 0;
}

ZLWin32ExecMessageSender::ZLWin32ExecMessageSender(const std::string &program) {
	::createNTWCHARString(myProgram, program);
}

void ZLWin32ExecMessageSender::sendStringMessage(const std::string &message) {
	ZLUnicodeUtil::Ucs2String messageBuffer;
	ShellExecute(0, ::wchar(myProgram), ::wchar(::createNTWCHARString(messageBuffer, message)), 0, 0, SW_SHOWNORMAL);
}

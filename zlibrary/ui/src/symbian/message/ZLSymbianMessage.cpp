#include <unistd.h>
#include <cstdlib>

#include <ZLFile.h>

#include "ZLSymbianMessage.h"


ZLSymbianCommunicationManager::ZLSymbianCommunicationManager() {
}

void ZLSymbianCommunicationManager::createInstance() {
	if (ourInstance == 0) {
		ourInstance = new ZLSymbianCommunicationManager();
	}
}

shared_ptr<ZLMessageOutputChannel> ZLSymbianCommunicationManager::createMessageOutputChannel(const std::string &protocol, const std::string &testFile) {
	if (protocol != "execute") {
		return 0;
	}

	if (!testFile.empty() && !ZLFile(testFile).exists()) {
		return 0;
	}

	return new ZLSymbianExecMessageOutputChannel();
}

shared_ptr<ZLMessageSender> ZLSymbianExecMessageOutputChannel::createSender(const ZLCommunicationManager::Data &data) {
	ZLCommunicationManager::Data::const_iterator it = data.find("command");
	if (it == data.end()) {
		return 0;
	}
	const std::string &command = it->second;
	return (!command.empty()) ? new ZLSymbianExecMessageSender(command) : 0;
}

ZLSymbianExecMessageSender::ZLSymbianExecMessageSender(const std::string &command) : myCommand(command) {
}

void ZLSymbianExecMessageSender::sendStringMessage(const std::string &message) {
	// DO NOTHING
}

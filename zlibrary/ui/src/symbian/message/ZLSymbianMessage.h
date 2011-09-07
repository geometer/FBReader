#ifndef __ZLSYMBIANMESSAGE_H__
#define __ZLSYMBIANMESSAGE_H__

#include <string>

#include <ZLMessage.h>

class ZLSymbianCommunicationManager : public ZLCommunicationManager {

public:
	static void createInstance();

	shared_ptr<ZLMessageOutputChannel> createMessageOutputChannel(const std::string &protocol, const std::string &testFile);
	void addInputMessageDescription(const std::string&, const std::string&, const Data&) {}

private:
	ZLSymbianCommunicationManager();
};

class ZLSymbianExecMessageOutputChannel : public ZLMessageOutputChannel {

public:
	shared_ptr<ZLMessageSender> createSender(const ZLCommunicationManager::Data &data);
};

class ZLSymbianExecMessageSender : public ZLMessageSender {

private:
	ZLSymbianExecMessageSender(const std::string &command);

public:
	void sendStringMessage(const std::string &message);

private:
	std::string myCommand;

friend class ZLSymbianExecMessageOutputChannel;
};

#endif /* __ZLSYMBIANMESSAGE_H__ */

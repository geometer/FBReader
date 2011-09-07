#include "ZLQtNetworkManager.h"

void ZLQtNetworkManager::createInstance() {
	ourInstance = new ZLQtNetworkManager();
}


std::string ZLQtNetworkManager::perform(const ZLExecutionData::Vector &dataList) const {
	return std::string();
}

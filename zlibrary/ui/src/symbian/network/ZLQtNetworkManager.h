#ifndef __ZLQTNETWORKMANAGER_H__
#define __ZLQTNETWORKMANAGER_H__

#include <ZLNetworkManager.h>

class ZLQtNetworkManager : public ZLNetworkManager {

public:
	static void createInstance();

	std::string perform(const ZLExecutionData::Vector &dataList) const;

};

#endif /* __ZLQTNETWORKMANAGER_H__ */

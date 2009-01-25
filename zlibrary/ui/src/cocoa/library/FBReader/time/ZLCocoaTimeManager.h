/*
 *  ZLCocoaTimeManager.h
 *  FBReader
 *
 *  Created by Alexey Vyskubov on 25.01.09.
 *
 */

#ifndef __ZLCOCOATIMEMANAGER_H__
#define __ZLCOCOATIMEMANAGER_H__

#include <map>
#include "../../../../../../core/src/unix/time/ZLUnixTime.h"

class ZLCocoaTimeManager : public ZLUnixTimeManager {
	
public:
	static void createInstance() {
		ourInstance = new ZLCocoaTimeManager();
	}
	void addTask(shared_ptr<ZLRunnable> task, int interval);
	void removeTask(shared_ptr<ZLRunnable> task);
	
private:
	std::map<shared_ptr<ZLRunnable>,int> myHandlers;
};


#endif
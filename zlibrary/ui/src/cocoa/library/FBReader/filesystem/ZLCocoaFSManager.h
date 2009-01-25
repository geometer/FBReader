/*
 *  ZLCocoaFSManager.h
 *  FBReader
 *
 *  Created by Alexey Vyskubov on 25.01.09.
 *
 */
#ifndef __ZLCOCOAFSMANAGER_H__
#define __ZLCOCOAFSMANAGER_H__

#include "../../../../../../core/src/unix/filesystem/ZLUnixFSManager.h"

class ZLCocoaFSManager : public ZLUnixFSManager {
	
public:
	static void createInstance() {
		ourInstance = new ZLCocoaFSManager();
	}
private:
	ZLCocoaFSManager() {}
protected:
	std::string convertFilenameToUtf8(const std::string &name) const;
};
#endif

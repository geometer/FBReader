#ifndef __ZLSYMBIANFSMANAGER_H__
#define __ZLSYMBIANFSMANAGER_H__

#include "../../../../core/src/unix/filesystem/ZLUnixFSDir.h"
#include "../../qt4/filesystem/ZLQtFSManager.h"

class ZLSymbianFSManager : public ZLQtFSManager {

public:
        static void createInstance() { ourInstance = new ZLSymbianFSManager(); }
	
protected:
        ZLSymbianFSManager() {}
	
protected:
	void normalizeRealPath(std::string &path) const;

private:
	ZLFSDir *createNewDirectory(const std::string &path) const;
	std::string resolveSymlink(const std::string &path) const;
	int findArchiveFileNameDelimiter(const std::string &path) const;
	ZLFSDir *createPlainDirectory(const std::string &path) const;
	shared_ptr<ZLDir> rootDirectory() const;
	const std::string &rootDirectoryPath() const;
	std::string parentPath(const std::string &path) const;

};

#endif /* __ZLSYMBIANFSMANAGER_H__ */

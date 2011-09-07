#ifndef __ZLQTFSMANAGER_H__
#define __ZLQTFSMANAGER_H__

#include "../../../../core/src/unix/filesystem/ZLUnixFSManager.h"
#include "../../../../core/src/unix/filesystem/ZLUnixFSDir.h"

class ZLQtFSManager : public ZLUnixFSManager {

public:
	static void createInstance() { ourInstance = new ZLQtFSManager(); }
	
private:
	ZLQtFSManager() {}
	
protected:
	std::string convertFilenameToUtf8(const std::string &name) const;
	std::string mimeType(const std::string &path) const;

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

#endif /* __ZLQTFSMANAGER_H__ */

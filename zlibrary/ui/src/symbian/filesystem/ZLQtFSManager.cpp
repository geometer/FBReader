#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <QtCore/QString>
#include <QtCore/QDir>
#include <ZLLogger.h>

#include <set>

#include <ZLibrary.h>
#include <ZLStringUtil.h>

#include "ZLQtFSManager.h"


std::string ZLQtFSManager::convertFilenameToUtf8(const std::string &name) const {
	if (name.empty()) {
		return name;
	}

	QString qString = QString::fromLocal8Bit(name.c_str());
	return (qString == QString::null) ? "" : (const char*)qString.toUtf8();
}

std::string ZLQtFSManager::mimeType(const std::string &path) const {
	// TODO: implement
	return std::string();
}


static std::string getPwdDir() {
	return QDir::current().absolutePath().replace("/",QString::fromStdString(ZLibrary::FileNameDelimiter)).toStdString();
}

static std::string getHomeDir() {
	return QDir::home().absolutePath().replace("/",QString::fromStdString(ZLibrary::FileNameDelimiter)).toStdString();
}

std::string ZLQtFSManager::resolveSymlink(const std::string &path) const {
		return path;
}

ZLFSDir *ZLQtFSManager::createPlainDirectory(const std::string &path) const {
	return new ZLUnixFSDir(path);
}

void ZLQtFSManager::normalizeRealPath(std::string &path) const {

#ifndef __SYMBIAN__
	//TODO write ZLQtFSManager using only Qt instruments for this (to avoid these hacks)
	ZLUnixFSManager::normalizeRealPath(path);
	return;
#endif

	// TODO
	// code written in this method was taken from win32 normalizeRealPath (with some additions)
	// and may be does not check all cases

	static std::string HomeDir = getHomeDir();
	static std::string PwdDir = getPwdDir();

	// Qt in some cases (resolving current dir, home dir) returns filepaths with slashes,
	// that should be replaced with backslashes:

	QString qpath = QString::fromStdString(path);
	qpath.replace("/",QString::fromStdString(ZLibrary::FileNameDelimiter));
	qpath.replace("!!", QString::fromStdString(PwdDir));  //TODO remove !! hardcoding
	path = qpath.toStdString();

	if (path[0] == '~') {
		path = HomeDir + path.substr(1);
	} else if ((path.length() > 1) && (path[1] != ':') &&
													!ZLStringUtil::stringStartsWith(path, "\\\\")) {
			path = PwdDir + ZLibrary::FileNameDelimiter + path;
	}

	int index;
	while ((index = path.find(ZLibrary::FileNameDelimiter+"..")) != -1) {
		int prevIndex = path.rfind(ZLibrary::FileNameDelimiter[0], index - 1);
			if (prevIndex == -1) {
					break;
			}
			path.erase(prevIndex, index + 3 - prevIndex);
	}
	while ((index = path.find(ZLibrary::FileNameDelimiter+ZLibrary::FileNameDelimiter, 1)) > 0) {
			path.erase(index, 1);
	}

	if (!path.empty()) {
			path[0] = toupper(path[0]);
	}

}


int ZLQtFSManager::findArchiveFileNameDelimiter(const std::string &path) const {
	int index = path.rfind(':');
	return (index == 1) ? -1 : index;
}

static const std::string RootPath = ROOTPATH;


shared_ptr<ZLDir> ZLQtFSManager::rootDirectory() const {
		return (ZLDir*)createPlainDirectory(RootPath);
}

const std::string &ZLQtFSManager::rootDirectoryPath() const {
		return RootPath;
}

std::string ZLQtFSManager::parentPath(const std::string &path) const {
		if (path == RootPath) {
				return path;
		}
		int index = findLastFileNameDelimiter(path);
		return (index <= 0) ? RootPath : path.substr(0, index);
}

ZLFSDir *ZLQtFSManager::createNewDirectory(const std::string &path) const {
		std::vector<std::string> subpaths;
		std::string current = path;

		while (current.length() > 1) {
				struct stat fileStat;
				if (stat(current.c_str(), &fileStat) == 0) {
						if (!S_ISDIR(fileStat.st_mode)) {
								return 0;
						}
						break;
				} else {
						subpaths.push_back(current);
						int index = current.rfind('\\');
						if (index == -1) {
								return 0;
						}
						current.erase(index);
				}
		}

		for (int i = subpaths.size() - 1; i >= 0; --i) {
				if (mkdir(subpaths[i].c_str(), 0x1FF) != 0) {
						return 0;
				}
		}
		return createPlainDirectory(path);
}

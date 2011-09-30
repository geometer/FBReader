#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>
#include <ZLLogger.h>

#include <set>

#include <ZLibrary.h>
#include <ZLStringUtil.h>

#include "ZLQtFSManager.h"
#include "ZLQtFileInputStream.h"
#include "ZLQtFileOutputStream.h"
#include "ZLQtFSDir.h"


ZLQtFSManager::ZLQtFSManager() : myRootPath(QDir::rootPath().toStdString()) {
        connect(&myWatcher, SIGNAL(directoryChanged(QString)), SLOT(onPathChanged(QString)));
        connect(&myWatcher, SIGNAL(fileChanged(QString)), SLOT(onPathChanged(QString)));
}

ZLQtFSManager::~ZLQtFSManager() {
}

static QString fixPath(const QString &path) {
        if (path.startsWith('~'))
            return QDir::homePath().append(path.midRef(1));
        else if (path.isEmpty())
            return QDir::homePath();
        else
            return path;
}

void ZLQtFSManager::addWatcher(const std::string &path, shared_ptr<ZLFSWatcher> watcher) {
        QString qPath = fixPath(QString::fromStdString(path));
        myWatchers.insert(qPath, watcher);
        myWatcher.addPath(qPath);
}

void ZLQtFSManager::removeWatcher(const std::string &path, shared_ptr<ZLFSWatcher> watcher) {
        QString qPath = fixPath(QString::fromStdString(path));
        myWatchers.remove(qPath, watcher);
        if (!myWatchers.contains(qPath))
                myWatcher.removePath(qPath);
}

void ZLQtFSManager::onPathChanged(const QString &path) {
        std::string stlPath = path.toStdString();
        foreach (shared_ptr<ZLFSWatcher> watcher, myWatchers.values(path))
                watcher->onPathChanged(stlPath);
}

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

//static std::string getPwdDir() {
//	return QDir::current().absolutePath().replace("/",QString::fromStdString(ZLibrary::FileNameDelimiter)).toStdString();
//}

//static std::string getHomeDir() {
//	return QDir::home().absolutePath().replace("/",QString::fromStdString(ZLibrary::FileNameDelimiter)).toStdString();
//}

std::string ZLQtFSManager::resolveSymlink(const std::string &path) const {
    QString qpath = QString::fromStdString(path);
    QString symLink = QFile::symLinkTarget(qpath);
    return symLink.isEmpty() ? path : symLink.toStdString();
}

ZLFSDir *ZLQtFSManager::createPlainDirectory(const std::string &path) const {
        return new ZLQtFSDir(path);
}

void ZLQtFSManager::normalizeRealPath(std::string &path) const {
	QString qpath = QString::fromStdString(path);
        //qpath.replace("!!", QString::fromStdString(PwdDir));  //TODO remove !! hardcoding
        qpath = fixPath(qpath);
        qpath = QDir::cleanPath(qpath);

        path = qpath.toStdString();
}


int ZLQtFSManager::findArchiveFileNameDelimiter(const std::string &path) const {
        QString qpath = QString::fromStdString(path);
        int index = qpath.lastIndexOf(":");
	return (index == 1) ? -1 : index;
}

shared_ptr<ZLDir> ZLQtFSManager::rootDirectory() const {
                return (ZLDir*)createPlainDirectory(rootDirectoryPath());
}

const std::string &ZLQtFSManager::rootDirectoryPath() const {
                return myRootPath;
}

std::string ZLQtFSManager::parentPath(const std::string &path) const {
                QDir dir(QString::fromStdString(path));
                if (!dir.cdUp()) {
                    return rootDirectoryPath();
                }
                return dir.absolutePath().toStdString();
}

ZLFSDir *ZLQtFSManager::createNewDirectory(const std::string &path) const {
    //TODO check is it working right;
    QDir().mkpath(QString::fromStdString(path));
    return createPlainDirectory(path);
}

ZLInputStream *ZLQtFSManager::createPlainInputStream(const std::string &path) const {
    return new ZLQtFileInputStream(path);
}

ZLOutputStream *ZLQtFSManager::createOutputStream(const std::string &path) const {
    return new ZLQtFileOutputStream(path);
}

ZLFileInfo ZLQtFSManager::fileInfo(const std::string &path) const {
    QFileInfo fileInfo(QString::fromStdString(path));
    ZLFileInfo info;
    info.Exists = fileInfo.exists();
    if (info.Exists) {
        info.IsDirectory = fileInfo.isDir();
        info.Size = fileInfo.size();
    }
    return info;
}

bool ZLQtFSManager::removeFile(const std::string &path) const {
    return QFile::remove(QString::fromStdString(path));
}

bool ZLQtFSManager::canRemoveFile(const std::string &path) const {
    QFile file(QString::fromStdString(path));
    return file.exists() && (file.permissions() & QFile::WriteUser);
}

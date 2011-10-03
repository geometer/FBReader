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
        //qDebug() << Q_FUNC_INFO;
        connect(&myWatcher, SIGNAL(directoryChanged(QString)), SLOT(onPathChanged(QString)));
        connect(&myWatcher, SIGNAL(fileChanged(QString)), SLOT(onPathChanged(QString)));
}

ZLQtFSManager::~ZLQtFSManager() {
}

static QString fixPath(const QString &path) {
        //qDebug() << Q_FUNC_INFO << path;
        if (path.startsWith('~'))
            return QDir::homePath().append(path.midRef(1));
        else if (path.startsWith('!')) {
            //QString usr("/usr");
            //return usr.append(path.midRef(1));
            return QDir::currentPath().append(path.midRef(1));
        }
        else if (path.isEmpty())
            return QDir::homePath();
        else
            return path;
}

void ZLQtFSManager::addWatcher(const std::string &path, shared_ptr<ZLFSWatcher> watcher) {
    qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
        QString qPath = fixPath(QString::fromStdString(path));
        myWatchers.insert(qPath, watcher);
        myWatcher.addPath(qPath);
}

void ZLQtFSManager::removeWatcher(const std::string &path, shared_ptr<ZLFSWatcher> watcher) {
    qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
        QString qPath = fixPath(QString::fromStdString(path));
        myWatchers.remove(qPath, watcher);
        if (!myWatchers.contains(qPath))
                myWatcher.removePath(qPath);
}

void ZLQtFSManager::onPathChanged(const QString &path) {
    qDebug() << Q_FUNC_INFO << path;
        std::string stlPath = path.toStdString();
        foreach (shared_ptr<ZLFSWatcher> watcher, myWatchers.values(path))
                watcher->onPathChanged(stlPath);
}

std::string ZLQtFSManager::convertFilenameToUtf8(const std::string &name) const {
    //qDebug() << Q_FUNC_INFO << QString::fromStdString(name);
	if (name.empty()) {
		return name;
	}

	QString qString = QString::fromLocal8Bit(name.c_str());
	return (qString == QString::null) ? "" : (const char*)qString.toUtf8();
}

std::string ZLQtFSManager::mimeType(const std::string &path) const {
   // qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
	// TODO: implement
	return std::string();
}

std::string ZLQtFSManager::resolveSymlink(const std::string &path) const {
 //   qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
    QString qpath = QString::fromStdString(path);
    QString symLink = QFile::symLinkTarget(qpath);
    return symLink.isEmpty() ? path : symLink.toStdString();
}

std::string ZLQtFSManager::toNativeFileName(const std::string &path) const {
   // qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
    std::string tmpPath(path);
    //TODO may be use normalise() of ZLFSManager here?
    normalizeRealPath(tmpPath);
    return QDir::toNativeSeparators(QString::fromStdString(tmpPath)).toStdString();
}

ZLFSDir *ZLQtFSManager::createPlainDirectory(const std::string &path) const {
   //     qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
        return new ZLQtFSDir(path);
}

void ZLQtFSManager::normalizeRealPath(std::string &path) const {
    //    qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
	QString qpath = QString::fromStdString(path);
        qpath = fixPath(qpath);
        qpath = QDir::cleanPath(qpath);
        path = qpath.toStdString();
}


int ZLQtFSManager::findArchiveFileNameDelimiter(const std::string &path) const {
    //qDebug() << "ZLQtFSManager::findArchiveFileNameDelimiter for " << QString::fromStdString(path);
    //    qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
        QString qpath = QString::fromStdString(path);
        int index = qpath.lastIndexOf(":");
//        qDebug() << " index of : is " << index;
	return (index == 1) ? -1 : index;
}

shared_ptr<ZLDir> ZLQtFSManager::rootDirectory() const {
   // qDebug() << Q_FUNC_INFO;
                return (ZLDir*)createPlainDirectory(rootDirectoryPath());
}

const std::string &ZLQtFSManager::rootDirectoryPath() const {
   //qDebug() << Q_FUNC_INFO;
                return myRootPath;
}

std::string ZLQtFSManager::parentPath(const std::string &path) const {
    //qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
                QDir dir(QString::fromStdString(path));
                if (!dir.cdUp()) {
                    return rootDirectoryPath();
                }
                return dir.absolutePath().toStdString();
}

ZLFSDir *ZLQtFSManager::createNewDirectory(const std::string &path) const {
   // qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
    //TODO check is it working right;
    QDir().mkpath(QString::fromStdString(path));
    return createPlainDirectory(path);
}

ZLInputStream *ZLQtFSManager::createPlainInputStream(const std::string &path) const {
  //  qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
    return new ZLQtFileInputStream(path);
}

ZLOutputStream *ZLQtFSManager::createOutputStream(const std::string &path) const {
  //  qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
    return new ZLQtFileOutputStream(path);
}

ZLFileInfo ZLQtFSManager::fileInfo(const std::string &path) const {
  //  qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
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
  //  qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
    return QFile::remove(QString::fromStdString(path));
}

bool ZLQtFSManager::canRemoveFile(const std::string &path) const {
  //  qDebug() << Q_FUNC_INFO << QString::fromStdString(path);
    QFile file(QString::fromStdString(path));
    return file.exists() && (file.permissions() & QFile::WriteUser);
}

#ifndef __ZLQTFSMANAGER_H__
#define __ZLQTFSMANAGER_H__

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QFileSystemWatcher>

#include <ZLFSManager.h>
#include "../../../../core/src/unix/filesystem/ZLUnixFSDir.h"

class ZLQtFSManager : public QObject, public ZLFSManager {
    Q_OBJECT

public:
	static void createInstance() { ourInstance = new ZLQtFSManager(); }

private:
        ZLQtFSManager();
        ~ZLQtFSManager();

public:
        std::string resolveSymlink(const std::string &path) const;
        std::string toNativeFileName(const std::string &path) const;

public:
        void addWatcher(const std::string &path, shared_ptr<ZLFSWatcher> watcher);
        void removeWatcher(const std::string &path, shared_ptr<ZLFSWatcher> watcher);

protected Q_SLOTS:
        void onPathChanged(const QString &path);

protected:
        void normalizeRealPath(std::string &path) const;
        virtual ZLInputStream *createPlainInputStream(const std::string &path) const;
        virtual ZLOutputStream *createOutputStream(const std::string &path) const;
        virtual ZLFSDir *createPlainDirectory(const std::string &path) const;
        ZLFSDir *createNewDirectory(const std::string &path) const;
        virtual ZLFileInfo fileInfo(const std::string &path) const;
        virtual bool removeFile(const std::string &path) const;
        std::string convertFilenameToUtf8(const std::string &name) const;
        std::string mimeType(const std::string &path) const;

        int findArchiveFileNameDelimiter(const std::string &path) const;
        shared_ptr<ZLDir> rootDirectory() const;
        const std::string &rootDirectoryPath() const;
        std::string parentPath(const std::string &path) const;

        virtual bool canRemoveFile(const std::string &path) const;

private:
        //QFileSystemWatcher myWatcher;
        //QMultiHash<QString, shared_ptr<ZLFSWatcher> > myWatchers;
        std::string myRootPath;

};

#endif /* __ZLQTFSMANAGER_H__ */

/*
 * Copyright (C) 2004-2011 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __ZLQTNETWORKMANAGER_H__
#define __ZLQTNETWORKMANAGER_H__

#include <ZLNetworkManager.h>
#include <ZLNetworkRequest.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkDiskCache>
#include <QtNetwork/QNetworkCookieJar>
#include <QtCore/QMutex>
#include <QtCore/QSharedPointer>

class ZLQtNetworkCache;
class ZLQtNetworkCookieJar;
class QNetworkReply;
class QEventLoop;

struct ZLQtNetworkReplyScope {
	ZLNetworkRequest *request;
	QList<QNetworkReply*> *replies;
	QStringList *errors;
	QEventLoop *eventLoop;
};

class ZLQtNetworkManager : public QObject, public ZLNetworkManager {
	Q_OBJECT
public:
	ZLQtNetworkManager();
	~ZLQtNetworkManager();
	
	static void createInstance();
	void initPaths();
	
	QNetworkCookieJar *cookieJar() const;
	std::string perform(const ZLExecutionData::Vector &dataList) const;
	
protected Q_SLOTS:
    void onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void onReplyReadyRead();
    void onFinished(QNetworkReply *reply);

private:
	bool checkReply(QNetworkReply *reply);
	QNetworkAccessManager myManager;
	ZLQtNetworkCache *myCache;
	ZLQtNetworkCookieJar *myCookieJar;
};

class ZLQtNetworkCache : public QAbstractNetworkCache {
	Q_OBJECT
public:
	ZLQtNetworkCache(QObject *parent);
	virtual ~ZLQtNetworkCache();
	
    virtual QNetworkCacheMetaData metaData(const QUrl &url);
    virtual void updateMetaData(const QNetworkCacheMetaData &metaData);
    virtual QIODevice *data(const QUrl &url);
    virtual bool remove(const QUrl &url);
    virtual qint64 cacheSize() const;

    virtual QIODevice *prepare(const QNetworkCacheMetaData &metaData);
    virtual void insert(QIODevice *device);
    virtual void clear();
	
private:
	QSharedPointer<QNetworkDiskCache> myCache;
};

class ZLQtNetworkCookieJar : public QNetworkCookieJar {
	Q_OBJECT
public:
    ZLQtNetworkCookieJar(QObject *parent = 0);
    ~ZLQtNetworkCookieJar();
	
	void setFilePath(const QString &filePath);
    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);
    void save();
	
private:
	QString myFilePath;
};

Q_DECLARE_METATYPE(shared_ptr<ZLExecutionData>)
Q_DECLARE_METATYPE(ZLQtNetworkReplyScope)

#endif /* __ZLQTNETWORKMANAGER_H__ */

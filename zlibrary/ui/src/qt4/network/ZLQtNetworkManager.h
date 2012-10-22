/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslError>
#include <QtCore/QTimer>

class ZLQtNetworkCookieJar;
class QNetworkReply;
class QEventLoop;

struct ZLQtNetworkReplyScope {
	ZLNetworkRequest *request;
	QList<QNetworkReply*> *replies;
	QStringList *errors;
	QEventLoop *eventLoop;
	QTimer* timeoutTimer;
	bool authAskedAlready; //to avoid infinite asking about user & password if they're incorrect
};

class ZLQtNetworkManager : public QObject, public ZLNetworkManager {
	Q_OBJECT
public:
	ZLQtNetworkManager();
	~ZLQtNetworkManager();

	static void createInstance();

	void initPaths();

	std::string perform(const ZLNetworkRequest::Vector &requestsList) const;

protected Q_SLOTS:
	void onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
	void onFinished(QNetworkReply *reply);
	void onReplyReadyRead();
	void onTimeOut();
	void onSslErrors(const QList<QSslError> &errors);
	//void slotError(QNetworkReply::NetworkError error);


protected:
	bool handleRedirect(QNetworkReply *reply);
	void handleHeaders(QNetworkReply *reply) const;
	void handleContent(QNetworkReply *reply) const;
	void handleErrors(QNetworkReply *reply) const;

	int timeoutValue() const;
	void prepareReply(ZLQtNetworkReplyScope &scope, QNetworkRequest networkRequest) const;

private:
	QNetworkAccessManager myManager;
	QNetworkDiskCache *myCache;
	ZLQtNetworkCookieJar *myCookieJar;
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

Q_DECLARE_METATYPE(ZLQtNetworkReplyScope);
Q_DECLARE_METATYPE(QNetworkReply*);

#endif /* __ZLQTNETWORKMANAGER_H__ */

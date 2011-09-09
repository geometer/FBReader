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

#include <ZLStringUtil.h>
#include <ZLLogger.h>
#include <ZLResource.h>
#include "ZLQtNetworkManager.h"
#include <QtCore/QVector>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QEventLoop>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QAuthenticator>
//#include "ZLQtPostDevice.h"

ZLQtNetworkManager::ZLQtNetworkManager() {
	myCache = new QNetworkDiskCache(&myManager);
	myCache->setCacheDirectory(QString::fromStdString(CacheDirectory()));
	myManager.setCache(myCache);
	myCookieJar = new ZLQtNetworkCookieJar(QString::fromStdString(CookiesPath()), &myManager);
	myManager.setCookieJar(myCookieJar);
	QObject::connect(&myManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
	                 this, SLOT(onAuthenticationRequired(QNetworkReply*,QAuthenticator*)));
	QObject::connect(&myManager, SIGNAL(finished(QNetworkReply*)),
	                 this, SLOT(onFinished(QNetworkReply*)));
}

ZLQtNetworkManager::~ZLQtNetworkManager() {
}

void ZLQtNetworkManager::createInstance() {
	ourInstance = new ZLQtNetworkManager();
}


std::string ZLQtNetworkManager::perform(const ZLExecutionData::Vector &dataList) const {
	if (useProxy()) {
		QString proxyHost = QString::fromStdString(ZLNetworkManager::proxyHost());
		QNetworkProxy proxy(QNetworkProxy::HttpProxy, proxyHost, atoi(proxyPort().c_str()));
		const_cast<QNetworkAccessManager&>(myManager).setProxy(proxy);
	}
	QList<QNetworkReply*> replies;
	QVector<bool> booleans(dataList.size(), false);
	QStringList errors;
	QEventLoop eventLoop;

	foreach (const shared_ptr<ZLExecutionData> &data, dataList) {
		if (data.isNull() || !data->isInstanceOf(ZLNetworkRequest::TYPE_ID))
			continue;
		ZLNetworkRequest &request = static_cast<ZLNetworkRequest&>(*data);
		QNetworkRequest networkRequest;
		networkRequest.setUrl(QUrl::fromUserInput(QString::fromStdString(request.url())));
		
		if (!request.doBefore()) {
			std::string error = request.errorMessage();
			if (error.empty()) {
				const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];
				error = ZLStringUtil::printf(errorResource["somethingWrongMessage"].value(),
				                             networkRequest.url().host().toStdString());
			}
			errors << QString::fromStdString(error);
			continue;
		}
		
		networkRequest.setRawHeader("User-Agent", userAgent().c_str());
		QSslConfiguration configuration;
		if (!request.sslCertificate().Path.empty()) {
			QFile file(QString::fromStdString(request.sslCertificate().Path));
			if (file.open(QFile::ReadOnly)) {
				QSslCertificate certificate(&file);
				configuration.setLocalCertificate(certificate);
			}
		} else if (!request.sslCertificate().DoVerify) {
			configuration.setPeerVerifyMode(QSslSocket::VerifyNone);
		}
		networkRequest.setSslConfiguration(configuration);
		
		QNetworkReply *reply = NULL;
		if (request.isInstanceOf(ZLNetworkPostRequest::TYPE_ID)) {
			QByteArray data;
			ZLNetworkPostRequest &postRequest = static_cast<ZLNetworkPostRequest&>(request);
			if (postRequest.postParameters().empty()) {
				const std::string &postData = postRequest.postData();
				data = QByteArray(postData.c_str(), postData.length());
			} else {
				QUrl tmp;
				typedef std::pair<std::string, std::string> string_pair;
				foreach (const string_pair &pair, postRequest.postParameters()) {
					tmp.addQueryItem(QString::fromStdString(pair.first),
					                 QString::fromStdString(pair.second));
				}
				data = tmp.encodedQuery();
			}
			reply = const_cast<QNetworkAccessManager&>(myManager).post(networkRequest, data);
		} else {
			reply = const_cast<QNetworkAccessManager&>(myManager).get(networkRequest);
		}
		Q_ASSERT(reply);
		QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(onReplyReadyRead()));
		ZLQtNetworkReplyScope scope = { &request, &booleans[replies.size()], &replies, &errors, &eventLoop };
		replies << reply;
		reply->setProperty("scope", qVariantFromValue(scope));
	}
	if (!replies.isEmpty())
		eventLoop.exec(QEventLoop::AllEvents);

	return errors.join(QLatin1String("\n")).toStdString();
}

void ZLQtNetworkManager::onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator) {
	ZLQtNetworkReplyScope scope = reply->property("scope").value<ZLQtNetworkReplyScope>();
	Q_ASSERT(scope.request);
	authenticator->setUser(QString::fromStdString(scope.request->userName()));
	authenticator->setPassword(QString::fromStdString(scope.request->password()));
}

void ZLQtNetworkManager::onReplyReadyRead() {
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	Q_UNUSED(reply);
	ZLQtNetworkReplyScope scope = reply->property("scope").value<ZLQtNetworkReplyScope>();
	QByteArray data;
	if (!*scope.headerHandled) {
		QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
		if (redirect.isValid()) {
			reply->deleteLater();
			Q_ASSERT(scope.replies->removeOne(reply));
			reply->setProperty("redirected", true);
			QNetworkRequest request = reply->request();
			request.setUrl(reply->url().resolved(redirect));
			reply = myManager.get(request);
			scope.replies->append(reply);
			QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(onReplyReadyRead()));
			reply->setProperty("scope", qVariantFromValue(scope));
			return;
		}
		
		// We should fool the request about received header
		*scope.headerHandled = true;
		data = "HTTP/1.1 ";
		data += reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toByteArray();
		data += " ";
		data += reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();
		foreach (const QNetworkReply::RawHeaderPair &pair, reply->rawHeaderPairs()) {
			data += '\n';
			data += pair.first;
			data += ": ";
			data += pair.second;
		}
		data += '\n';
		scope.request->handleHeader(data.data(), data.size());
	}
	data = reply->readAll();
	if (!data.isEmpty())
		scope.request->handleContent(data.data(), data.size());
}

void ZLQtNetworkManager::onFinished(QNetworkReply *reply) {
	reply->deleteLater();
	ZLQtNetworkReplyScope scope = reply->property("scope").value<ZLQtNetworkReplyScope>();
	Q_ASSERT(scope.request);
	if (reply->property("redirected").toBool())
		return;
	Q_ASSERT(scope.replies->removeOne(reply));
	if (scope.replies->isEmpty())
		scope.eventLoop->quit();
	if (reply->error() != QNetworkReply::NoError) {
		scope.errors->append(reply->errorString());
	} else {
		QByteArray data = reply->readAll();
		if (!data.isEmpty())
			scope.request->handleContent(data.data(), data.size());
	}
	if (!scope.request->doAfter(reply->error() == QNetworkReply::NoError))
		scope.errors->append(QString::fromStdString(scope.request->errorMessage()));
}
	
ZLQtNetworkCookieJar::ZLQtNetworkCookieJar(const QString &filePath, QObject *parent) 
    : QNetworkCookieJar(parent), myFilePath(filePath + QLatin1String("/cache.dat")) {
	QFile file(myFilePath);
	QList<QNetworkCookie> cookies;
	if (file.open(QFile::ReadOnly))
		cookies = QNetworkCookie::parseCookies(file.readAll());
	setAllCookies(cookies);
}

ZLQtNetworkCookieJar::~ZLQtNetworkCookieJar() {
	save();
}

bool ZLQtNetworkCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url) {
	bool result = QNetworkCookieJar::setCookiesFromUrl(cookieList, url);
	save();
	return result;
}

void ZLQtNetworkCookieJar::save() {
	QFile file(myFilePath);
	if (file.open(QFile::WriteOnly)) {
		bool first = true;
		foreach (const QNetworkCookie &cookie, allCookies()) {
			if (first)
				first = false;
			else
				file.write("\n", 1);
			file.write(cookie.toRawForm(QNetworkCookie::Full));
		}
	} else {
		ZLLogger::Instance().println("ZLQtNetworkCookieJar",
		                             myFilePath.toStdString() + " can't be open for writing");
	}
}

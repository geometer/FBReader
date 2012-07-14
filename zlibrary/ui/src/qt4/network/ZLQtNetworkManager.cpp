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

#include <ZLStringUtil.h>
#include <ZLLogger.h>
#include <ZLResource.h>
#include "ZLQtNetworkManager.h"
#include <QtCore/QVector>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QEventLoop>
#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QDebug>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QAuthenticator>

static QString fixPath(const QString &path) {
	if (path.startsWith('~')) {
		return QDir::homePath() + path.mid(1);
	}
	return path;
}

ZLQtNetworkManager::ZLQtNetworkManager() {
	myCache = new QNetworkDiskCache(&myManager);
	myManager.setCache(myCache);
	myCookieJar = new ZLQtNetworkCookieJar(&myManager);
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

void ZLQtNetworkManager::initPaths() {
	myCookieJar->setFilePath(fixPath(QString::fromStdString(CookiesPath())));
	QDir cacheDirectory = fixPath(QString::fromStdString(CacheDirectory()));
	if (!cacheDirectory.exists()) {
		cacheDirectory.mkpath(cacheDirectory.absolutePath());
	}
	myCache->setCacheDirectory(cacheDirectory.absolutePath());
}


std::string ZLQtNetworkManager::perform(const ZLExecutionData::Vector &dataList) const {
	if (useProxy()) {
		QString host = QString::fromStdString(proxyHost());
		QNetworkProxy proxy(QNetworkProxy::HttpProxy, host, atoi(proxyPort().c_str()));
		const_cast<QNetworkAccessManager&>(myManager).setProxy(proxy);
	}
	QList<QNetworkReply*> replies;
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
		QSslConfiguration configuration = QSslConfiguration::defaultConfiguration();
		if (!request.sslCertificate().DoVerify) {
			configuration.setPeerVerifyMode(QSslSocket::VerifyNone);
		}
		networkRequest.setSslConfiguration(configuration);

		QNetworkReply *reply = NULL;
		if (request.isInstanceOf(ZLNetworkPostRequest::TYPE_ID)) {
			QByteArray data;
			ZLNetworkPostRequest &postRequest = static_cast<ZLNetworkPostRequest&>(request);
			QUrl tmp;
			typedef std::pair<std::string, std::string> string_pair;
			foreach (const string_pair &pair, postRequest.postData()) {
				tmp.addQueryItem(QString::fromStdString(pair.first), QString::fromStdString(pair.second));
			}
			data = tmp.encodedQuery();
			reply = const_cast<QNetworkAccessManager&>(myManager).post(networkRequest, data);
		} else {
			reply = const_cast<QNetworkAccessManager&>(myManager).get(networkRequest);
		}
		ZLQtNetworkReplyScope scope = { &request, &replies, &errors, &eventLoop };
		replies.push_back(reply);
		QObject::connect(reply, SIGNAL(sslErrors(QList<QSslError>)),this, SLOT(onSslErrors(QList<QSslError>)));
		reply->setProperty("scope", qVariantFromValue(scope));
	}
	if (!replies.isEmpty()) {
		eventLoop.exec(QEventLoop::AllEvents);
	}

	return errors.join(QLatin1String("\n")).toStdString();
}

void ZLQtNetworkManager::onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator) {
	ZLQtNetworkReplyScope scope = reply->property("scope").value<ZLQtNetworkReplyScope>();
	authenticator->setUser(QString::fromStdString(scope.request->userName()));
	authenticator->setPassword(QString::fromStdString(scope.request->password()));
}

void ZLQtNetworkManager::onSslErrors(const QList<QSslError> &errors) {
	qDebug() << Q_FUNC_INFO << errors;
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	QList<QSslError> ignoredErrors = errors;
	foreach (const QSslError &error, errors) {
			ignoredErrors << error;
	}
	reply->ignoreSslErrors(ignoredErrors);
}

void ZLQtNetworkManager::onFinished(QNetworkReply *reply) {
	reply->deleteLater();
	ZLQtNetworkReplyScope scope = reply->property("scope").value<ZLQtNetworkReplyScope>();
	qDebug() << Q_FUNC_INFO << reply->url();
	if (handleRedirect(reply)) {
		return;
	}
	handleHeaders(reply);
	handleContent(reply);

	if (!scope.request->doAfter(reply->error() == QNetworkReply::NoError)) {
		scope.errors->append(QString::fromStdString(scope.request->errorMessage()));
	}

	scope.replies->removeOne(reply);
	if (scope.replies->isEmpty()) {
		scope.eventLoop->quit();
	}
}

bool ZLQtNetworkManager::handleRedirect(QNetworkReply *reply) {
	ZLQtNetworkReplyScope scope = reply->property("scope").value<ZLQtNetworkReplyScope>();
	if (!scope.request->isRedirectionSupported()) {
		return false;
	}
	QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redirect.isValid()) {
		return false;
	}
	QObject::disconnect(reply, 0, this, 0);
	scope.replies->removeOne(reply);
	QNetworkRequest request = reply->request();
	request.setUrl(reply->url().resolved(redirect));
	reply = myManager.get(request);
	scope.replies->append(reply);
	reply->setProperty("scope", qVariantFromValue(scope));
	return true;
}

void ZLQtNetworkManager::handleHeaders(QNetworkReply *reply) {
	ZLQtNetworkReplyScope scope = reply->property("scope").value<ZLQtNetworkReplyScope>();
	QByteArray data = "HTTP/1.1 ";
	data += reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toByteArray();
	data += " ";
	data += reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();
	scope.request->handleHeader(data.data(), data.size());
	foreach (const QByteArray &headerName, reply->rawHeaderList()) {
		data  = headerName;
		QByteArray value = reply->rawHeader(headerName);
		data += ": ";
		data += value;
		if (headerName.toLower() == QString("content-encoding") && value.toLower().contains("gzip")) {
			continue;
		}
		scope.request->handleHeader(data.data(), data.size());
	}
}

void ZLQtNetworkManager::handleContent(QNetworkReply *reply) {
	ZLQtNetworkReplyScope scope = reply->property("scope").value<ZLQtNetworkReplyScope>();
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray data = reply->readAll();
		if (!data.isEmpty()) {
			scope.request->handleContent(data.data(), data.size());
		}
	} else {
		scope.errors->append(reply->errorString());
	}
}

ZLQtNetworkCookieJar::ZLQtNetworkCookieJar(QObject *parent) :
	QNetworkCookieJar(parent) {
}

ZLQtNetworkCookieJar::~ZLQtNetworkCookieJar() {
	save();
}

void ZLQtNetworkCookieJar::setFilePath(const QString &filePath) {
	myFilePath = filePath;
	QFile file(myFilePath);
	QList<QNetworkCookie> cookies;
	if (file.open(QFile::ReadOnly))
		cookies = QNetworkCookie::parseCookies(file.readAll());
	setAllCookies(cookies);
}

bool ZLQtNetworkCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url) {
	bool result = QNetworkCookieJar::setCookiesFromUrl(cookieList, url);
	save();
	return result;
}

void ZLQtNetworkCookieJar::save() {
	if (myFilePath.isEmpty())
		return;
	QFile file(myFilePath);
	QDir dir = QFileInfo(myFilePath).absoluteDir();
	if (!dir.exists()) {
		dir.mkpath(dir.absolutePath());
	}
	if (!file.open(QFile::WriteOnly)) {
		ZLLogger::Instance().println("ZLQtNetworkCookieJar", myFilePath.toStdString() + " can't be open for writing");
		return;
	}
	bool first = true;
	foreach (const QNetworkCookie &cookie, allCookies()) {
		if (first) {
			first = false;
		} else {
			file.write("\n", 1);
		}
		file.write(cookie.toRawForm(QNetworkCookie::Full));
	}
}

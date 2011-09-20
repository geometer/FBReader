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
#include <QtCore/QDir>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QAuthenticator>
//#include "ZLQtPostDevice.h"

Q_GLOBAL_STATIC(QWeakPointer<QNetworkDiskCache>, globalCache)
Q_GLOBAL_STATIC(QMutex, globalCacheMutex)

static QString fixPath(const QString &path) {
	if (path.startsWith('~'))
		return QDir::homePath() + path.mid(1);
	return path;
}

ZLQtNetworkManager::ZLQtNetworkManager() {
	myCache = new ZLQtNetworkCache(&myManager);
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
	QMutexLocker locker(globalCacheMutex());
	myCookieJar->setFilePath(QString::fromStdString(CookiesPath()));
	Q_ASSERT(!globalCache()->isNull());
	QDir cacheDirectory = fixPath(QString::fromStdString(ZLNetworkManager::CacheDirectory()))
	        + QLatin1String("/qt4");
	if (!cacheDirectory.exists())
		cacheDirectory.mkpath(cacheDirectory.absolutePath());
	(*globalCache()).toStrongRef()->setCacheDirectory(cacheDirectory.absolutePath());
}

QNetworkCookieJar *ZLQtNetworkManager::cookieJar() const {
	return myCookieJar;
}

std::string ZLQtNetworkManager::perform(const ZLExecutionData::Vector &dataList) const {
	if (useProxy()) {
		QString proxyHost = QString::fromStdString(ZLNetworkManager::proxyHost());
		QNetworkProxy proxy(QNetworkProxy::HttpProxy, proxyHost, atoi(proxyPort().c_str()));
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
			if (!request.hasListener()) {
				std::string error = request.errorMessage();
				if (error.empty()) {
					const ZLResource &errorResource = ZLResource::resource("dialog")["networkError"];
					error = ZLStringUtil::printf(errorResource["somethingWrongMessage"].value(),
												 networkRequest.url().host().toStdString());
				}
				errors << QString::fromStdString(error);
			}
			continue;
		}
		qDebug("Do request to %s", qPrintable(networkRequest.url().toString()));
		
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
		ZLQtNetworkReplyScope scope = { &request, &replies, &errors, &eventLoop };
		if (!request.hasListener()) {
			replies << reply;
		} else {
			scope.replies = 0;
			scope.errors = 0;
			scope.eventLoop = 0;
		}
		reply->setProperty("scope", qVariantFromValue(scope));
		reply->setProperty("executionData", qVariantFromValue(data));
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
	Q_ASSERT(reply);
	if (!checkReply(reply))
		return;
	ZLQtNetworkReplyScope scope = reply->property("scope").value<ZLQtNetworkReplyScope>();
	QByteArray data = reply->readAll();
	if (!data.isEmpty())
		scope.request->handleContent(data.data(), data.size());
}

void ZLQtNetworkManager::onFinished(QNetworkReply *reply) {
	if (!checkReply(reply))
		return;
	qDebug("Finished request with code %d to %s",
	       reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(),
	       qPrintable(reply->url().toString()));
	reply->deleteLater();
	ZLQtNetworkReplyScope scope = reply->property("scope").value<ZLQtNetworkReplyScope>();
	Q_ASSERT(scope.request);
	if (scope.request->hasListener()) {
		scope.request->doAfter(reply->error() == QNetworkReply::NoError
		                       ? std::string()
		                       : reply->errorString().toStdString());
		return;
	}

	Q_ASSERT(scope.replies->removeOne(reply));
	if (scope.eventLoop && scope.replies->isEmpty())
		scope.eventLoop->quit();
	if (reply->error() != QNetworkReply::NoError) {
		scope.request->doAfter(reply->errorString().toStdString());
		scope.errors->append(reply->errorString());
	} else {
		QByteArray data = reply->readAll();
		if (!data.isEmpty())
			scope.request->handleContent(data.data(), data.size());
		if (!scope.request->doAfter(std::string()))
			scope.errors->append(QString::fromStdString(scope.request->errorMessage()));
	}
}

bool ZLQtNetworkManager::checkReply(QNetworkReply *reply) {
	if (reply->property("headerHandled").toBool())
		return true;
	if (reply->property("redirected").toBool())
		return false;
	ZLQtNetworkReplyScope scope = reply->property("scope").value<ZLQtNetworkReplyScope>();
	QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	qDebug() << "request to" << reply->url() << "was redirected to" << redirect;
	qDebug() << "request's http code is" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()
			 << "and it's redirectionSupported =" << scope.request->isRedirectionSupported();
	if (redirect.isValid() && scope.request->isRedirectionSupported()) {
		reply->deleteLater();
		Q_ASSERT(scope.request->hasListener() || scope.replies->removeOne(reply));
		reply->setProperty("redirected", true);
		QVariant executionData = reply->property("executionData");
		QNetworkRequest request = reply->request();
		request.setUrl(reply->url().resolved(redirect));
		QObject::disconnect(reply, 0, this, 0);
		reply = myManager.get(request);
		Q_ASSERT(reply);
		if (!scope.request->hasListener())
			scope.replies->append(reply);
		QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(onReplyReadyRead()));
		reply->setProperty("scope", qVariantFromValue(scope));
		reply->setProperty("executionData", executionData);
		return false;
	}
	
	// We should fool the request about the received headers
	reply->setProperty("headerHandled", true);
	QByteArray data = "HTTP/1.1 ";
	data += reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toByteArray();
	data += " ";
	data += reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();
	scope.request->handleHeader(data.data(), data.size());
	foreach (const QNetworkReply::RawHeaderPair &pair, reply->rawHeaderPairs()) {
		data  = pair.first;
		data += ": ";
		data += pair.second;
		scope.request->handleHeader(data.data(), data.size());
	}
	return true;
}

ZLQtNetworkCache::ZLQtNetworkCache(QObject *parent) : QAbstractNetworkCache(parent) {
	QMutexLocker locker(globalCacheMutex());
	QWeakPointer<QNetworkDiskCache> &diskCache = *globalCache();
	if (diskCache.isNull()) {
		myCache = QSharedPointer<QNetworkDiskCache>::create();
		diskCache = myCache.toWeakRef();
	} else {
		myCache = diskCache.toStrongRef();
	}
}

ZLQtNetworkCache::~ZLQtNetworkCache() {
	QMutexLocker locker(globalCacheMutex());
	myCache.clear();
}

QNetworkCacheMetaData ZLQtNetworkCache::metaData(const QUrl &url) {
	QMutexLocker locker(globalCacheMutex());
	return myCache->metaData(url);
}

void ZLQtNetworkCache::updateMetaData(const QNetworkCacheMetaData &metaData) {
	QMutexLocker locker(globalCacheMutex());
	myCache->updateMetaData(metaData);
}

QIODevice *ZLQtNetworkCache::data(const QUrl &url) {
	QMutexLocker locker(globalCacheMutex());
	return myCache->data(url);
}

bool ZLQtNetworkCache::remove(const QUrl &url) {
	QMutexLocker locker(globalCacheMutex());
	return myCache->remove(url);
}

qint64 ZLQtNetworkCache::cacheSize() const {
	QMutexLocker locker(globalCacheMutex());
	return myCache->cacheSize();
}

QIODevice *ZLQtNetworkCache::prepare(const QNetworkCacheMetaData &metaData) {
	QMutexLocker locker(globalCacheMutex());
	return myCache->prepare(metaData);
}

void ZLQtNetworkCache::insert(QIODevice *device) {
	QMutexLocker locker(globalCacheMutex());
	myCache->insert(device);
}

void ZLQtNetworkCache::clear() {
	QMutexLocker locker(globalCacheMutex());
	myCache.clear();
}
	
ZLQtNetworkCookieJar::ZLQtNetworkCookieJar(QObject *parent) 
    : QNetworkCookieJar(parent) {
}

ZLQtNetworkCookieJar::~ZLQtNetworkCookieJar() {
	save();
}

void ZLQtNetworkCookieJar::setFilePath(const QString &filePath) {
	myFilePath = fixPath(filePath) + QLatin1String("/cache.dat");
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
	if (!dir.exists())
		dir.mkpath(dir.absolutePath());
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

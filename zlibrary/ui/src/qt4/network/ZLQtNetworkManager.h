#ifndef __ZLQTNETWORKMANAGER_H__
#define __ZLQTNETWORKMANAGER_H__

#include <ZLNetworkManager.h>
#include <ZLNetworkRequest.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkDiskCache>
#include <QtNetwork/QNetworkCookieJar>

class ZLQtNetworkCookieJar;
class QNetworkReply;
class QEventLoop;

struct ZLQtNetworkReplyScope {
	ZLNetworkRequest *request;
	bool *headerHandled;
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

	std::string perform(const ZLExecutionData::Vector &dataList) const;
	
protected Q_SLOTS:
    void onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void onReplyReadyRead();
    void onFinished(QNetworkReply *reply);

private:
	QNetworkAccessManager myManager;
	QNetworkDiskCache *myCache;
	ZLQtNetworkCookieJar *myCookieJar;
};

class ZLQtNetworkCookieJar : public QNetworkCookieJar {
	Q_OBJECT
public:
    ZLQtNetworkCookieJar(const QString &filePath, QObject *parent = 0);
    ~ZLQtNetworkCookieJar();
	
    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);
    void save();
	
private:
	QString myFilePath;
};

Q_DECLARE_METATYPE(ZLQtNetworkReplyScope)

#endif /* __ZLQTNETWORKMANAGER_H__ */

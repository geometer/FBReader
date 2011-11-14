#ifndef __IMAGEPROVIDER_H__
#define __IMAGEPROVIDER_H__

#include <QtCore/QObject>
#include <QtCore/QRunnable>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QUrl>
#include <QtCore/QSet>
#include <QtGui/QPixmap>

#include <ZLTreeTitledNode.h>

#include <ZLImageManager.h>

Q_DECLARE_METATYPE(const ZLTreeTitledNode*)
Q_DECLARE_METATYPE(shared_ptr<ZLImage>)

class ImageRunnable : public QRunnable {
public:
    ImageRunnable(const ZLTreeTitledNode *node, QObject *requester);
    void run();

private:
    QWeakPointer<QObject> myRequester;
    const ZLTreeTitledNode *myNode;
};

class ImageProvider : public QObject {
    Q_OBJECT

public:
    ImageProvider(QObject* parent = 0);

public:
    QPixmap getImageForNode(const ZLTreeTitledNode *titledNode);

signals:
    void cacheUpdated() const;

private slots:
    void onRequestFinished(QNetworkReply*);
    void handleImageResult(const ZLTreeTitledNode *node, shared_ptr<ZLImage> image);

private:
    QPixmap getZLImage(shared_ptr<ZLImage> image) const;
    QPixmap getFSImage(QUrl imageUrl) const;
    void getNetworkImage(QUrl url) const;

private:
    void updateCache(QString cacheUrl, QPixmap pixmap);

private:
    static QUrl generateUrl(const ZLTreeTitledNode* node);

private:
        QMap<QString,QPixmap> myCache;
        QSet<const ZLTreeTitledNode*> myProcessedNodes;
        QPixmap myEmptyPixmap;
        mutable QNetworkAccessManager myManager;
        //TODO cache should not be deleted after closing net library dialog (??)

};

#endif /* __IMAGEPROVIDER_H__ */


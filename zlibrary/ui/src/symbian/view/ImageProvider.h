#ifndef __IMAGEPROVIDER_H__
#define __IMAGEPROVIDER_H__

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QUrl>
#include <QtGui/QPixmap>

#include <ZLTreeTitledNode.h>

#include <ZLImageManager.h>

class PixmapTransfomer : public QThread {
    Q_OBJECT

public:
   explicit PixmapTransfomer(QObject *parent = 0);

public slots:
    void needImage(const ZLTreeTitledNode* node) const;

signals:
    void imageIsReady(const ZLTreeTitledNode* node, QPixmap pixmap) const;

private slots:
    void onRequestFinished(QNetworkReply*);

private:
    QPixmap getZLImage(shared_ptr<ZLImage> image) const;
    QPixmap getFSImage(QUrl imageUrl) const;
    void getNetworkImage(const ZLTreeTitledNode* node, QUrl url) const;

private:
    mutable QNetworkAccessManager myManager;
    mutable QMap<QUrl, const ZLTreeTitledNode*> myNetworkImageCache;
    mutable QMap<QUrl,QPixmap> myCache;

};


class ImageProvider : public QObject {
    Q_OBJECT

public:
    ImageProvider();

public:
    QPixmap getImageForNode(const ZLTreeTitledNode *titledNode) const;
//public:
//    QPixmap getUrlImage(QUrl url) const;
//    QPixmap getFromZLImage(shared_ptr<ZLImage> image) const;
//    QPixmap getFromZLImage(QString cacheId, shared_ptr<ZLImage> image) const;

signals:
    void cacheUpdated() const;
    void needImage(const ZLTreeTitledNode* node) const;

public slots:
    void imageIsReady(const ZLTreeTitledNode* node, QPixmap pixmap) const;

private:
        const QPixmap& downloadImage(QUrl url) const;

private:
        PixmapTransfomer* myTransformer;
        mutable QMap<const ZLTreeTitledNode*,QPixmap> myCache;
        QPixmap myEmptyPixmap;
        //TODO cache should not be deleted after closing net library dialog (??)

};

#endif /* __IMAGEPROVIDER_H__ */


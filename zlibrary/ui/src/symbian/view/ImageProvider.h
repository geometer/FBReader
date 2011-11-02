#ifndef __IMAGEPROVIDER_H__
#define __IMAGEPROVIDER_H__

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QUrl>
#include <QtGui/QPixmap>

#include <ZLImageManager.h>

class PixmapTransfomer : public QThread {
    Q_OBJECT

public:
   explicit PixmapTransfomer(QObject *parent = 0);

public slots:
    void ZLImageToQPixmap(QString cacheId, shared_ptr<ZLImage> image,QSize size);
    void fileUrlToQPixmap(QUrl url, QSize size);

signals:
    void ZLImageIsReady(QString cacheId, QPixmap image);
    void fileUrlIsReady(QUrl url, QPixmap image);
};


class ImageProvider : public QObject {
    Q_OBJECT

public:
    static ImageProvider& Instance();
    //TODO implement deleteInstance

public:
    QPixmap getUrlImage(QUrl url) const;
    QPixmap getFromZLImage(shared_ptr<ZLImage> image) const;
    QPixmap getFromZLImage(QString cacheId, shared_ptr<ZLImage> image) const;

signals:
//    void imageTransfromed();
//    void imageLoaded();
    void cacheUpdated();
    void ZLImageToQPixmap(QString cacheId, shared_ptr<ZLImage> image,QSize size) const;
    void fileUrlToQPixmap(QUrl url, QSize size) const;

private:
    ImageProvider();

private slots:
        void onRequestFinished(QNetworkReply*);
        void onZLImageIsReady(QString cacheId, QPixmap pixmap);
        void onFileUrlIsReady(QUrl url, QPixmap pixmap);

private:
        const QPixmap& downloadImage(QUrl url) const;

private:
        mutable QNetworkAccessManager myManager;
        //TODO should it be link?
        PixmapTransfomer myTransformer;
        mutable QMap<QString,QPixmap> myCache;
        //TODO one cache for everything
        mutable QMap<QString,QPixmap> myZLImageCache;
        QPixmap myEmptyPixmap;
        //TODO cache should not be deleted after closing net library dialog (??)


private:
    static ImageProvider* myInstance;


};

#endif /* __IMAGEPROVIDER_H__ */


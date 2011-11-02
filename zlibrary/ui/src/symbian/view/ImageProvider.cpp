#include <QtCore/QDebug>

#include "ImageProvider.h"
#include "ImageUtils.h"
#include "../menu/DrillDownMenu.h"

ImageProvider* ImageProvider::myInstance = 0;

ImageProvider& ImageProvider::Instance() {
    if (myInstance == 0) {
        myInstance = new ImageProvider;
    }
    return *myInstance;
}

ImageProvider::ImageProvider() {
    connect(&myManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onRequestFinished(QNetworkReply*)));
    myTransformer.start();
    connect(&myTransformer, SIGNAL(ZLImageIsReady(QString,QPixmap)), this, SLOT(onZLImageIsReady(QString,QPixmap)));
    connect(this, SIGNAL(ZLImageToQPixmap(QString,shared_ptr<ZLImage>,QSize)), &myTransformer, SLOT(ZLImageToQPixmap(QString,shared_ptr<ZLImage>,QSize)));
    connect(&myTransformer, SIGNAL(fileUrlIsReady(QUrl,QPixmap)), this, SLOT(onFileUrlIsReady(QUrl,QPixmap)));
    connect(this, SIGNAL(fileUrlToQPixmap(QUrl,QSize)), &myTransformer, SLOT(fileUrlToQPixmap(QUrl,QSize)));
    myEmptyPixmap = QPixmap(MenuItemParameters::getImageSize());
    myEmptyPixmap.fill(Qt::transparent);
}

QPixmap ImageProvider::getFromZLImage(shared_ptr<ZLImage> image) const {
    qDebug() << Q_FUNC_INFO;
    //implement caching
    //qDebug() << Q_FUNC_INFO << MenuItemParameters::getImageSize();
    return ImageUtils::ZLImageToQPixmap(image, 0, MenuItemParameters::getImageSize() );
}

QPixmap ImageProvider::getUrlImage(QUrl url) const {
    //qDebug() << Q_FUNC_INFO << url;
    //TODO implement retransform image if MenuItemParamters has been changed
    if (myCache.contains(url.toString())) {
        return myCache[url.toString()];
    }
    if (url.scheme() == QLatin1String("file")) {
        qDebug() << url << url.toLocalFile();
        emit fileUrlToQPixmap(url, MenuItemParameters::getImageSize());
        return myEmptyPixmap;
    } else {
        return downloadImage(url);
    }
}

void ImageProvider::onRequestFinished(QNetworkReply* reply) {
    //qDebug() << Q_FUNC_INFO<< reply << reply->url();
    QPixmap pixmap;
    pixmap.loadFromData(reply->readAll());
    QSize imageSize =  MenuItemParameters::getImageSize();
    if (!pixmap.isNull()) {
        pixmap = ImageUtils::scaleAndCenterPixmap(pixmap, imageSize, true);
    }
    myCache[reply->url().toString()] = pixmap.isNull() ? myEmptyPixmap : pixmap;
    emit cacheUpdated();
}

void ImageProvider::onZLImageIsReady(QString cacheId, QPixmap pixmap) {
    qDebug() << Q_FUNC_INFO << cacheId;
    myZLImageCache[cacheId] = pixmap.isNull() ? myEmptyPixmap : pixmap;
    emit cacheUpdated();
}

void ImageProvider::onFileUrlIsReady(QUrl url, QPixmap pixmap) {
    qDebug() << Q_FUNC_INFO << url;
    myCache[url.toString()] = pixmap.isNull() ? myEmptyPixmap : pixmap;
    emit cacheUpdated();
}

QPixmap ImageProvider::getFromZLImage(QString cacheId, shared_ptr<ZLImage> image) const {
    qDebug() << Q_FUNC_INFO << cacheId;
    if (myZLImageCache.contains(cacheId)) {
        return myZLImageCache[cacheId];
    }
    emit ZLImageToQPixmap(cacheId, image, MenuItemParameters::getImageSize());
    return myEmptyPixmap;
}

 const QPixmap& ImageProvider::downloadImage(QUrl url) const {
     if (!url.isValid()) {
         return myEmptyPixmap;
     }
     QNetworkRequest request(url);
     myManager.get(request);
     return myEmptyPixmap;
 }

 PixmapTransfomer::PixmapTransfomer(QObject *parent) : QThread(parent) {

 }

void PixmapTransfomer::ZLImageToQPixmap(QString cacheId, shared_ptr<ZLImage> image,QSize size) {
    qDebug() << Q_FUNC_INFO << cacheId << size;
    QPixmap pixmap = ImageUtils::ZLImageToQPixmap(image, 0, size);
    emit ZLImageIsReady(cacheId, pixmap);
}

void PixmapTransfomer::fileUrlToQPixmap(QUrl url, QSize size) {
    qDebug() << Q_FUNC_INFO << url << size;
    QPixmap pixmap = ImageUtils::fileUrlToQPixmap(url, 0, size);
    emit fileUrlIsReady(url, pixmap);
}

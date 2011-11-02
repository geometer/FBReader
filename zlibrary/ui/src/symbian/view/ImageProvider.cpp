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
        QPixmap pixmap = ImageUtils::fileUrlToQPixmap(url, 0, MenuItemParameters::getImageSize());
        myCache[url.toString()] = pixmap.isNull() ? myEmptyPixmap : pixmap;
        return pixmap;
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

QPixmap ImageProvider::getFromZLImage(QString cacheId, shared_ptr<ZLImage> image) const {
    qDebug() << Q_FUNC_INFO << cacheId;
    if (myZLImageCache.contains(cacheId)) {
        return myZLImageCache[cacheId];
    }
    //TODO implement rescaling in other thread
    QPixmap pixmap = getFromZLImage(image);
    myZLImageCache[cacheId] = pixmap.isNull() ? myEmptyPixmap : pixmap;
    return pixmap;
}

 const QPixmap& ImageProvider::downloadImage(QUrl url) const {
     if (!url.isValid()) {
         return myEmptyPixmap;
     }
     QNetworkRequest request(url);
     myManager.get(request);
     return myEmptyPixmap;
 }

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
    //implement caching
    //qDebug() << Q_FUNC_INFO << MenuItemParameters::getImageSize();
    return ImageUtils::ZLImageToQPixmap(image, 0, MenuItemParameters::getImageSize() );
}

QPixmap ImageProvider::getUrlImage(QUrl url) const {
    //qDebug() << Q_FUNC_INFO << url;
    //TODO implement retransfrom image if MenuItemParamters has been changed
    if (url.scheme() == QLatin1String("file")) {
        qDebug() << url << url.toLocalFile();
        //TODO add cache here
        return ImageUtils::fileUrlToQPixmap(url, 0, MenuItemParameters::getImageSize());
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
    emit updated();
}

 const QPixmap& ImageProvider::downloadImage(QUrl url) const {
     if (!url.isValid()) {
         return myEmptyPixmap;
     }
     if (myCache.contains(url.toString())) {
         return myCache.value(url.toString());
     }
     QNetworkRequest request(url);
     myManager.get(request);
     //qDebug() << myEmptyPixmap.size();
     return myEmptyPixmap;
 }

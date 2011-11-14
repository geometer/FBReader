#include <QtCore/QDebug>
#include <QtGui/QPainter>

#include "../image/ZLQtImageManager.h"

#include "ImageUtils.h"

QPixmap ImageUtils::ZLImageToQPixmap(shared_ptr<ZLImage> image, QSize *size, const QSize &requestedSize) {
    if (image.isNull()) {
       return QPixmap();
    }
    shared_ptr<ZLImageData> imageData = ZLImageManager::Instance().imageData(*image);
    if (imageData.isNull()) {
        return QPixmap();
    }
    const QImage *qImage = static_cast<ZLQtImageData&>(*imageData).image();
    if (qImage) {
            if (size)
                *size = qImage->size();
            QImage finalImage = *qImage;
            return centerPixmap(scalePixmap(QPixmap::fromImage(finalImage), requestedSize, true), requestedSize);
    }
    return QPixmap();
}

QPixmap ImageUtils::fileUrlToQPixmap(QUrl url, QSize *size, const QSize &requestedSize) {
    QPixmap pixmap(url.toLocalFile());
    qDebug() << "fileUrlToQPixmap! asking for file" << url << url.toLocalFile() << "is pixmap null?" << pixmap.isNull();
    if (size) {
        *size = pixmap.size();
    }
//    if (pixmap.isNull()) {
//        qDebug() << "ImageUtils:: final image is null by undefined reasons";
//        return QPixmap();
//    }
    return scaleAndCenterPixmap(pixmap, requestedSize, false);
}

QPixmap ImageUtils::scalePixmap(const QPixmap& pixmap, const QSize& requestedSize, bool scaleIfLess) {
    if (pixmap.isNull() || !requestedSize.isValid()) {
        qDebug() << "ImageUtils: scaling -- pixmap is null or req size is not valid";
        return pixmap;
    }
    if (!scaleIfLess && requestedSize.width() > pixmap.width() && requestedSize.height() > pixmap.height()) {
        //qDebug() << "notScaleifLess!" << requestedSize << pixmap.size();
        return pixmap;
    }
    //TODO for small item we should use Qt::FastTransformation
    return pixmap.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QPixmap ImageUtils::centerPixmap(const QPixmap& pixmap, const QSize& requestedSize) {
    if (pixmap.isNull() || !requestedSize.isValid()) {
        qDebug() << "ImageUtils: centring -- pixmap is null or req size is not valid";
        return pixmap;
    }
    QPixmap centeredPixmap(requestedSize);
    centeredPixmap.fill(Qt::transparent);
    QPainter painter(&centeredPixmap);
    qreal diffX = (requestedSize.width() - pixmap.width()) / 2;
    qreal diffY = (requestedSize.height() - pixmap.height()) / 2;
    painter.drawPixmap(QPointF(diffX, diffY), pixmap);
    return centeredPixmap;
}

QPixmap ImageUtils::scaleAndCenterPixmap(const QPixmap& image, const QSize& requestedSize, bool scaleIfLess) {
    //this is a shortcut
    return centerPixmap(scalePixmap(image, requestedSize, scaleIfLess), requestedSize);
}

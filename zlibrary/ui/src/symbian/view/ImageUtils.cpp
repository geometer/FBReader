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
    QImage finalImage = pixmap.toImage();
    if (size) {
        *size = finalImage.size();
    }
    if (finalImage.isNull()) {
        return QPixmap();
    }
    return scalePixmap(QPixmap::fromImage(finalImage), requestedSize, false);
}

QPixmap ImageUtils::scalePixmap(const QPixmap& pixmap, const QSize& requestedSize, bool scaleIfLess) {
    if (!requestedSize.isValid()) {
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
    if (!requestedSize.isValid()) {
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

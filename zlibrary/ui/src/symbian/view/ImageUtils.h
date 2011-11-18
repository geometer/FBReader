#ifndef __IMAGEUTILS_H__
#define __IMAGEUTILS_H__

#include <QtCore/QUrl>
#include <QtGui/QPixmap>

#include <ZLImage.h>

class ImageUtils {
public:
    static QPixmap ZLImageToQPixmap(shared_ptr<ZLImage> image, QSize *size, const QSize &requestedSize, Qt::TransformationMode mode=Qt::FastTransformation);
    static QPixmap fileUrlToQPixmap(QUrl url, QSize *size, const QSize &requestedSize, Qt::TransformationMode mode=Qt::FastTransformation);
    static QPixmap scalePixmap(const QPixmap& image, const QSize& requestedSize, bool scaleIfLess, Qt::TransformationMode mode=Qt::FastTransformation);
    static QPixmap centerPixmap(const QPixmap& image, const QSize& requestedSize);
    static QPixmap scaleAndCenterPixmap(const QPixmap& image, const QSize& requestedSize, bool scaleIfLess, Qt::TransformationMode mode=Qt::FastTransformation);
};

#endif /* __IMAGEUTILS_H__ */

#ifndef __IMAGEUTILS_H__
#define __IMAGEUTILS_H__

#include <QtCore/QUrl>
#include <QtGui/QPixmap>

#include <ZLImage.h>

class ImageUtils {
public:
    static QPixmap ZLImageToQPixmap(shared_ptr<ZLImage> image, QSize *size, const QSize &requestedSize);
    static QPixmap urlToQPixmap(QUrl url, QSize *size, const QSize &requestedSize);
    static QPixmap scalePixmap(const QPixmap& image, const QSize& requestedSize, bool scaleIfLess);
    static QPixmap centerPixmap(const QPixmap& image, const QSize& requestedSize);
    static QPixmap scaleAndCenterPixmap(const QPixmap& image, const QSize& requestedSize, bool scaleIfLess);
};

#endif /* __IMAGEUTILS_H__ */

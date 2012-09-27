#ifndef __ZLQTIMAGEUTILS_H__
#define __ZLQTIMAGEUTILS_H__

#include <QtCore/QUrl>
#include <QtGui/QPixmap>

#include <ZLImage.h>

class ZLQtImageUtils {
public:
	static QPixmap ZLImageToQPixmap(shared_ptr<const ZLImage> image);
    static QPixmap fileUrlToQPixmap(QUrl url, QSize *size, const QSize &requestedSize, Qt::TransformationMode mode=Qt::FastTransformation);
    static QPixmap scalePixmap(const QPixmap& image, const QSize& requestedSize, bool scaleIfLess, Qt::TransformationMode mode=Qt::FastTransformation);
    static QPixmap centerPixmap(const QPixmap& image, const QSize& requestedSize);
    static QPixmap scaleAndCenterPixmap(const QPixmap& image, const QSize& requestedSize, bool scaleIfLess, Qt::TransformationMode mode=Qt::FastTransformation);
	static QPixmap ZLImageToQPixmapWithSize(shared_ptr<const ZLImage> image, const QSize &requestedSize, Qt::TransformationMode mode=Qt::FastTransformation);
};

#endif /* __ZLQTIMAGEUTILS_H__ */

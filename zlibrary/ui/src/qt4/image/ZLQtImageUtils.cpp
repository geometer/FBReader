/*
 * Copyright (C) 2012-2013 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <QtCore/QBuffer>
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QColor>

#include "../image/ZLQtImageManager.h"

#include "ZLQtImageUtils.h"

class ZLQtPixmapImage : public ZLSingleImage {

public:
	ZLQtPixmapImage(QPixmap pixmap) : ZLSingleImage(ZLMimeType::EMPTY) {
		QImage image = pixmap.toImage();
		QByteArray rawData;
		QBuffer buffer(&rawData);
		buffer.open(QIODevice::WriteOnly);
		image.save(&buffer, "PNG");
		myStringData = new std::string(rawData.constData(), rawData.size());
	}

	const shared_ptr<std::string> stringData() const {
		return myStringData;
	}

private:
	shared_ptr<std::string> myStringData;
};

shared_ptr<const ZLImage> ZLQtImageUtils::QPixmapToZLImage(QPixmap pixmap) {
	return new ZLQtPixmapImage(pixmap);
}

QPixmap ZLQtImageUtils::ZLImageToQPixmapWithSize(shared_ptr<const ZLImage> image, const QSize &requestedSize, bool scaleIfLess, Qt::TransformationMode mode) {
	QPixmap pixmap = ZLImageToQPixmap(image);
	if (!pixmap.isNull()) {
		pixmap = centerPixmap(scalePixmap(pixmap, requestedSize, scaleIfLess, mode), requestedSize);
	}
	return pixmap;
}

QPixmap ZLQtImageUtils::addBorder(const QPixmap &image, QColor color, int borderSize) {
	   QPixmap pixmap(image.width() + borderSize * 2, image.height() + borderSize * 2);
	   pixmap.fill(color);
	   QPainter painter(&pixmap);
	   painter.drawPixmap(borderSize, borderSize ,image);
	   return pixmap;
}

QPixmap ZLQtImageUtils::addOppositeBorder(const QPixmap &image, int borderSize) {
	QImage img = image.toImage();
	QPoint brPoint = img.rect().bottomRight();
	QColor leftTop(img.pixel(0,0));
	QColor rightTop(img.pixel(brPoint.x(), 0));
	QColor leftBottom(img.pixel(0,brPoint.y()));
	QColor rightBottom(img.pixel(brPoint));
	QColor mixedColor(
		(leftTop.red() + rightTop.red() + leftBottom.red() + rightBottom.red()) / 4,
		(leftTop.green() + rightTop.green() + leftBottom.green() + rightBottom.green()) / 4,
		(leftTop.blue() + rightTop.blue() + leftBottom.blue() + rightBottom.blue()) / 4,
		(leftTop.alpha() + rightTop.alpha() + leftBottom.alpha() + rightBottom.alpha()) / 4
	);
	QColor oppositeColor(255 - mixedColor.red(), 255 - mixedColor.green(), 255 - mixedColor.blue(), mixedColor.alpha());
	return addBorder(QPixmap::fromImage(img), oppositeColor, borderSize);
}

QPixmap ZLQtImageUtils::ZLImageToQPixmap(shared_ptr<const ZLImage> image) {
	if (image.isNull()) {
	   return QPixmap();
	}
	shared_ptr<ZLImageData> imageData = ZLImageManager::Instance().imageData(*image);
	if (imageData.isNull()) {
		return QPixmap();
	}
	const QImage *qImage = static_cast<ZLQtImageData&>(*imageData).image();
	if (!qImage) {
		return QPixmap();
	}
	return QPixmap::fromImage(*qImage);
}

QPixmap ZLQtImageUtils::fileUrlToQPixmap(QUrl url, QSize *size, const QSize &requestedSize,  Qt::TransformationMode mode) {
    QPixmap pixmap(url.toLocalFile());
    if (size) {
        *size = pixmap.size();
    }
    return scaleAndCenterPixmap(pixmap, requestedSize, false, mode);
}

QPixmap ZLQtImageUtils::scalePixmap(const QPixmap& pixmap, const QSize& requestedSize, bool scaleIfLess, Qt::TransformationMode mode) {
    if (pixmap.isNull() || !requestedSize.isValid()) {
        return pixmap;
    }
    if (!scaleIfLess && requestedSize.width() > pixmap.width() && requestedSize.height() > pixmap.height()) {
        return pixmap;
    }
    //TODO for small item we should use Qt::FastTransformation
    return pixmap.scaled(requestedSize, Qt::KeepAspectRatio, mode);
}

QPixmap ZLQtImageUtils::centerPixmap(const QPixmap& pixmap, const QSize& requestedSize) {
    if (pixmap.isNull() || !requestedSize.isValid()) {
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

QPixmap ZLQtImageUtils::scaleAndCenterPixmap(const QPixmap& image, const QSize& requestedSize, bool scaleIfLess,  Qt::TransformationMode mode) {
    //this is a shortcut
    return centerPixmap(scalePixmap(image, requestedSize, scaleIfLess, mode), requestedSize);
}

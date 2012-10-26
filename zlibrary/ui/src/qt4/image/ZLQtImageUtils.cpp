/*
 * Copyright (C) 2012 Geometer Plus <contact@geometerplus.com>
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

#include <QtCore/QDebug>
#include <QtGui/QPainter>

#include "../image/ZLQtImageManager.h"

#include "ZLQtImageUtils.h"

QPixmap ZLQtImageUtils::ZLImageToQPixmapWithSize(shared_ptr<const ZLImage> image, const QSize &requestedSize, Qt::TransformationMode mode) {
	QPixmap pixmap = ZLImageToQPixmap(image);
	if (!pixmap.isNull()) {
		pixmap = centerPixmap(scalePixmap(pixmap, requestedSize, true, mode), requestedSize);
	}
	return pixmap;
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

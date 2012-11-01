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
	static QPixmap ZLImageToQPixmapWithSize(shared_ptr<const ZLImage> image, const QSize &requestedSize, bool scaleIfLess, Qt::TransformationMode mode=Qt::FastTransformation);
};

#endif /* __ZLQTIMAGEUTILS_H__ */

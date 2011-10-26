/*
 * Copyright (C) 2004-2011 Geometer Plus <contact@geometerplus.com>
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

#include "ZLQmlTreeImageProvider.h"
#include "../dialogs/ZLQmlTree.h"
#include <ZLTreeTitledNode.h>
#include "../image/ZLQtImageManager.h"
#include <QtCore/QDebug>

ZLQmlTreeImageProvider::ZLQmlTreeImageProvider()
    : QDeclarativeImageProvider(Pixmap)
{
}

QPixmap ZLQmlTreeImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) {
	bool ok1 = true, ok2 = true;
	qptrdiff treeId = static_cast<qptrdiff>(id.section('$', 0, 0).toLongLong(&ok1, 16));
	qptrdiff nodeId = static_cast<qptrdiff>(id.section('$', 1, 1).toLongLong(&ok2, 16));
	if (!ok1 || !ok2 || !treeId || !nodeId
	        || !ZLQmlTreeDialog::isAlive(reinterpret_cast<ZLQmlTreeDialog*>(treeId))) {
		return QPixmap();
	}
	ZLTreeNode *node = reinterpret_cast<ZLTreeNode*>(nodeId);
	if (ZLTreeTitledNode *titledNode = zlobject_cast<ZLTreeTitledNode*>(node)) {
		shared_ptr<ZLImage> image = titledNode->image();
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
			if (requestedSize.isValid())
				finalImage = finalImage.scaled(requestedSize, Qt::KeepAspectRatio,
				                               Qt::SmoothTransformation);
			return QPixmap::fromImage(finalImage);
		}
	}
	return QPixmap();
}

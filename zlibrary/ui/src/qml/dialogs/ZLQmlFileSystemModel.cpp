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

#include "ZLQmlFileSystemModel.h"
#include <QtCore/QTimer>

Q_DECLARE_METATYPE(QModelIndex)

enum {
	ZLQmlDirectoryRole = Qt::UserRole + 100,
	ZLQmlRootRole,
	ZLQmlFileTypeRole
};

ZLQmlFileSystemModel::ZLQmlFileSystemModel(QObject *parent) :
    QFileSystemModel(parent) {
	QHash<int, QByteArray> names = roleNames();
	names[ZLQmlDirectoryRole] = "directory";
	names[ZLQmlFileTypeRole] = "subtitle";
	names.insertMulti(FileNameRole, "title");
	setRoleNames(names);
	setFilter(QDir::AllEntries | QDir::NoDot);
	connect(this, SIGNAL(layoutChanged()), SLOT(onLayoutChanged()));
	sort(0, Qt::AscendingOrder);
}

void ZLQmlFileSystemModel::setRootPath(const QString &path) {
	QFileSystemModel::setRootPath(path);
}

QVariant ZLQmlFileSystemModel::rootIndex() const {
	return qVariantFromValue(myRootIndex);
}

QVariant ZLQmlFileSystemModel::data(const QModelIndex &index, int role) const {
	switch (role) {
	case ZLQmlDirectoryRole: {
		QFileInfo info = fileInfo(index);
		return info.isDir();
	}
	case ZLQmlRootRole: {
		QFileInfo info = fileInfo(index);
		return info.isRoot();
	}
	case ZLQmlFileTypeRole:
		return type(index);
	default:
		return QFileSystemModel::data(index, role);
	}
}

void ZLQmlFileSystemModel::onLayoutChanged() {
	myRootIndex = index(rootPath());
	emit rootIndexChanged(rootIndex());
}

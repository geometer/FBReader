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

#ifndef ZLQMLFILESYSTEMMODEL_H
#define ZLQMLFILESYSTEMMODEL_H

#include <QtGui/QFileSystemModel>
#include <QtDeclarative/qdeclarative.h>

class ZLQmlFileSystemModel : public QFileSystemModel
{
    Q_OBJECT
	Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath)
	Q_PROPERTY(QVariant rootIndex READ rootIndex NOTIFY rootIndexChanged)
public:
    explicit ZLQmlFileSystemModel(QObject *parent = 0);
	
	Q_INVOKABLE void setRootPath(const QString &path);
	QVariant rootIndex() const;
	
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

signals:
	void rootIndexChanged(const QVariant &rootIndex);

private slots:
	void onLayoutChanged();
	
private:
	QModelIndex myRootIndex;
};

#endif // ZLQMLFILESYSTEMMODEL_H

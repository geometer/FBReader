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

#ifndef ZLQMLTREE_H
#define ZLQMLTREE_H

#include <QtCore/QAbstractItemModel>
#include <ZLTreeDialog.h>

class ZLQmlTreeDialog : public QAbstractItemModel, public ZLTreeDialog
{
    Q_OBJECT
public:
    ZLQmlTreeDialog();
	~ZLQmlTreeDialog();
	
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
	Q_INVOKABLE void fetchChildren(const QModelIndex &index);
	Q_INVOKABLE void activate(const QModelIndex &index);
	Q_INVOKABLE void finish();
	
	virtual void onNodeBeginInsert(ZLTreeNode *parent, size_t index);
	virtual void onNodeEndInsert();
	virtual void onNodeBeginRemove(ZLTreeNode *parent, size_t index);
	virtual void onNodeEndRemove();
	virtual void onNodeUpdated(ZLTreeNode *node);
	virtual void run();
	
Q_SIGNALS:
	void finished();
	
private:
	QModelIndex createIndex(ZLTreeNode *node) const;
	ZLTreeNode *treeNode(const QModelIndex &index) const;
};

#endif // ZLQMLTREE_H

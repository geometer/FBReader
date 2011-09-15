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

#include <QtCore/QEventLoop>
#include <QtCore/QCoreApplication>
#include "ZLQmlTree.h"
#include <ZLTreeActionNode.h>
#include <ZLTreePageNode.h>

enum {
	SubTitleRole = Qt::UserRole,
	ActivatableRole,
	PageRole
};

ZLQmlTreeDialog::ZLQmlTreeDialog()
{
	QHash<int, QByteArray> names = roleNames();
	names[Qt::DisplayRole] = "title";
	names[SubTitleRole] = "subtitle";
	names[Qt::DecorationRole] = "imageSource";
	names[ActivatableRole] = "activatable";
	names[PageRole] = "page";
	setRoleNames(names);
}

ZLQmlTreeDialog::~ZLQmlTreeDialog() {
}

QModelIndex ZLQmlTreeDialog::index(int row, int column, const QModelIndex &parent) const {
	Q_UNUSED(column);
	ZLTreeNode *node = treeNode(parent);
	if (row < 0 || uint(row) >= node->children().size())
		return QModelIndex();
	return createIndex(node->children().at(row));
}

QModelIndex ZLQmlTreeDialog::parent(const QModelIndex &child) const {
	ZLTreeNode *node = treeNode(child);
	if (node->parent())
		return createIndex(node->parent());
	else
		return QModelIndex();
}

int ZLQmlTreeDialog::rowCount(const QModelIndex &parent) const {
	ZLTreeNode *node = treeNode(parent);
	return node->children().size();
}

int ZLQmlTreeDialog::columnCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	return 0;
}

bool ZLQmlTreeDialog::hasChildren(const QModelIndex &parent) const {
	ZLTreeNode *node = treeNode(parent);
	return zlobject_cast<ZLTreeActionNode*>(node) != NULL;
}

QVariant ZLQmlTreeDialog::data(const QModelIndex &index, int role) const {
	ZLTreeNode *node = treeNode(index);
	if (!node)
		return QVariant();
	
	switch (role) {
	case Qt::DisplayRole:
		if (ZLTreeTitledNode *titledNode = zlobject_cast<ZLTreeTitledNode*>(node))
			return QString::fromStdString(titledNode->title());
		else
			return QString();
	case Qt::DecorationRole:
		if (ZLTreeTitledNode *titledNode = zlobject_cast<ZLTreeTitledNode*>(node))
			return QString::fromStdString(titledNode->imageUrl());
		else
			return QString();
	case SubTitleRole:
		if (ZLTreeTitledNode *titledNode = zlobject_cast<ZLTreeTitledNode*>(node))
			return QString::fromStdString(titledNode->subtitle());
		else
			return QString();
	case ActivatableRole:
		return zlobject_cast<ZLTreeActionNode*>(node) != NULL;
	case PageRole:
		return zlobject_cast<ZLTreePageNode*>(node) != NULL;
	default:
		return QVariant();
	}
}

void ZLQmlTreeDialog::fetchChildren(const QModelIndex &index) {
	ZLTreeNode *node = treeNode(index);
	node->requestChildren();
}

void ZLQmlTreeDialog::activate(const QModelIndex &index) {
	if (ZLTreeActionNode *node = zlobject_cast<ZLTreeActionNode*>(treeNode(index)))
		node->activate();
}

void ZLQmlTreeDialog::finish() {
	emit finished();
}

void ZLQmlTreeDialog::onNodeBeginInsert(ZLTreeNode *parent, size_t index) {
	beginInsertRows(createIndex(parent), index, index);
}

void ZLQmlTreeDialog::onNodeEndInsert() {
	endInsertRows();
}

void ZLQmlTreeDialog::onNodeBeginRemove(ZLTreeNode *parent, size_t index) {
	beginRemoveRows(createIndex(parent), index, index);
}

void ZLQmlTreeDialog::onNodeEndRemove() {
	endRemoveRows();
}

void ZLQmlTreeDialog::onNodeUpdated(ZLTreeNode *node) {
	QModelIndex index = createIndex(node);
	emit dataChanged(index, index);
}

void ZLQmlTreeDialog::run() {
    QEventLoop eventLoop;
	connect(this, SIGNAL(finished()), &eventLoop, SLOT(quit()), Qt::QueuedConnection);
    eventLoop.exec(QEventLoop::AllEvents);
	qApp->sendPostedEvents(0, QEvent::DeferredDelete);
}

QModelIndex ZLQmlTreeDialog::createIndex(ZLTreeNode *node) const {
	if (!node || !node->parent())
		return QModelIndex();
	return QAbstractItemModel::createIndex(node->childIndex(), 0, node);
}

ZLTreeNode *ZLQmlTreeDialog::treeNode(const QModelIndex &index) const {
	if (!index.isValid())
		return &const_cast<ZLQmlTreeDialog*>(this)->rootNode();
	else
		return reinterpret_cast<ZLTreeNode*>(index.internalPointer());
}

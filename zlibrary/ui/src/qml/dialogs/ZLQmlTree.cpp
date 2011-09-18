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
#include <QtCore/QDebug>
#include "ZLQmlTree.h"
#include <ZLTreeActionNode.h>
#include <ZLTreePageNode.h>
#include <ZLTimeManager.h>

Q_DECLARE_METATYPE(QModelIndex)

enum {
	SubTitleRole = Qt::UserRole,
	ActivatableRole,
	PageRole
};

ZLQmlTreeDialog::ZLQmlTreeDialog()
{
	qRegisterMetaType<QModelIndex>();
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
	qDebug() << Q_FUNC_INFO << &const_cast<ZLQmlTreeDialog*>(this)->rootNode()
	         << parent << node << node->children().size();
	return node->children().size();
}

int ZLQmlTreeDialog::columnCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	return 0;
}

//bool ZLQmlTreeDialog::hasChildren(const QModelIndex &parent) const {
//	ZLTreeNode *node = treeNode(parent);
//	return zlobject_cast<ZLTreeActionNode*>(node) == NULL;
//}

QVariant ZLQmlTreeDialog::data(const QModelIndex &index, int role) const {
	ZLTreeNode *node = treeNode(index);
	if (!node)
		return QVariant();
	
	switch (role) {
	case Qt::DisplayRole:
		if (ZLTreeTitledNode *titledNode = zlobject_cast<ZLTreeTitledNode*>(node))
			return QString::fromStdString(titledNode->title());
		else
			return QString("No title");
	case Qt::DecorationRole:
		if (ZLTreeTitledNode *titledNode = zlobject_cast<ZLTreeTitledNode*>(node))
			return QString::fromStdString(titledNode->imageUrl());
		else
			return QString();
	case SubTitleRole:
		if (ZLTreeTitledNode *titledNode = zlobject_cast<ZLTreeTitledNode*>(node))
			return QString::fromStdString(titledNode->subtitle());
		else
			return QString("No subtitle");
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

typedef std::vector<shared_ptr<ZLRunnableWithKey> > ActionVector;

QStringList ZLQmlTreeDialog::actions(const QModelIndex &index) {
	QStringList result;
	ZLTreeNode *node = treeNode(index);
	const ActionVector &actions = node->actions();
	for (ActionVector::const_iterator it = actions.begin(); it != actions.end(); ++it) {
		result << QString::fromStdString(node->actionText(*it));
	}
	return result;
}

void ZLQmlTreeDialog::run(const QModelIndex &index, int action) {
	ZLTreeNode *node = treeNode(index);
	const ActionVector &actions = node->actions();
	if (action >= 0 && action < actions.size()) {
		shared_ptr<ZLRunnableWithKey> runnable = actions.at(action);
		ZLTimeManager::Instance().addAutoRemovableTask(runnable.staticCast<ZLRunnable>());
	}
}

void ZLQmlTreeDialog::finish() {
	emit finished();
}

void ZLQmlTreeDialog::onNodeBeginInsert(ZLTreeNode *parent, size_t index) {
	qDebug() << Q_FUNC_INFO << parent << index;
	beginInsertRows(createIndex(parent), index, index);
}

void ZLQmlTreeDialog::onNodeEndInsert() {
	qDebug() << Q_FUNC_INFO;
	endInsertRows();
}

void ZLQmlTreeDialog::onNodeBeginRemove(ZLTreeNode *parent, size_t index) {
	qDebug() << Q_FUNC_INFO << parent << index;
	beginRemoveRows(createIndex(parent), index, index);
}

void ZLQmlTreeDialog::onNodeEndRemove() {
	qDebug() << Q_FUNC_INFO;
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

ZLQmlDataModel::ZLQmlDataModel() {
}

ZLQmlDataModel::~ZLQmlDataModel() {
}

QObject *ZLQmlDataModel::model() const {
	return myModel.data();
}

void ZLQmlDataModel::setModel(QObject *model) {
	if (model == myModel.data())
		return;
	beginResetModel();
	doSetModel(qobject_cast<QAbstractItemModel*>(model));
	myIndex = QModelIndex();
	emit rootIndexChanged(myIndex);
	endResetModel();
}

void ZLQmlDataModel::doSetModel(QAbstractItemModel *model) {
	if (myModel.data() == model)
		return;
	myModel = model;
	connect(model, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
	        SLOT(onRowsAboutToBeInserted(QModelIndex,int,int)));
	connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
	        SLOT(onRowsInserted(QModelIndex,int,int)));
	connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
	        SLOT(onRowsAboutToBeInserted(QModelIndex,int,int)));
	connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
	        SLOT(onRowsRemoved(QModelIndex,int,int)));
	connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
	        SLOT(onDataChanged(QModelIndex,QModelIndex)));
	emit modelChanged(myModel.data());
}

QModelIndex ZLQmlDataModel::rootIndex() const {
	return myIndex;
}

void ZLQmlDataModel::setRootIndex(const QModelIndex &index) {
	if (myIndex == index)
		return;
	beginResetModel();
	myIndex = index;
	if (index.model())
		doSetModel(const_cast<QAbstractItemModel*>(myIndex.model()));
	emit rootIndexChanged(myIndex);
	endResetModel();
}

QModelIndex ZLQmlDataModel::modelIndex(int index) const {
	return myIndex.child(index, 0);
}

int ZLQmlDataModel::rowCount(const QModelIndex &parent) const {
	Q_UNUSED(parent);
	return myIndex.row();
}

QVariant ZLQmlDataModel::data(const QModelIndex &index, int role) const {
	QModelIndex mappedIndex = myIndex.child(index.row(), 0);
	if (mappedIndex.isValid())
		return mappedIndex.data(role);
	else
		return QVariant();
}

void ZLQmlDataModel::onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last) {
	if (parent == myIndex)
		beginInsertRows(QModelIndex(), first, last);
}

void ZLQmlDataModel::onRowsInserted(const QModelIndex &parent, int first, int last) {
	Q_UNUSED(first);
	Q_UNUSED(last);
	if (parent == myIndex)
		endInsertRows();
}

void ZLQmlDataModel::onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last) {
	if (parent == myIndex)
		beginRemoveRows(QModelIndex(), first, last);
}

void ZLQmlDataModel::onRowsRemoved(const QModelIndex &parent, int first, int last) {
	Q_UNUSED(first);
	Q_UNUSED(last);
	if (parent == myIndex)
		endRemoveRows();
}

void ZLQmlDataModel::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight) {
	if (topLeft.parent() == myIndex || bottomRight.parent() == myIndex) {
		QModelIndex left = createIndex(qMin(topLeft.row(), bottomRight.row()), 0, 0);
		QModelIndex right = createIndex(qMax(topLeft.row(), bottomRight.row()), 0, 0);
		emit dataChanged(left, right);
	}
}

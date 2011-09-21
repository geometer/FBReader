#include <ZLTreeTitledNode.h>
#include <ZLTreeActionNode.h>
#include <ZLApplication.h>

#include "ZLQtTreeModel.h"

#include "../dialogs/ZLQtUtil.h"

#include <QtCore/QDebug>

ZLQtTreeModel::ZLQtTreeModel(ZLTreeListener::RootNode& rootNode, QObject *parent) :  QAbstractListModel(parent), myRootNode(rootNode) {
	myCurrentNode = &myRootNode;
}


bool ZLQtTreeModel::back() {
	if (myCurrentNode == &myRootNode) {
		return false;
	}
	myCurrentNode = myCurrentNode->parent();
	emit layoutChanged();
	return true;
}

bool  ZLQtTreeModel::enter(QModelIndex index) {
	//return false if it was action that sucessfully executed
	//qDebug() << "entering " << index.row() << index.column();

	ZLTreeNode* node = myCurrentNode->children().at(index.row());
	if (ZLTreeActionNode *actionNode = zlobject_cast<ZLTreeActionNode*>(node)) {
		bool result = actionNode->activate();
		if (result == true) {
			ZLApplication::Instance().refreshWindow();
			return false;
		}
	} else {
		myCurrentNode = node;
	}
	emit layoutChanged();
	return true;
}

int ZLQtTreeModel::rowCount(const QModelIndex &parent) const {
	//qDebug() << "asking for rowCount... returning " << myCurrentNode->children().size();
	return myCurrentNode->children().size();
}

QVariant ZLQtTreeModel::data(const QModelIndex &index, int role) const {
	if (index.isValid() && role == Qt::DisplayRole) {
		//qDebug() << "asking for data... at " << role << index.row() << index.column();
		const ZLTreeNode* node = myCurrentNode->children().at(index.row());
		if (const ZLTreeTitledNode *titledNode = zlobject_cast<const ZLTreeTitledNode*>(node)) {
			//qDebug() << "return " << ::qtString(titledNode->title());
			return ::qtString(titledNode->title());
		}
	}
	return QVariant();
}

bool ZLQtTreeModel::insertRows(int row, int count, const QModelIndex &parent) {
//	beginInsertRows();
//	endInsertRows();
}

bool ZLQtTreeModel::removeRows(int row, int count, const QModelIndex &parent) {
//	beginRemoveRows
//	endRemoveRows();
}


void ZLQtTreeModel::onNodeBeginInsert(ZLTreeNode *parent, size_t index) {

}

void ZLQtTreeModel::onNodeEndInsert() {

}

void ZLQtTreeModel::onNodeBeginRemove(ZLTreeNode *parent, size_t index) {

}

void ZLQtTreeModel::onNodeEndRemove() {

}

void ZLQtTreeModel::onNodeUpdated(ZLTreeNode *node) {
	emit layoutChanged();
}

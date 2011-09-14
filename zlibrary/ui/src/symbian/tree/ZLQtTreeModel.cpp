#include <ZLTreeTitledNode.h>

#include "ZLQtTreeModel.h"

ZLQtTreeModel::ZLQtTreeModel(ZLTreeListener::RootNode& rootNode, QObject *parent) :  QAbstractListModel(parent), myRootNode(rootNode) {
	myCurrentNode = &myRootNode;
}


int ZLQtTreeModel::rowCount(const QModelIndex &parent) const {
	return myCurrentNode->children().size();
}

QVariant ZLQtTreeModel::data(const QModelIndex &index, int role) const {
	const ZLTreeNode* node = myCurrentNode->children().at(index.row());
	if (const ZLTreeTitledNode *titledNode = zlobject_cast<const ZLTreeTitledNode*>(node)) {
		return titledNode->title().c_str();
	}
	return 0;
}

bool ZLQtTreeModel::insertRows(int row, int count, const QModelIndex &parent) {
//	beginInsertRows();
//	endInsertRows();
}

bool ZLQtTreeModel::removeRows(int row, int count, const QModelIndex &parent) {
//	beginRemoveRows();
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

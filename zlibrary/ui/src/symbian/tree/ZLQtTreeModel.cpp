#include <ZLTreeTitledNode.h>
#include <ZLTreeActionNode.h>
#include <ZLApplication.h>

#include "ZLQtTreeModel.h"

#include "../dialogs/ZLQtUtil.h"

#include <QtCore/QDebug>
#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>

#include "../menu/DrillDownMenu.h"

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
                //qDebug() << "\nrequesting children";
                myCurrentNode->requestChildren();
	}
        emit layoutChanged();
	return true;
}

int ZLQtTreeModel::rowCount(const QModelIndex &parent) const {
        //qDebug() << "asking for rowCount... returning " << myCurrentNode->children().size();
	return myCurrentNode->children().size();
}

QVariant ZLQtTreeModel::data(const QModelIndex &index, int role) const {
        if (!index.isValid() || index.row() >= myCurrentNode->children().size()) {
            //TODO may be remove second statement in 'if' above:
            //it is need in case if view don't take attention to rowCount
            //const ZLTreeNode* node = myCurrentNode->children().at(index.row());
            return QVariant();
        }

        const ZLTreeNode* node = myCurrentNode->children().at(index.row());
        switch (role) {
            case Qt::DisplayRole:
                    if (const ZLTreeTitledNode *titledNode = zlobject_cast<const ZLTreeTitledNode*>(node))
                            return QString::fromStdString(titledNode->title());
                    else
                            return QString("No title");
//            case Qt::DecorationRole:
//                    if (const ZLTreeTitledNode *titledNode = zlobject_cast<const ZLTreeTitledNode*>(node)) {
//                        return titledNode->image();
//                    }
            case SubTitleRole:
                    if (const ZLTreeTitledNode *titledNode = zlobject_cast<const ZLTreeTitledNode*>(node)) {
                            return QString::fromStdString(titledNode->subtitle());
                    }
            case Qt::SizeHintRole:
                    return MenuItemParameters::getSize();
            case Qt::FontRole:
                return MenuItemParameters::getFont();
//        case ActivatableRole:
//                return zlobject_cast<ZLTreeActionNode*>(node) != NULL;
//        case PageRole:
//                return zlobject_cast<ZLTreePageNode*>(node) != NULL;
        }
        return QVariant();
}


const ZLTreeNode* ZLQtTreeModel::getTreeNode(const QModelIndex& index) const {
    //qDebug() << Q_FUNC_INFO << index;
    if (index.isValid()) {
        //TODO remove it:
        // it needs in case if view don't take attention to rowCount
        if (index.row() >= myCurrentNode->children().size()) {
            return 0;
        }
        const ZLTreeNode* node = myCurrentNode->children().at(index.row());
        //qDebug() << "returns node";
        return node;
    }
    return 0;
}

void ZLQtTreeModel::onNodeBeginInsert(ZLTreeNode *parent, size_t index) {
    //qDebug() << Q_FUNC_INFO << parent << index << parent->childIndex();
    //TODO there should be beginInsertRows instead of layoutChanged()
    emit layoutChanged();
//    beginInsertRows(createIndex(parent), index, index);
}

void ZLQtTreeModel::onNodeEndInsert() {
    //qDebug() << Q_FUNC_INFO;
    //TODO there should be endInsertRows instead of layoutChanged()
    emit layoutChanged();
//    endInsertRows();
}

void ZLQtTreeModel::onNodeBeginRemove(ZLTreeNode *parent, size_t index) {
    //qDebug() << Q_FUNC_INFO << parent << index;
    //TODO there should be beginRemoveRows instead of layoutChanged()
    emit layoutChanged();
//    beginRemoveRows(createIndex(parent), index, index);
}

void ZLQtTreeModel::onNodeEndRemove() {
    //qDebug() << Q_FUNC_INFO;
    //TODO there should be endRemoveRows instead of layoutChanged()
    emit layoutChanged();
//    endRemoveRows();
}
void ZLQtTreeModel::onNodeUpdated(ZLTreeNode *node) {
    //qDebug() << Q_FUNC_INFO << node;
    //TODO there should be dataChanged instead of layoutChanged()
    emit layoutChanged();
//    QModelIndex index = createIndex(node);
//    emit dataChanged(index, index);
}

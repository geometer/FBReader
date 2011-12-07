#include <ZLTreeTitledNode.h>
#include <ZLTreeActionNode.h>
#include <ZLTreePageNode.h>
#include <ZLApplication.h>
#include <ZLNetworkManager.h>

#include "ZLQtTreeModel.h"
#include "../image/ZLQtImageManager.h"
#include "../dialogs/ZLQtPageDialog.h"
#include "../menu/DrillDownMenu.h"
#include "../network/ZLQtNetworkManager.h"

#include "../dialogs/ZLQtUtil.h"

#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtGui/QPainter>
#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>

#include "../menu/DrillDownMenu.h"

//TODO remove including this:
#include "../../../fbreader/src/bookmodel/TOCTreeNodes.h"

ZLQtTreeModel::ZLQtTreeModel(ZLTreeListener::RootNode& rootNode, QDialog* treeDialog, shared_ptr<ZLExecutionData::Listener> listener, QObject *parent) :
    QAbstractListModel(parent), myRootNode(rootNode), myTreeDialog(treeDialog), myListener(listener) {
    myRootNode.requestChildren(myListener);

    //TODO (ROOTNODE_WAITING_HACK) move out this hack:
    //it's need for showing loading icon at the time there's empty list;
    //it's finished with first element has been added to list
    //qDebug() << Q_FUNC_INFO << "showPercent(0,0)";
    rootNodeListWaiting = true;
    myListener->showPercent(0,0);

    myCurrentNode = &myRootNode;
    myImageProvider = new ImageProvider(ImageProvider::THUMBNAIL, this);
    connect(myImageProvider, SIGNAL(cacheUpdated()), this, SLOT(update()));
}

bool ZLQtTreeModel::back() {
        if (myCurrentNode == &myRootNode) {
                return false;
        }
        myCurrentNode = myCurrentNode->parent();
        emit currentNodeChanged(myCurrentNode);
        //TODO fix problem:
        //when change description of catalog, it changes just in case when net libraries is opened again
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
        } else if (ZLTreePageNode *pageNode = zlobject_cast<ZLTreePageNode*>(node)) {
                ZLQtPageDialog dialog(*pageNode,myTreeDialog);
                connect(myTreeDialog, SIGNAL(finished(int)), &dialog, SLOT(done(int)));
                dialog.run();
        } else {
                myCurrentNode = node;
                //qDebug() << "\nrequesting children";
                myListener->showPercent(0,0); // indeterminant progress-bar
                //TODO implement each listener for each current node, because:
                //in case if one catalog item will requesting childrens,
                //then another item will requesting others,
                //progress bar will be hided if one will completed before other
                myCurrentNode->requestChildren(myListener);
                qDebug() << Q_FUNC_INFO << "emitting current node changed";
                emit currentNodeChanged(myCurrentNode);
        }
        emit layoutChanged();
        return true;
}

void ZLQtTreeModel::update() {
    emit layoutChanged();
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
            case Qt::DecorationRole:

                //TODO move this hack away (should not know about fbreader's TOC Tree)
                /*
                  This is was made because of following reasons (it's need simple nice solve):
                  1. Very long time extracting of Book Covers (BookNode)
                  2. And, we can't call imageUrl() from Node to see, will it be long,
                     so imageUrl() calls from other thread
                  3. Also, there's caching used.

                  Summary: Looks like, ImageProvider class is very unnecessary here, and
                  should load book cover as lazy operations in FBReader code (maybe?)
                */
                if (const ReferenceTreeNode *titledNode = zlobject_cast<const ReferenceTreeNode*>(node)) {
                    return QVariant();
                }
                if (const ReferenceNode *titledNode = zlobject_cast<const ReferenceNode*>(node)) {
                    return QVariant();
                }

                //TODO check for asking decoration role only for items on screen
                if (const ZLTreeTitledNode *titledNode = zlobject_cast<const ZLTreeTitledNode*>(node)) {
                    return myImageProvider->getImageForNode(titledNode);
                }
                break;
            case SubTitleRole:
                    if (const ZLTreeTitledNode *titledNode = zlobject_cast<const ZLTreeTitledNode*>(node)) {
                            return QString::fromStdString(titledNode->subtitle());
                    }
            case Qt::SizeHintRole:
                    //return MenuItemParameters::getSmallItemSize();
                    return MenuItemParameters::getItemSize();
            case Qt::FontRole:
                return MenuItemParameters::getFont();
//        case ActivatableRole:
//                return zlobject_cast<ZLTreeActionNode*>(node) != NULL;
//        case PageRole:
                  //TODO move code from this model to view, that will be work with roles
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

    //TODO (ROOTNODE_WAITING_HACK) clean this hack
    if (rootNodeListWaiting) {
        myListener->finished();
        rootNodeListWaiting = false;
    }

    //qDebug() << Q_FUNC_INFO << parent << index << parent->childIndex();
    //TODO there should be beginInsertRows instead of layoutChanged()
    //TODO remove it, emitting signal is needed to set actions for left soft-button
    emit currentNodeChanged(myCurrentNode);
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



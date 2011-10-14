#ifndef __ZLQTTREEMODEL_H__
#define __ZLQTTREEMODEL_H__

#include <QtCore/QAbstractListModel>

#include <ZLTreeListener.h>

class ZLQtTreeModel : public QAbstractListModel {
	Q_OBJECT;

public:
        enum TreeRoles {
                SubTitleRole = Qt::UserRole,
                ActivatableRole,
                PageRole
        };

public:
	explicit ZLQtTreeModel(ZLTreeListener::RootNode& rootNode, QObject *parent = 0);

public:
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        const ZLTreeNode* getTreeNode(const QModelIndex& index) const;

public:
	void onNodeBeginInsert(ZLTreeNode *parent, size_t index);
	void onNodeEndInsert();
	void onNodeBeginRemove(ZLTreeNode *parent, size_t index);
	void onNodeEndRemove();
	void onNodeUpdated(ZLTreeNode *node);

signals:

public:
	bool back();
	bool enter(QModelIndex index);

private:
	ZLTreeListener::RootNode& myRootNode;
	ZLTreeNode* myCurrentNode;

};

#endif /* __ZLQTTREEMODEL_H__ */

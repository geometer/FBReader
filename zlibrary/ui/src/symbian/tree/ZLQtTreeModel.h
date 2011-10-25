#ifndef __ZLQTTREEMODEL_H__
#define __ZLQTTREEMODEL_H__

#include <QtCore/QAbstractListModel>

#include <QtGui/QDialog>

#include <ZLTreeListener.h>

class ZLQtTreeModel : public QAbstractListModel {
	Q_OBJECT;

public:
        enum TreeRoles {
                SubTitleRole = Qt::UserRole,
                ImageSizeRole,
                ActivatableRole,
                PageRole
        };

public:
        static QPixmap ZLImageToQPixmap(shared_ptr<ZLImage> image, QSize *size, const QSize &requestedSize);
        static QPixmap urlToQPixmap(QUrl url, QSize *size, const QSize &requestedSize);
        static QPixmap scalePixmap(const QPixmap& image, const QSize& requestedSize, bool notScaleIfLess);
        static QPixmap centerPixmap(const QPixmap& image, const QSize& requestedSize);

public:
        explicit ZLQtTreeModel(ZLTreeListener::RootNode& rootNode, QDialog* treeDialog, QObject *parent = 0);

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
        //TODO remove two-sided connection
        QDialog* myTreeDialog;

};

#endif /* __ZLQTTREEMODEL_H__ */

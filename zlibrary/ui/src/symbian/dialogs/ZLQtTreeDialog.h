#ifndef __ZLQTTREEDIALOG_H__
#define __ZLQTTREEDIALOG_H__

#include <QtGui/QDialog>
#include <QtGui/QListView>

#include <ZLTreeDialog.h>

#include "../tree/ZLQtTreeModel.h"

class ZLQtTreeDialog : public QDialog, public ZLTreeDialog{
	Q_OBJECT;
public:
	ZLQtTreeDialog(QWidget* parent = 0);

public:
	void run();

public:
        void onCloseRequest();
	void onNodeBeginInsert(ZLTreeNode *parent, size_t index);
	void onNodeEndInsert();
	void onNodeBeginRemove(ZLTreeNode *parent, size_t index);
	void onNodeEndRemove();
	void onNodeUpdated(ZLTreeNode *node);

public slots:
	void back();
        void showMenu(QModelIndex index);
	void enter(QModelIndex index);

private:
	QListView* myView;
	ZLQtTreeModel* myModel;
};

#endif /* __ZLQTTREEDIALOG_H__ */

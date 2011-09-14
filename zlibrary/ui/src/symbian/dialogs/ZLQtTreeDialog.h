#ifndef __ZLQTTREEDIALOG_H__
#define __ZLQTTREEDIALOG_H__

#include <QDialog>
#include <QListView>

#include <ZLTreeDialog.h>

#include "../tree/ZLQtTreeModel.h"

class ZLQtTreeDialog : public ZLTreeDialog, public QDialog {
	Q_OBJECT;
public:
	ZLQtTreeDialog(QWidget* parent = 0);

public:
	void run();

public:
	void onNodeBeginInsert(ZLTreeNode *parent, size_t index);
	void onNodeEndInsert();
	void onNodeBeginRemove(ZLTreeNode *parent, size_t index);
	void onNodeEndRemove();
	void onNodeUpdated(ZLTreeNode *node);

private:
	QListView* myListView;
	ZLQtTreeModel* myModel;
};

#endif /* __ZLQTTREEDIALOG_H__ */

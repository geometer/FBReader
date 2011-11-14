#ifndef __ZLQTTREEDIALOG_H__
#define __ZLQTTREEDIALOG_H__

#include <QtGui/QDialog>
#include <QtGui/QListView>
#include <QtGui/QProgressBar>
#include <QtGui/QTreeView>

#include <ZLTreeDialog.h>

#include "../tree/ZLQtTreeModel.h"
#include "../tree/ZLQtTreeView.h"

class WaitWidget;
class TreeActionListener;

class ZLQtTreeDialog : public QDialog, public ZLTreeDialog{
	Q_OBJECT
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
	void enter(QModelIndex index);
	
private:
	WaitWidget* myWaitWidget;
	shared_ptr<ZLExecutionData::Listener> myListener;
	QTreeView *myView;
	ZLQtTreeModel* myModel;
};

class WaitWidget : public QWidget {
    Q_OBJECT
public:
    WaitWidget(QWidget* parent = 0);
	
public slots:
    void showPercent(int ready, int full);
    void finished(const std::string &error);
	
	//    int value() const { return myValue; }
	//    int maximumValue() const { return myMaximum; }
	//    bool isInfinite() const { return myValue == -1; }
	
private:
    QProgressBar* myProgressBar;
	//    int myValue;
	//    int myMaximum;
};

class TreeActionListener : public QObject, public ZLExecutionData::Listener {
    Q_OBJECT
	
public:
    void showPercent(int ready, int full);
    void finished(const std::string &error);
	
signals:
    void percentChanged(int ready, int full);
    void finishedHappened(const std::string& error);
	
};

#endif /* __ZLQTTREEDIALOG_H__ */

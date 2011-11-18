#ifndef __ZLQTTREEDIALOG_H__
#define __ZLQTTREEDIALOG_H__

#include <QtGui/QDialog>
#include <QtGui/QListView>
#include <QtGui/QProgressBar>

#include <ZLTreeDialog.h>

#include "../tree/ZLQtTreeModel.h"
#include "../tree/ZLQtTreeView.h"

class WaitWidget;
class TreeActionListener;
class TreeNodeActionsMenu;

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
	void enter(QModelIndex index);
        void onCurrentNodeChanged(const ZLTreeNode* node);

private:
        WaitWidget* myWaitWidget;
        shared_ptr<ZLExecutionData::Listener> myWaitWidgetListener;
        ZLQtTreeView* myView;
	ZLQtTreeModel* myModel;
        TreeNodeActionsMenu* myActionsMenu;
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
    TreeActionListener();

public:
    void showPercent(int ready, int full);
    void finished(const std::string &error);

public:
    bool isFinished() const;

signals:
    void percentChanged(int ready, int full);
    void finishedHappened(const std::string& error);

private:
    bool myIsFinished;

};

class WrapperAction : public ZLApplication::Action {
public:
    WrapperAction(shared_ptr<ZLTreeAction> runnable);
    bool isVisible() const;
    void run();

private:
    shared_ptr<ZLTreeAction> myRunnable;
};

class TreeNodeActionsMenu : public QObject {
    Q_OBJECT

public:
    TreeNodeActionsMenu(QAction* action, QObject* parent = 0);
    void setTreeNode(const ZLTreeNode* treeNode);

public:
    static std::vector<shared_ptr<ZLTreeAction> > filterSensibleActions(const std::vector<shared_ptr<ZLTreeAction> > & actions);

signals:
    void drillDownMenuClose();

public slots:
    void activate();
    void onFinish(int);

private:
    void setActionText();

private:
    QAction* myAction;
    //TODO it's better to use shared_ptr here
    const ZLTreeNode* myTreeNode;
};

#endif /* __ZLQTTREEDIALOG_H__ */

#include <QtGui/QVBoxLayout>
#include <QtGui/QAction>
#include <QtGui/QPushButton>
#include <QtGui/QDirModel>

#include <QtCore/QDebug>

#include <ZLDialogManager.h>
#include <ZLResource.h>
#include <ZLDialogManager.h>

#include "ZLQtDialogManager.h"
#include "ZLQtProgressDialog.h"
#include "ZLQtTreeDialog.h"

#include "ScrollerManager.h"

#include "../menu/DrillDownMenu.h"
#include "../menu/ActionButtons.h"

ZLQtTreeDialog::ZLQtTreeDialog( QWidget* parent) : QDialog(parent) {
	QVBoxLayout* layout = new QVBoxLayout(this);
        TreeActionListener* listener = new TreeActionListener;
        //because we should have a shared_ptr
        myListener = listener;

        myLoadingIcon = new LoadingIcon(this);
        connect(listener, SIGNAL(percentChanged(int,int)), myLoadingIcon, SLOT(start()));
        connect(listener, SIGNAL(finishedHappened(std::string)), myLoadingIcon, SLOT(finish()));

        myView = new ZLQtTreeView;
        myModel = new ZLQtTreeModel(rootNode(), this, myListener);
	myView->setModel(myModel);
        myView->setItemDelegate(new SubtitleDelegate);

        ScrollerManager::setScroll(myView);

        //TODO should use this?
        //connect(myModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), myView, SLOT(dataChanged(QModelIndex,QModelIndex)))

        const std::string& back = ZLDialogManager::Instance().buttonName(ZLResourceKey("back"));
        QAction* action = new QAction(QString::fromStdString(back),this);
#ifdef 	__SYMBIAN__
	action->setSoftKeyRole( QAction::NegativeSoftKey );
#endif
	connect(action, SIGNAL(triggered()), this, SLOT(back()));
	addAction( action );

        connect(myView, SIGNAL(clicked(QModelIndex)), this, SLOT(enter(QModelIndex)), Qt::QueuedConnection);

	layout->addWidget(myView);

#ifndef 	__SYMBIAN__
        QPushButton* button = new ButtonAction(action);
	layout->addWidget(button);
#endif

    QAction* treeActionsMenu = new QAction(this);
#ifdef __SYMBIAN__
    treeActionsMenu->setSoftKeyRole( QAction::PositiveSoftKey );
#endif
    myActionsMenu = new TreeNodeActionsMenu(treeActionsMenu, this);
    //TODO there is no sense to read actions from TreeDialog constructor, fix it
    myActionsMenu->setTreeNode(&rootNode());
    addAction(treeActionsMenu);
    connect(myModel, SIGNAL(currentNodeChanged(const ZLTreeNode*)), this, SLOT(onCurrentNodeChanged(const ZLTreeNode*)));
    connect(treeActionsMenu, SIGNAL(triggered()), myActionsMenu, SLOT(activate()));
    connect(this,SIGNAL(finished(int)), myActionsMenu, SLOT(onFinish(int)));
#ifndef __SYMBIAN__
    // TODO QPushButton should be strongly connected with action (text and slots/signals)
    QPushButton* menuButton = new ButtonAction(treeActionsMenu);
    layout->addWidget(menuButton);
#endif
}

void ZLQtTreeDialog::back() {
	if (!myModel->back()) {
		close();
	}
}

void ZLQtTreeDialog::enter(QModelIndex index) {
        //qDebug() << Q_FUNC_INFO << index;
	if (!myModel->enter(index)) {
		close();
	}
}

void ZLQtTreeDialog::onCurrentNodeChanged(const ZLTreeNode* node) {
    //qDebug() << Q_FUNC_INFO << node;
    myActionsMenu->setTreeNode(node);
}

void ZLQtTreeDialog::run() {
	//TODO should return QDialog::Accepted ??
	//return exec() == QDialog::Accepted;
#ifdef __SYMBIAN__
	setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);
        setWindowState(Qt::WindowFullScreen);
#else
        setFixedSize(400,300);
#endif
        myLoadingIcon->moveToPosition(this->size());
	exec();
}

void ZLQtTreeDialog::onCloseRequest() {
    this->close();
}

void ZLQtTreeDialog::onNodeBeginInsert(ZLTreeNode *parent, size_t index) {
        myModel->onNodeBeginInsert(parent,index);
}

void ZLQtTreeDialog::onNodeEndInsert() {
        myModel->onNodeEndInsert();
}

void ZLQtTreeDialog::onNodeBeginRemove(ZLTreeNode *parent, size_t index) {
        myModel->onNodeBeginRemove(parent,index);
}
void ZLQtTreeDialog::onNodeEndRemove() {
        myModel->onNodeEndRemove();
}

void ZLQtTreeDialog::onNodeUpdated(ZLTreeNode *node) {
        myModel->onNodeUpdated(node);
}

void ZLQtTreeDialog::paintEvent(QPaintEvent *event) {
    QDialog::paintEvent(event);
}

void ZLQtTreeDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    myLoadingIcon->moveToPosition(event->size());
}

TreeActionListener::TreeActionListener() : myIsFinished(false) {

}

bool TreeActionListener::isFinished() const {
    return myIsFinished;
}


void TreeActionListener::showPercent(int ready, int full) {
    emit percentChanged(ready,full);
}

void TreeActionListener::finished(const std::string &error) {
    myIsFinished = true;
    emit finishedHappened(error);
}

WrapperAction::WrapperAction(shared_ptr<ZLTreeAction> runnable) : myRunnable(runnable) {

}

bool WrapperAction::isVisible() const {
    return myRunnable->makesSense();
}

void WrapperAction::run() {
    shared_ptr<ZLExecutionData::Listener> listener = new TreeActionListener;
    myRunnable->setListener(listener);
    shared_ptr<ZLQtProgressDialog> dialog = new ZLQtProgressDialog(myRunnable->key());
    TreeActionListener* treeActionListener = static_cast<TreeActionListener*>(&*listener);
    qDebug() << Q_FUNC_INFO << "running Action";
    myRunnable->run();
    dialog->run(treeActionListener);
    qDebug() << Q_FUNC_INFO << "is finished yet?" << treeActionListener->isFinished();
}

TreeNodeActionsMenu::TreeNodeActionsMenu(QAction* action, QObject* parent): QObject(parent), myAction(action), myTreeNode(0) {
    setActionText();
}

void TreeNodeActionsMenu::setTreeNode(const ZLTreeNode* treeNode) {
    //qDebug() << Q_FUNC_INFO << treeNode;
    myTreeNode = treeNode;
    setActionText();
}

std::vector<shared_ptr<ZLTreeAction> > TreeNodeActionsMenu::filterSensibleActions(const std::vector<shared_ptr<ZLTreeAction> > & actions) {
    std::vector<shared_ptr<ZLTreeAction> > senseActions;
    for (size_t i=0; i<actions.size(); ++i) {
        if (actions[i]->makesSense()) {
            senseActions.push_back(actions[i]);
        }
    }
    return senseActions;
}

void TreeNodeActionsMenu::setActionText() {
    //TODO setActionText should calls when some changes are happened
    if (!myAction) {
        qDebug() << Q_FUNC_INFO << "!myAction";
        return;
    }
    //TODO uncomment const here
    /*const*/ std::vector<shared_ptr<ZLTreeAction> > senseActions;
    if (myTreeNode) {
        //qDebug() << Q_FUNC_INFO << "asking for actions";
        senseActions = filterSensibleActions(myTreeNode->actions());
    }
    if (senseActions.size() == 0) {
        //qDebug() << Q_FUNC_INFO << "senseActions size is 0";
        myAction->setText(QString());
        return;
    }
    if (senseActions.size() == 1) {
        //qDebug() << Q_FUNC_INFO << "senseActions size is 1";
        //qDebug() << Q_FUNC_INFO << "setting " << QString::fromStdString(myTreeNode->actionText(senseActions[0]));
        myAction->setText(QString::fromStdString(myTreeNode->actionText(senseActions[0])));
    } else {
        //qDebug() << Q_FUNC_INFO << "senseActions size is more than 1";
        myAction->setText(QString::fromStdString(ZLDialogManager::Instance().buttonName(ZLResourceKey("menu"))));
    }
}

void TreeNodeActionsMenu::onFinish(int) {
    //TODO remove this slot and signal; DrillDownMenu dialog should close automatically by other way
    emit drillDownMenuClose();
}

void TreeNodeActionsMenu::activate() {
    //TODO refactor: code of this method is similar to setActionsButton
    //TODO uncomment const here
    /*const*/ std::vector<shared_ptr<ZLTreeAction> > senseActions;
    if (myTreeNode) {
        //qDebug() << Q_FUNC_INFO << "asking for actions";
        senseActions = filterSensibleActions(myTreeNode->actions());
    }
    if (senseActions.size() == 0) {
        //qDebug() << Q_FUNC_INFO << "senseActions size is 0";
        return;
    }
    if (senseActions.size() == 1) {
        //qDebug() << Q_FUNC_INFO << "senseActions size is 1, running";
        WrapperAction(senseActions.at(0)).run();
        return;
    }
    //qDebug() << Q_FUNC_INFO << "senseActions size is more than 1 showing drilldown dialog";
    ZLQtDialogManager& dialogManager = static_cast<ZLQtDialogManager&>(ZLQtDialogManager::Instance());
    DrillDownMenuDialog dialog(true, dialogManager.getParentWidget());
    connect(this, SIGNAL(drillDownMenuClose()), &dialog, SLOT(close()));
    DrillDownMenu* menu = new DrillDownMenu;
    for (size_t i=0; i < senseActions.size(); ++i) {
        //qDebug() << Q_FUNC_INFO << QString::fromStdString(myTreeNode->actionText(senseActions[i]));
        menu->addItem(myTreeNode->actionText(senseActions[i]), new WrapperAction(senseActions[i]));
    }
    dialog.showDrillDownMenu(menu);
    dialog.runNoFullScreen();
}

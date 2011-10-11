#include <QtGui/QVBoxLayout>
#include <QtGui/QAction>
#include <QtGui/QPushButton>
#include <QtGui/QDirModel>

#include <QtCore/QDebug>

#include <ZLDialogManager.h>
#include <ZLResource.h>
#include "ZLQtTreeDialog.h"

#include <QtScroller>

#include "../menu/DrillDownMenu.h"

ZLQtTreeDialog::ZLQtTreeDialog( QWidget* parent) : QDialog(parent) {
	QVBoxLayout* layout = new QVBoxLayout(this);
	myView = new QListView;
	myModel = new ZLQtTreeModel(rootNode(), this);
	myView->setModel(myModel);

        myView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        QtScroller::grabGesture(myView->viewport(), QtScroller::LeftMouseButtonGesture);

        //TODO should use this?
        //connect(myModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), myView, SLOT(dataChanged(QModelIndex,QModelIndex)))

	const ZLResource& back = ZLResource::resource("dialog")["button"]["back"];
	QAction* action = new QAction(QString::fromStdString(back.value()),this);
#ifdef 	__SYMBIAN__
	action->setSoftKeyRole( QAction::NegativeSoftKey );
#endif
	connect(action, SIGNAL(triggered()), this, SLOT(back()));
	addAction( action );

        connect(myView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(showMenu(QModelIndex)));
        connect(myView, SIGNAL(clicked(QModelIndex)), this, SLOT(enter(QModelIndex)));
	myView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	layout->addWidget(myView);

#ifndef 	__SYMBIAN__
	QPushButton* button = new QPushButton("back");
	connect(button, SIGNAL(clicked()), this, SLOT(back()));
	layout->addWidget(button);
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

class WrapperAction : public ZLApplication::Action {
public:
    WrapperAction(shared_ptr<ZLTreeAction> runnable) : myRunnable(runnable) {}
    bool isVisible() const { return myRunnable->makesSense(); }
    void run() {
        //ZLDialogManager::Instance().wait(ZLResourceKey("loadingBook"),*myRunnable);
        myRunnable->run();
    }

private:
    shared_ptr<ZLTreeAction> myRunnable;
};

void ZLQtTreeDialog::showMenu(QModelIndex index) {
        //qDebug() << Q_FUNC_INFO << index;
        const ZLTreeNode* node = myModel->getTreeNode(index);
        if (node == 0) {
            return;
        }
        DrillDownMenuDialog dialog;
        DrillDownMenu* menu = new DrillDownMenu;

        const std::vector<shared_ptr<ZLTreeAction> > & actions = node->actions();
        for (size_t i=0; i<actions.size(); ++i) {
            if (actions[i]->makesSense()) {
                menu->addItem(node->actionText(actions[i]), new WrapperAction(actions[i]));
            }
        }
        dialog.showDrillDownMenu(menu);
        dialog.runNoFullScreen();
}

void ZLQtTreeDialog::run() {
	//TODO should return QDialog::Accepted ??
	//return exec() == QDialog::Accepted;
#ifdef __SYMBIAN__
	setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);
#endif
	setWindowState(Qt::WindowFullScreen);
	exec();
}

void ZLQtTreeDialog::onCloseRequest() {
    //TODO implement
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

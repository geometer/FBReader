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
        myWaitWidget = new WaitWidget;

        TreeActionListener* listener = new TreeActionListener;
        //because we should have a shared_ptr
        myListener = listener;

        myView = new ZLQtTreeView;
        myModel = new ZLQtTreeModel(rootNode(), this, myListener);
	myView->setModel(myModel);
        myView->setItemDelegate(new SubtitleDelegate);

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

        connect(myView, SIGNAL(clicked(QModelIndex)), this, SLOT(enter(QModelIndex)));
	myView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        layout->addWidget(myWaitWidget);
	layout->addWidget(myView);

#ifndef 	__SYMBIAN__
	QPushButton* button = new QPushButton("back");
	connect(button, SIGNAL(clicked()), this, SLOT(back()));
	layout->addWidget(button);
#endif
        connect(listener,SIGNAL(percentChanged(int,int)), myWaitWidget, SLOT(showPercent(int,int)));
        connect(listener,SIGNAL(finishedHappened(std::string)), myWaitWidget, SLOT(finished(std::string)));
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

WaitWidget::WaitWidget(QWidget* parent) : QWidget(parent) {
    myProgressBar = new QProgressBar;
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(myProgressBar);
    this->setLayout(layout);
    this->hide(); // hide by default

}

void WaitWidget::showPercent(int ready, int full) {
    myProgressBar->setRange(0,full);
    myProgressBar->setValue(ready);
    this->show();
}

void WaitWidget::finished(const std::string &error) {
    this->hide();
}

void TreeActionListener::showPercent(int ready, int full) {
    emit percentChanged(ready,full);
}

void TreeActionListener::finished(const std::string &error) {
    emit finishedHappened(error);
}

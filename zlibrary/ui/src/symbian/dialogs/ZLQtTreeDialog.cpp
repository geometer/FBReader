#include <QtGui/QVBoxLayout>
#include <QtGui/QAction>
#include <QtGui/QPushButton>
#include <QtGui/QDirModel>

#include <QtCore/QDebug>

#include <ZLResource.h>
#include "ZLQtTreeDialog.h"

ZLQtTreeDialog::ZLQtTreeDialog( QWidget* parent) : QDialog(parent) {
	QVBoxLayout* layout = new QVBoxLayout(this);
	myView = new QListView;
	myModel = new ZLQtTreeModel(rootNode(), this);
	myView->setModel(myModel);

	const ZLResource& back = ZLResource::resource("dialog")["button"]["back"];
	QAction* action = new QAction(QString::fromStdString(back.value()),this);
#ifdef 	__SYMBIAN__
	action->setSoftKeyRole( QAction::NegativeSoftKey );
#endif
	connect(action, SIGNAL(triggered()), this, SLOT(back()));
	addAction( action );

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
    //TODO implement
}

void ZLQtTreeDialog::onNodeBeginInsert(ZLTreeNode *parent, size_t index) {
	//qDebug() << "onNodeBeginInsert";
	//myModel->onNodeBeginInsert(parent,index);
	//myListView->update();
}

void ZLQtTreeDialog::onNodeEndInsert() {
	//qDebug() << "onNodeEndInsert";
	//myModel->onNodeEndInsert();
	//myListView->update();
}

void ZLQtTreeDialog::onNodeBeginRemove(ZLTreeNode *parent, size_t index) {
	//qDebug() << "onNodeBeginRemove";
	//myModel->onNodeBeginRemove(parent,index);
	//myListView->update();
}

void ZLQtTreeDialog::onNodeEndRemove() {
	//qDebug() << "onNodeEndRemove";
	//myModel->onNodeEndRemove();
	//myListView->update();
}

void ZLQtTreeDialog::onNodeUpdated(ZLTreeNode *node) {
	//qDebug() << "onNodeUpdated";
	//myModel->onNodeUpdated(node);
	//myListView->update();
}

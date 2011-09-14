#include <QVBoxLayout>
#include <QPushButton>

#include "ZLQtTreeDialog.h"


ZLQtTreeDialog::ZLQtTreeDialog(QWidget* parent) : QDialog(parent) {
	QVBoxLayout* layout = new QVBoxLayout(this);
	QPushButton* button = new QPushButton("back");
	myListView = new QListView;
	myModel = new ZLQtTreeModel(rootNode(), this);

	myListView->setModel(myModel);

	layout->addWidget(myListView);
	layout->addWidget(button);
}

void ZLQtTreeDialog::run() {
	//TODO should return QDialog::Accepted ??
	//return exec() == QDialog::Accepted;
	exec();
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

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtGui/QResizeEvent>
#include <QtGui/QScrollArea>
#include <QtGui/QAction>
#include <QtCore/QDebug>

#include <ZLDialogManager.h>

#include "ZLQtOptionsDialog.h"
#include "ZLQtDialogContent.h"
#include "ZLQtUtil.h"


MyTabWidget::MyTabWidget(QWidget* parent): QWidget(parent) {
	QVBoxLayout *layout = new QVBoxLayout(this);
	//TODO remove myScrollArea from the class state?
	//TODO check order of commands here
	myScrollArea = new QScrollArea;
	myStackedWidget = new QStackedWidget;

	//TODO menu showing
	myMenuWidget = new QListWidget;

	layout->addWidget(myMenuWidget);
	layout->addWidget(myScrollArea);

//TODO setHorRules for myMenuWidget;
	myScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	myScrollArea->setWidgetResizable(false);


//TODO hide myMenuWidget and myStackedWidget in right time

	connect(myMenuWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(clicked(QModelIndex)));
//TODO remove PrefActions totally


}

//TODO rename addTab
void MyTabWidget::addTab(QWidget *widget, const QString &label) {
	myMenuWidget->addItem(label);
	myStackedWidget->addWidget(widget);
}

int MyTabWidget::currentIndex() const {
	return myStackedWidget->currentIndex();
}

void MyTabWidget::setCurrentWidget(QWidget *widget) {
	//TODO check for already containg
	//TODO remove setWidget method to one place
	myScrollArea->setWidget(myStackedWidget);
	myStackedWidget->setCurrentWidget(widget);
	myStackedWidget->update();
}

void MyTabWidget::setCurrentIndex(int index) {
	//TODO remove setWidget method to one place
	myScrollArea->setWidget(myStackedWidget);
	myStackedWidget->setCurrentIndex(index);
	myStackedWidget->update();
}

void MyTabWidget::clicked(const QModelIndex &index) {
	//TODO may be here we should emit signal to stackedWidget
	qDebug() << Q_FUNC_INFO << index.row();
	setCurrentIndex(index.row());
}

ZLQtOptionsDialog::ZLQtOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction) : QDialog(qApp->activeWindow()), ZLOptionsDialog(resource, applyAction) {
		//setModal(true);
		setWindowTitle(::qtString(caption()));
		QVBoxLayout *layout = new QVBoxLayout(this);

		myTabWidget = new MyTabWidget(this);
		layout->addWidget(myTabWidget);

		QAction* okButton = new QAction(::qtButtonName(ZLDialogManager::OK_BUTTON),this);
		okButton->setSoftKeyRole( QAction::PositiveSoftKey );
		addAction( okButton );
		connect(okButton, SIGNAL(triggered()), this, SLOT(accept()));

		QAction* cancelButton = new QAction(::qtButtonName(ZLDialogManager::CANCEL_BUTTON),this);
		cancelButton->setSoftKeyRole( QAction::NegativeSoftKey);
		addAction( cancelButton );
		connect(cancelButton, SIGNAL(triggered()), this, SLOT(reject()));

#ifndef 	__SYMBIAN__
		QPushButton* realOkButton = new QPushButton( ::qtButtonName(ZLDialogManager::OK_BUTTON), this );
		layout->addWidget(realOkButton);
		connect(realOkButton, SIGNAL(clicked()), this, SLOT(accept()));
#endif
}

void ZLQtOptionsDialog::apply() {
	ZLOptionsDialog::accept();
}

ZLDialogContent &ZLQtOptionsDialog::createTab(const ZLResourceKey &key) {
	//TODO look at what deletes DialogContent and QWidget (QStackedWidget itself owns these elements)
	ZLQtDialogContent *tab = new ZLQtDialogContent(new QWidget, tabResource(key));
	myTabWidget->addTab(tab->widget(), ::qtString(tab->displayName()));
	myTabs.push_back(tab);
	return *tab;
}

const std::string &ZLQtOptionsDialog::selectedTabKey() const {
	//TODO may be remove it?
	return myTabs[myTabWidget->currentIndex()]->key();
}

void ZLQtOptionsDialog::selectTab(const ZLResourceKey &key) {
	//TODO may be remove it?
//	for (std::vector<shared_ptr<ZLDialogContent> >::const_iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
//		if ((*it)->key() == key.Name) {
//			myTabWidget->setCurrentWidget(((ZLQtDialogContent&)**it).widget());
//			break;
//		}
//	}
}

bool ZLQtOptionsDialog::run() {
		//TODO should we call myScrollArea?
		//myScrollArea->setWidget(myWidget);
		//setModal(true);
		ZLQtDialogContent* tab = (ZLQtDialogContent*)(&(*myTabs[2]));
		myTabWidget->setCurrentWidget( tab->widget() );
		setFullScreenWithSoftButtons();
		bool code = ZLOptionsDialog::run();
		return code;
}

void ZLQtOptionsDialog::setFullScreenWithSoftButtons() {
	setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);
	setWindowState(Qt::WindowFullScreen);
}

bool ZLQtOptionsDialog::runInternal() {
	for (std::vector<shared_ptr<ZLDialogContent> >::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		((ZLQtDialogContent&)**it).close();
	}
	//TODO set focus to current widget
	//TODO maybe it should be in MyTabWidget
	//myWidget->setFocus();
	return exec() == QDialog::Accepted;
}




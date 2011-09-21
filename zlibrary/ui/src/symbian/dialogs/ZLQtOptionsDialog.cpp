#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtGui/QResizeEvent>
#include <QtGui/QScrollArea>
#include <QtGui/QAction>

#include <ZLDialogManager.h>

#include "ZLQtOptionsDialog.h"
#include "ZLQtDialogContent.h"
#include "ZLQtUtil.h"


ZLQtOptionsDialog::ZLQtOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction) : QDialog(qApp->activeWindow()), ZLOptionsDialog(resource, applyAction), myKey(resource.value()) {
		//setModal(true);
		setWindowTitle(::qtString(caption()));
		QVBoxLayout *layout = new QVBoxLayout(this);

		myScrollArea = new QScrollArea;
		myScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		myScrollArea->setWidgetResizable(false);
		layout->addWidget(myScrollArea);

		myWidget = new QWidget;
		myContent = new ZLQtDialogContent(myWidget, resource );
		// to enable setting accepting, that executes in ZLOptionsDialog::accept() :
		myTabs.push_back(myContent);

		QAction* okButton = new QAction(::qtButtonName(ZLDialogManager::OK_BUTTON),this);
#ifdef __SYMBIAN__
		okButton->setSoftKeyRole(QAction::PositiveSoftKey);
#endif
		addAction( okButton );
		connect(okButton, SIGNAL(triggered()), this, SLOT(accept()));

		QAction* cancelButton = new QAction(::qtButtonName(ZLDialogManager::CANCEL_BUTTON),this);
#ifdef __SYMBIAN__
		cancelButton->setSoftKeyRole(QAction::NegativeSoftKey);
#endif
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
	return *myContent;
}

const std::string &ZLQtOptionsDialog::selectedTabKey() const {
		return myKey.Name;
}

void ZLQtOptionsDialog::selectTab(const ZLResourceKey &key) {
}

bool ZLQtOptionsDialog::run() {
		myScrollArea->setWidget(myWidget);
		//setModal(true);
		setFullScreenWithSoftButtons();
		bool code = ZLOptionsDialog::run();
		return code;
}

void ZLQtOptionsDialog::setFullScreenWithSoftButtons() {
#ifdef __SYMBIAN__
	setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);
#endif
	setWindowState(Qt::WindowFullScreen);
}

bool ZLQtOptionsDialog::runInternal() {
	myContent->close();
	myWidget->setFocus();
	return exec() == QDialog::Accepted;
}




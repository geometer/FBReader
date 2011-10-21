#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtGui/QResizeEvent>
#include <QtGui/QScrollArea>
#include <QtGui/QAction>
#include <QtScroller>

#include <ZLDialogManager.h>

#include "ZLQtOptionsDialog.h"
#include "ZLQtDialogContent.h"
#include "ZLQtUtil.h"

#include "../menu/DrillDownMenu.h"

TabMenuWidget::TabMenuWidget(QWidget* parent): QWidget(parent) {
	QVBoxLayout *layout = new QVBoxLayout(this);
	myScrollArea = new QScrollArea;
	myStackedWidget = new QStackedWidget;
	myMenuWidget = new QListWidget;
	myMenuWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	myScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	myScrollArea->setWidgetResizable(false);
	layout->addWidget(myMenuWidget);
	layout->addWidget(myScrollArea);
	setStatus(MENU);
        connect(myMenuWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(menuItemClicked(QModelIndex)));

        myMenuWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        QtScroller::grabGesture(myMenuWidget->viewport(), QtScroller::LeftMouseButtonGesture);
        QtScroller::grabGesture(myScrollArea->viewport(), QtScroller::LeftMouseButtonGesture);

}

void TabMenuWidget::addItem(QWidget *widget, const QString &label) {
        //myMenuWidget->addItem(label);
        myMenuWidget->addItem( new NiceSizeListWidgetItem(label) );
	myStackedWidget->addWidget(widget);
}

TabMenuWidget::ShowStatus TabMenuWidget::getStatus() const {
	return myMenuWidget->isVisible() ? MENU : TAB;
}

void TabMenuWidget::setStatus(ShowStatus status) {
	if (status == MENU) {
		myScrollArea->hide();
		myMenuWidget->show();
#ifdef __SYMBIAN__
		// for phones with keyboard (activating for single-click):
		myMenuWidget->setEditFocus(true);
#endif
	} else if (status == TAB) {
		myScrollArea->show();
		myMenuWidget->hide();
		myStackedWidget->setFocus();
	}
}

void TabMenuWidget::menuItemClicked(const QModelIndex &index) {
	if (!myScrollArea->widget()) {
		myScrollArea->setWidget(myStackedWidget);
	}
	myStackedWidget->setCurrentIndex(index.row());
	setStatus(TAB);
}

ZLQtOptionsDialog::ZLQtOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction) : QDialog(qApp->activeWindow()), ZLOptionsDialog(resource, applyAction) {
		setWindowTitle(::qtString(caption()));
		QVBoxLayout *layout = new QVBoxLayout(this);

		myTabMenuWidget = new TabMenuWidget(this);
		layout->addWidget(myTabMenuWidget);

		const ZLResource& back = ZLResource::resource("dialog")["button"]["back"];
		QAction* backAction = new QAction(QString::fromStdString(back.value()),this);
		backAction->setSoftKeyRole( QAction::NegativeSoftKey);
		addAction( backAction );
		connect(backAction, SIGNAL(triggered()), this, SLOT(back()));

#ifndef 	__SYMBIAN__
		QPushButton* backButton = new QPushButton(QString::fromStdString(back.value()));
		layout->addWidget(backButton);
		connect(backButton, SIGNAL(clicked()), this, SLOT(back()));
#endif
}

void ZLQtOptionsDialog::back() {
	if (myTabMenuWidget->getStatus() == TabMenuWidget::MENU) {
		QDialog::accept();
		return;
	}
	myTabMenuWidget->setStatus(TabMenuWidget::MENU);
}

ZLDialogContent &ZLQtOptionsDialog::createTab(const ZLResourceKey &key) {
        ZLQtDialogContent *tab = new ZLQtDialogContent(tabResource(key));
	myTabMenuWidget->addItem(tab->widget(), ::qtString(tab->displayName()));
	myTabs.push_back(tab);
	return *tab;
}

const std::string &ZLQtOptionsDialog::selectedTabKey() const {
	return myEmptyString;
}

void ZLQtOptionsDialog::selectTab(const ZLResourceKey &key) {
	Q_UNUSED(key);
}

bool ZLQtOptionsDialog::run() {
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
	for (std::vector<shared_ptr<ZLDialogContent> >::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		((ZLQtDialogContent&)**it).close();
	}
	return exec() == QDialog::Accepted;
}




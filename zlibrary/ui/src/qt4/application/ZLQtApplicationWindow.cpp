/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <QtGui/QApplication>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>
#include <QtGui/QToolBar>
#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtGui/QLineEdit>
#include <QtGui/QWheelEvent>

#include <ZLibrary.h>
#include <ZLFile.h>
#include <ZLPopupData.h>
#include <ZLKeyBindings.h>

#include "ZLQtApplicationWindow.h"
#include "../view/ZLQtViewWidget.h"
#include "../util/ZLQtKeyUtil.h"
#include "../util/ZLQtToolbarButton.h"

ZLQtApplicationWindow *ZLQtApplicationWindow::ourInstance = 0;

ZLQtApplicationWindow *ZLQtApplicationWindow::Instance() {
	return ourInstance;
}

static const std::string OPTIONS = "Options";

ZLQtApplicationWindow::ZLQtApplicationWindow(ZLApplication *application) :
	ZLQtMainWindow(0, std::string()),
	ZLApplicationWindow(application),
	mySearchBox(0),
	mySearchBoxAction(0),
	myCursorIsHyperlink(false) {

	const std::string iconFileName = ZLibrary::ImageDirectory() + ZLibrary::FileNameDelimiter + ZLibrary::ApplicationName() + ".png";
	QPixmap icon(iconFileName.c_str());
	setWindowIcon(icon);

	menuBar()->hide();

	ourInstance = this;
}

QLineEdit *ZLQtApplicationWindow::searchBox() {
	if (mySearchBox == 0) {
		mySearchBox = new QLineEdit(myToolbar);
		mySearchBox->setAttribute(Qt::WA_MacShowFocusRect, false);
		mySearchBox->setStyleSheet("QLineEdit { height:19px; border: 1px solid gray; border-radius: 10px; padding-left: 10px; padding-right:10px }");
		mySearchBoxAction = myToolbar->addWidget(mySearchBox);
	}
	return mySearchBox;
}

void ZLQtApplicationWindow::hideSearchBox() {
	if (mySearchBoxAction != 0) {
		myToolbar->removeAction(mySearchBoxAction);
		mySearchBoxAction = 0;
		mySearchBox = 0;
	}
}

void ZLQtApplicationWindow::initMenu() {
	MenuBuilder(*this).processMenu(application());
}

void ZLQtApplicationWindow::init() {
	typedef std::map<std::string,shared_ptr<ZLApplication::Action> > ActionMap;
	const ActionMap &actions = application().actions();
	for (ActionMap::const_iterator it = actions.begin(); it != actions.end(); ++it) {
		getAction(it->first);
	}
	typedef std::map<std::string,std::string> BindingMap;
	const BindingMap &bindings = application().keyBindings()->bindings();
	for (BindingMap::const_iterator it = bindings.begin(); it != bindings.end(); ++it) {
		ZLQtAction *action = getAction(it->second);
		QList<QKeySequence> shortcuts = action->shortcuts();
		shortcuts.append(QKeySequence(QString::fromStdString(it->first)));
		action->setShortcuts(shortcuts);
	}

	ZLApplicationWindow::init();

	QWidget* spacer = new QWidget(myToolbar);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	myToolbar->addWidget(spacer);
}

ZLQtApplicationWindow::~ZLQtApplicationWindow() {
	ourInstance = 0;
}

void ZLQtApplicationWindow::wheelEvent(QWheelEvent *event) {
	if (event->orientation() == Qt::Vertical) {
		if (event->delta() > 0) {
			application().doActionByKey(ZLApplication::MouseScrollUpKey);
		} else {
			application().doActionByKey(ZLApplication::MouseScrollDownKey);
		}
	}
}

void ZLQtApplicationWindow::closeEvent(QCloseEvent *event) {
	if (application().closeView()) {
		event->accept();
	} else {
		event->ignore();
	}
}

void ZLQtApplicationWindow::addToolbarItem(ZLToolbar::ItemPtr item) {
	if (item->isSeparator()) {
		myToolbarActions[item] = myToolbar->addSeparator();
	} else {
		ZLToolbar::ButtonItem& buttonItem = (ZLToolbar::ButtonItem&)*item;
		QAction *action = getAction(buttonItem.actionId());
		ZLQtToolbarButton *button = new ZLQtToolbarButton(buttonItem.iconName(), myToolbar);
		button->setToolTip(QString::fromUtf8(buttonItem.tooltip().c_str()));
		connect(button, SIGNAL(clicked()), action, SLOT(onActivated()));
		myToolbarActions[item] = myToolbar->addWidget(button);
	}
}

void ZLQtApplicationWindow::setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled) {
	QAction *action = myToolbarActions[item];
	if (action != 0) {
		action->setEnabled(enabled);
		action->setVisible(visible);
	}
}

ZLViewWidget *ZLQtApplicationWindow::createViewWidget() {
	ZLQtViewWidget *viewWidget = new ZLQtViewWidget(this, &application());
	setCentralWidget(viewWidget->widget());
	viewWidget->widget()->show();
	return viewWidget;
}

void ZLQtApplicationWindow::close() {
	QMainWindow::close();
}

void ZLQtApplicationWindow::refresh() {
	QMetaObject::invokeMethod(this, "onRefresh", Qt::AutoConnection);
}

void ZLQtApplicationWindow::onRefresh() {
	for (std::list<ZLQtAction*>::const_iterator it = myMenuActions.begin(); it != myMenuActions.end(); ++it) {
		ZLQtAction *action = *it;
		action->setVisible(application().isActionVisible(action->Id));
		action->setEnabled(application().isActionEnabled(action->Id));
	}
	for (std::list<ZLQtMenu*>::const_iterator it = mySubmenuList.begin(); it != mySubmenuList.end(); ++it) {
		ZLQtMenu *menu = *it;
		shared_ptr<ZLPopupData> data = application().popupData(menu->Id);
		if (data.isNull() || data->generation() == menu->Generation) {
			continue;
		}
		menu->Generation = data->generation();
		menu->clear();
		const size_t count = data->count();
		for (size_t i = 0; i < count; ++i) {
			ZLQtAction *action = new ZLQtAction(application(), menu->Id, menu);
			action->setActionIndex(i);
			action->setText(QString::fromUtf8(data->text(i).c_str()));
			menu->addAction(action);
		}
	}
	refreshToolbar();
	qApp->processEvents();
}

void ZLQtApplicationWindow::grabAllKeys(bool) {
}

void ZLQtApplicationWindow::setCaption(const std::string &caption) {
	QMainWindow::setWindowTitle(QString::fromUtf8(caption.c_str()));
}

void ZLQtApplicationWindow::setHyperlinkCursor(bool hyperlink) {
	if (hyperlink == myCursorIsHyperlink) {
		return;
	}
	myCursorIsHyperlink = hyperlink;
	if (hyperlink) {
		myStoredCursor = cursor();
		setCursor(Qt::PointingHandCursor);
	} else {
		setCursor(myStoredCursor);
	}
}

void ZLQtApplicationWindow::setFocusToMainWidget() {
	centralWidget()->setFocus();
}

ZLQtApplicationWindow::MenuBuilder::MenuBuilder(ZLQtApplicationWindow &window) : myWindow(window) {
}

ZLQtMenu::ZLQtMenu(const std::string &id, const std::string &title) : QMenu(QString::fromUtf8(title.c_str())), Id(id), Generation(size_t(-1)) {
}

void ZLQtApplicationWindow::MenuBuilder::processSubmenuBeforeItems(ZLMenubar::Submenu &submenu) {
	ZLQtMenu *menu = new ZLQtMenu(submenu.id(), submenu.menuName());
	if (myMenuStack.empty()) {
		if (!myWindow.menuBar()->isVisible()) {
			myWindow.menuBar()->show();
		}
		myWindow.menuBar()->addMenu(menu);
	} else {
		myMenuStack.back()->addMenu(menu);
		myWindow.mySubmenuList.push_back(menu);
	}
	myMenuStack.push_back(menu);
}

void ZLQtApplicationWindow::MenuBuilder::processSubmenuAfterItems(ZLMenubar::Submenu &submenu) {
	myMenuStack.pop_back();
}

ZLQtAction::ZLQtAction(ZLApplication &application, const std::string &id, QObject *parent) : QAction(parent), myApplication(application), Id(id) {
	connect(this, SIGNAL(triggered()), this, SLOT(onActivated()));
}

void ZLQtAction::setActionIndex(std::size_t index) {
	myActionIndex = index;
}

void ZLQtAction::onActivated() {
	myApplication.doAction(Id, myActionIndex);
}

void ZLQtApplicationWindow::MenuBuilder::processItem(ZLMenubar::PlainItem &item) {
	ZLQtAction *action = myWindow.getAction(item.actionId());
	action->setText(QString::fromUtf8(item.name().c_str()));
	myMenuStack.back()->addAction(action);
	myWindow.addMenuAction(action);
}

ZLQtAction *ZLQtApplicationWindow::getAction(const std::string &actionId) {
	std::map<std::string,ZLQtAction*>::const_iterator it = myActions.find(actionId);
	if (it != myActions.end()) {
		return it->second;
	}
	ZLQtAction *action = new ZLQtAction(application(), actionId);
	addAction(action);
	myActions[actionId] = action;
	return action;
}

void ZLQtApplicationWindow::addMenuAction(ZLQtAction *action) {
	myMenuActions.push_back(action);
}

void ZLQtApplicationWindow::MenuBuilder::processSeparator(ZLMenubar::Separator &separator) {
	myMenuStack.back()->addSeparator();
}

void ZLQtApplicationWindow::toggleFullscreen() {
	if (isFullScreen()) {
		showNormal();
	} else {
		showFullScreen();
	}
}

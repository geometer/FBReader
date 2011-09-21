/*
 * Copyright (C) 2004-2011 Geometer Plus <contact@geometerplus.com>
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
#include <QtGui/QImage>
#include <QtGui/QIcon>
#include <QtGui/QToolBar>
#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtGui/QToolButton>
#include <QtGui/QLayout>
#include <QtGui/QWheelEvent>
#include <QtGui/QDockWidget>
#include <QtGui/QFileSystemModel>
#include <QtCore/QtDebug>
#include <QtCore/QTimer>

#include <ZLibrary.h>
#include <ZLPopupData.h>

#include "ZLQmlApplicationWindow.h"
#include "../dialogs/ZLQmlDialogManager.h"
#include "../dialogs/ZLQmlFileSystemModel.h"
#include "../dialogs/ZLQmlTree.h"
#include "../view/ZLQmlViewWidget.h"

void ZLQmlDialogManager::createApplicationWindow(ZLApplication *application) const {
	new ZLQmlApplicationWindow(application);
}

void ZLQmlApplicationWindow::setToggleButtonState(const ZLToolbar::ToggleButtonItem &button) {
	ZLQmlToolBarItem *item = myActionsHash.value(&button);
	if (ZLQmlToolBarAction *action = qobject_cast<ZLQmlToolBarAction*>(item))
		action->setChecked(button.isPressed());
}

ZLQmlApplicationWindow::ZLQmlApplicationWindow(ZLApplication *application) :
	ZLApplicationWindow(application), myFullScreen(false) {
	myMenu = new ZLQmlMenuBar(this);
#ifdef MEEGO_EDITION
	myResourceSet = new ResourcePolicy::ResourceSet(QLatin1String("player"), this);
	myResourceSet->addResourceObject(new ResourcePolicy::ScaleButtonResource);
	myResourceSet->acquire();
	qApp->installEventFilter(this);
#endif
}

void ZLQmlApplicationWindow::init() {
	qmlRegisterType<ZLQmlFileSystemModel>("org.fbreader", 0, 14, "FileSystemModel");
	qmlRegisterType<ZLQmlBookContent>("org.fbreader", 0, 14, "BookView");
	qmlRegisterType<ZLQmlDataModel>("org.fbreader", 0, 14, "DataModel");
	qmlRegisterUncreatableType<ZLQmlToolBarItem>("org.fbreader", 0, 14, "ToolBarItem", "Type is uncreatable");
	ZLApplicationWindow::init();
}

void ZLQmlApplicationWindow::initMenu() {
	myMenu->init();
}

ZLQmlApplicationWindow::~ZLQmlApplicationWindow() {
}

void ZLQmlApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == myFullScreen)
		return;
	myFullScreen = fullscreen;
	emit fullScreenChanged(myFullScreen);
}

bool ZLQmlApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

ZLQmlMenuBar::ZLQmlMenuBar(ZLQmlApplicationWindow *window) : QObject(window) {
	connect(this, SIGNAL(activated(int)),
	        this, SLOT(delayedActivate(int)),
	        Qt::QueuedConnection);
}

ZLQmlMenuBar::~ZLQmlMenuBar() {
}

void ZLQmlMenuBar::init() {
	ZLQmlApplicationWindow *window = static_cast<ZLQmlApplicationWindow*>(parent());
	
	Builder builder;
	builder.processMenu(window->application());
	myIds = builder.ids();
	myNames = builder.names();
	recheckItems();
	emit itemsChanged(myNames);
}

QStringList ZLQmlMenuBar::items() {
	return myNames;
}

QStringList ZLQmlMenuBar::visibleItems() {
	return myVisibleItems;
}

QStringList ZLQmlMenuBar::enabledItems() {
	return myEnabledItems;
}

void ZLQmlMenuBar::activate(int index) {
	emit activated(index);
}

void ZLQmlMenuBar::recheckItems() {
	ZLQmlApplicationWindow *window = static_cast<ZLQmlApplicationWindow*>(parent());
	ZLApplication &application = window->application();
	myVisibleItems.clear();
	myEnabledItems.clear();
	for (int i = 0; i < myIds.size(); ++i) {
		shared_ptr<ZLApplication::Action> action = application.action(myIds.at(i));
		Q_ASSERT(!action.isNull());
		if (action->isEnabled())
			myEnabledItems << myNames.at(i);
		if (action->isVisible())
			myVisibleItems << myNames.at(i);
	}
	emit enabledItemsChanged(myEnabledItems);
	emit visibleItemsChanged(myVisibleItems);
}

void ZLQmlMenuBar::delayedActivate(int index) {
	ZLQmlApplicationWindow *window = static_cast<ZLQmlApplicationWindow*>(parent());
	shared_ptr<ZLApplication::Action> action = window->application().action(myIds.at(index));
	Q_ASSERT(!action.isNull());
	action->checkAndRun();
}

ZLQmlMenuBar::Builder::Builder() {
}

ZLQmlMenuBar::Builder::~Builder() {
}

QStringList ZLQmlMenuBar::Builder::names() {
	return myNames;
}

QList<std::string> ZLQmlMenuBar::Builder::ids() {
	return myIds;
}

void ZLQmlMenuBar::Builder::processSubmenuBeforeItems(ZLMenubar::Submenu &submenu) {
	// Submenus are BAD
	Q_UNUSED(submenu);
}

void ZLQmlMenuBar::Builder::processSubmenuAfterItems(ZLMenubar::Submenu &submenu) {
	// Submenus are BAD
	Q_UNUSED(submenu);
}

void ZLQmlMenuBar::Builder::processItem(ZLMenubar::PlainItem &item) {
	myNames << QString::fromUtf8(item.name().c_str());
	myIds << item.actionId();
}

void ZLQmlMenuBar::Builder::processSepartor(ZLMenubar::Separator &separator) {
	Q_UNUSED(separator);
}

bool ZLQmlApplicationWindow::eventFilter(QObject *obj, QEvent *event) {
#ifdef MEEGO_EDITION
	if (event->type() == QEvent::ApplicationActivate)
		myResourceSet->acquire();
	else if (event->type() == QEvent::ApplicationDeactivate)
		myResourceSet->release();
#endif
	return QObject::eventFilter(obj, event);
}

void ZLQmlApplicationWindow::addToolbarItem(ZLToolbar::ItemPtr item) {
	// We have only full screen mode
	qDebug() << Q_FUNC_INFO << type(*item) << item->type();
	if (type(*item) == FULLSCREEN_TOOLBAR)
		return;
	ZLQmlToolBarItem *action = 0;
	
	switch (item->type()) {
		case ZLToolbar::Item::PLAIN_BUTTON:
		case ZLToolbar::Item::TOGGLE_BUTTON:
			qDebug("\"%s\" \"%s\"",
				   static_cast<ZLToolbar::MenuButtonItem&>(*item).actionId().c_str(),
				   static_cast<ZLToolbar::MenuButtonItem&>(*item).label().c_str());
			action = new ZLQmlToolBarAction(static_cast<ZLToolbar::AbstractButtonItem&>(*item), this);
			break;
		case ZLToolbar::Item::MENU_BUTTON:
			qDebug("\"%s\" \"%s\"",
			       static_cast<ZLToolbar::MenuButtonItem&>(*item).actionId().c_str(),
			       static_cast<ZLToolbar::MenuButtonItem&>(*item).label().c_str());
			action = new ZLQmlToolBarMenu(static_cast<ZLToolbar::MenuButtonItem&>(*item), this);
			break;
		case ZLToolbar::Item::TEXT_FIELD:
		case ZLToolbar::Item::SEARCH_FIELD:
		{
		// TODO: implement
//			ZLToolbar::ParameterItem &textFieldItem =
//				(ZLToolbar::ParameterItem&)*item;
//			LineEditParameter *para = new LineEditParameter(tb, *this, textFieldItem);
//			addVisualParameter(textFieldItem.parameterId(), para);
//			item = para->action();
			break;
		}
		case ZLToolbar::Item::SEPARATOR:
			break;
		case ZLToolbar::Item::COMBO_BOX:
		// TODO: implement
			break;
		case ZLToolbar::Item::FILL_SEPARATOR:
			break;
	}
	if (action) {
		myActionsHash.insert(&*item, action);
		myActions << action;
		emit actionsChanged(actions());
	}
}

void ZLQmlApplicationWindow::setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled) {
	ZLQmlToolBarItem *action = myActionsHash.value(&*item);
	if (!action)
		return;
	action->setEnabled(enabled);
	action->setVisible(visible);
	switch (item->type()) {
		default:
			break;
		case ZLToolbar::Item::MENU_BUTTON:
		{
			ZLToolbar::MenuButtonItem &buttonItem = static_cast<ZLToolbar::MenuButtonItem&>(*item);
			ZLQmlToolBarMenu *menu = static_cast<ZLQmlToolBarMenu*>(action);
			menu->setPopupData(buttonItem.popupData());
			break;
		}
	}
}

QDeclarativeListProperty<QObject> ZLQmlApplicationWindow::actions()
{
	QDeclarativeListProperty<QObject> actions(this, myActions);
	actions.append = NULL;
	actions.clear = NULL;
	return actions;
}

void ZLQmlApplicationWindow::processAllEvents() {
	qApp->processEvents();
}

ZLViewWidget *ZLQmlApplicationWindow::createViewWidget() {
	ZLQmlViewObject *viewWidget = new ZLQmlViewObject(this, &application());
	viewWidget->widget()->showFullScreen();
	return viewWidget;
}

void ZLQmlApplicationWindow::close() {
}

void ZLQmlApplicationWindow::grabAllKeys(bool) {
}

void ZLQmlApplicationWindow::setCaption(const std::string &caption) {
	Q_UNUSED(caption);
}

void ZLQmlApplicationWindow::setHyperlinkCursor(bool hyperlink) {
	Q_UNUSED(hyperlink);
}

void ZLQmlApplicationWindow::setFocusToMainWidget() {
//	centralWidget()->setFocus();
}

QObject *ZLQmlApplicationWindow::menuBar() {
	return myMenu;
}

ZLQmlToolBarItem::ZLQmlToolBarItem(ZLToolbar::Item::Type type, QObject *parent)
    : QObject(parent), myVisible(true), myEnabled(true), myType(static_cast<Type>(type)) {
}

ZLQmlToolBarItem::~ZLQmlToolBarItem() {
}

ZLQmlToolBarItem::Type ZLQmlToolBarItem::type() const {
	return myType;
}

bool ZLQmlToolBarItem::enabled() const {
	return myEnabled;
}

void ZLQmlToolBarItem::setEnabled(bool enabled) {
	if (myEnabled == enabled)
		return;
	myEnabled = enabled;
	emit enabledChanged(myEnabled);
}

bool ZLQmlToolBarItem::visible() const {
	return myVisible;
}

void ZLQmlToolBarItem::setVisible(bool visible) {
	if (myVisible == visible)
		return;
	myVisible = visible;
	emit visibleChanged(myVisible);
}

const char *platformIcons[][2] = {
    { "addBook", "toolbar-add" },
	{ "advancedSearchOnNetwork", "" },
	{ "bookInfo", "" },
	{ "byAuthor", "" },
	{ "byTag", "" },
	{ "findNext", "" },
	{ "findPrevious", "" },
	{ "gotoHome", "toolbar-home" },
	{ "preferences", "toolbar-settings" },
	{ "redo", "toolbar-redo" },
	{ "rotate", "" },
	{ "search", "toolbar-search" },
	{ "showHelp", "" },
	{ "library", "" },
	{ "networkLibrary", "" },
	{ "showReading", "" },
	{ "showRecent", "" },
	{ "toc", "toolbar-list" },
	{ "undo", "toolbar-undo" }
};

ZLQmlToolBarAction::ZLQmlToolBarAction(ZLToolbar::AbstractButtonItem &item, QObject *parent)
    : ZLQmlToolBarItem(item.type(), parent), myChecked(false), myItem(item) {
	myIconSource = QString::fromUtf8(ZLibrary::ApplicationImageDirectory().c_str());
	myIconSource += '/';
	myIconSource += QString::fromUtf8(myItem.iconName().c_str());
	myIconSource += QLatin1String(".png");
	for (uint i = 0; i < sizeof(platformIcons)/sizeof(platformIcons[0]); ++i) {
		if (platformIcons[i][0] == myItem.iconName()) {
			myPlatformIconId = QLatin1String(platformIcons[i][1]);
			break;
		}
	}
}

ZLQmlToolBarAction::~ZLQmlToolBarAction() {
}

QString ZLQmlToolBarAction::iconSource() const {
	return myIconSource;
}

QString ZLQmlToolBarAction::platformIconId() const {
	return myPlatformIconId;
}

bool ZLQmlToolBarAction::checked() const {
	return myChecked;
}

void ZLQmlToolBarAction::setChecked(bool checked) {
	if (checked == myChecked)
		return;
	myChecked = checked;
	emit checkedChanged(myChecked);
}

void ZLQmlToolBarAction::activate() {
	QTimer::singleShot(0, this, SLOT(delayedActivate()));
}

void ZLQmlToolBarAction::delayedActivate() {
	static_cast<ZLQmlApplicationWindow*>(parent())->onButtonPress(myItem);
}

ZLQmlToolBarMenu::ZLQmlToolBarMenu(ZLToolbar::MenuButtonItem &item, QObject *parent)
    : ZLQmlToolBarAction(item, parent), myPopupData(item.popupData()) {
//	setPopupData(item.popupData());
}

ZLQmlToolBarMenu::~ZLQmlToolBarMenu() {
}

QStringList ZLQmlToolBarMenu::items() {
	return myItems;
}

void ZLQmlToolBarMenu::setPopupData(const shared_ptr<ZLPopupData> &popupData) {
//	if ((myPopupData.isNull() ? -1 : myPopupData->id()) == (popupData.isNull() ? -1 : popupData->id()))
//		return;
	myPopupData = popupData;
	myItems.clear();
	if (!myPopupData.isNull()) {
		for (uint i = 0; i < myPopupData->count(); ++i)
			myItems << QString::fromUtf8(myPopupData->text(i).c_str());
	}
	emit itemsChanged(myItems);
}

void ZLQmlToolBarMenu::activate(int index) {
	if (!myPopupData.isNull())
		myPopupData->run(index);
}

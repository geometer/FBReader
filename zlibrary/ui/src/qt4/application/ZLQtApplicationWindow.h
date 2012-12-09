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

#ifndef __ZLQTAPPLICATIONWINDOW_H__
#define __ZLQTAPPLICATIONWINDOW_H__

#include <map>
#include <list>

#include <QtGui/QMainWindow>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QCursor>

#include <ZLOptions.h>

class QToolButton;
class QLineEdit;

class ZLPopupData;

class ZLQtAction;
class ZLQtMenu;

#include "../../../../core/src/application/ZLApplicationWindow.h"
#include "../../../../core/src/application/ZLMenu.h"
#include "../util/ZLQtMainWindow.h"

class ZLQtApplicationWindow : public ZLQtMainWindow, public ZLApplicationWindow {
	Q_OBJECT

public:
	static ZLQtApplicationWindow *Instance();

private:
	static ZLQtApplicationWindow *ourInstance;

public:
	ZLQtApplicationWindow(ZLApplication *application);
	~ZLQtApplicationWindow();

	void setFocusToMainWidget();

	void toggleFullscreen();

	QLineEdit *searchBox();
	void hideSearchBox();

private:
	class MenuBuilder : public ZLMenuVisitor {

	public:
		MenuBuilder(ZLQtApplicationWindow &window);

	private:
		void processSubmenuBeforeItems(ZLMenubar::Submenu &submenu);
		void processSubmenuAfterItems(ZLMenubar::Submenu &submenu);
		void processItem(ZLMenubar::PlainItem &item);
		void processSeparator(ZLMenubar::Separator &separator);

	private:
		ZLQtApplicationWindow &myWindow;
		std::list<QMenu*> myMenuStack;
	};

	void initMenu();

	ZLViewWidget *createViewWidget();
	ZLQtAction *getAction(const std::string &actionId);

	void addToolbarItem(ZLToolbar::ItemPtr item);
	void addMenuAction(ZLQtAction *action);
	void init();
	void close();

	void refresh();

	void grabAllKeys(bool grab);

	void setCaption(const std::string &caption);

	void setHyperlinkCursor(bool hyperlink);

	void setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled);

	void closeEvent(QCloseEvent *event);
	void wheelEvent(QWheelEvent *event);

private Q_SLOTS:
	void onRefresh();

private:
	QLineEdit *mySearchBox;
	QAction *mySearchBoxAction;

	std::map<std::string,ZLQtAction*> myActions;
	std::map<ZLToolbar::ItemPtr,QAction*> myToolbarActions;
	std::list<ZLQtAction*> myMenuActions;
	std::list<ZLQtMenu*> mySubmenuList;

	bool myCursorIsHyperlink;
	QCursor myStoredCursor;
};

class ZLQtAction : public QAction {
	Q_OBJECT

public:
	ZLQtAction(ZLApplication &application, const std::string &id, QObject *parent = 0);
	void setActionIndex(std::size_t index);

private Q_SLOTS:
	void onActivated();

private:
	ZLApplication &myApplication;

public:
	const std::string Id;

private:
	std::size_t myActionIndex;
};

class ZLQtMenu : public QMenu {

public:
	ZLQtMenu(const std::string &id, const std::string &title);

public:
	const std::string Id;
	size_t Generation;
};

#endif /* __ZLQTAPPLICATIONWINDOW_H__ */

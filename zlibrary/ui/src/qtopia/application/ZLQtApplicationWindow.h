/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <vector>
#include <map>
#include <set>
#include <stack>

#include <qmainwindow.h>

#include "../../../../core/src/application/ZLApplicationWindow.h"

class QMenuBar;
class QPopupMenu;
class ZLQtMenuAction;
class QPixmap;

class ZLQtApplicationWindow;

class MyMainWindow : public QMainWindow {
	Q_OBJECT

public:
	MyMainWindow(ZLQtApplicationWindow *applicationWindow) : QMainWindow(), myApplicationWindow(applicationWindow) {}
	WFlags getWFlags() { return QMainWindow::getWFlags(); }
	void setWFlags(WFlags flags) { QMainWindow::setWFlags(flags); }

	void focusInEvent(QFocusEvent *event);
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);

private slots:
	void setDocument(const QString &fileName);

private:
	ZLQtApplicationWindow *myApplicationWindow;
};

class ZLQtApplicationWindow : public ZLApplicationWindow {

public:
	ZLQtApplicationWindow(ZLApplication *application);

	void fullScreenWorkaround();
	int verticalAdjustment();

	QMainWindow *mainWindow();

private:
	ZLViewWidget *createViewWidget();

	void addToolbarItem(ZLApplication::Toolbar::ItemPtr item);
	void initMenu();
	class MenuMaskCalculator : public ZLApplication::MenuVisitor {

	public:
		MenuMaskCalculator(ZLQtApplicationWindow &window);
		bool shouldBeUpdated();

	private:
		void processSubmenuBeforeItems(ZLApplication::Menubar::Submenu &submenu);
		void processSubmenuAfterItems(ZLApplication::Menubar::Submenu &submenu);
		void processItem(ZLApplication::Menubar::PlainItem &item);
		void processSepartor(ZLApplication::Menubar::Separator &separator);

	private:
		ZLQtApplicationWindow &myWindow;
		bool myFirstTime;
		bool myShouldBeUpdated;
		int myCounter;
	};
	class MenuUpdater : public ZLApplication::MenuVisitor {

	public:
		MenuUpdater(ZLQtApplicationWindow &window);

	private:
		void processSubmenuBeforeItems(ZLApplication::Menubar::Submenu &submenu);
		void processSubmenuAfterItems(ZLApplication::Menubar::Submenu &submenu);
		void processItem(ZLApplication::Menubar::PlainItem &item);
		void processSepartor(ZLApplication::Menubar::Separator &separator);

	private:
		ZLQtApplicationWindow &myWindow;
		std::stack<QPopupMenu*> myMenuStack;
		int myCounter;
	};
	void updateMenu();

	void refresh();
	void close();

	void grabAllKeys(bool grab);

	void setCaption(const std::string &caption);

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	void setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button);
	void setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled);

private:
	std::vector<bool> myToolbarMask;
	std::vector<bool> myMenuMask;
	std::map<std::string,ZLQtMenuAction*> myMenuMap;

	bool myFullScreen;
	int myTitleHeight;

	int myVerticalDelta;

	class MyMainWindow *myMainWindow;
	QMenuBar *myToolBar;
	QPopupMenu *myMenu;

friend class MyMainWindow;
friend class MenuMaskCalculator;
friend class MenuUpdater;
friend class ToolBarButton;
friend class ZLQtMenuAction;
};

class ToolBarButton : public QObject {
	Q_OBJECT

public:	
	ToolBarButton(ZLQtApplicationWindow &window, ZLApplication::Toolbar::ButtonItem &button);
	~ToolBarButton();

	QPixmap &pixmap();

	bool toggle();

public slots:
	void doActionSlot();

private:
	ZLQtApplicationWindow &myWindow;
	ZLApplication::Toolbar::ButtonItem &myButton;
	QPixmap *myReleasedPixmap;
	QPixmap *myPressedPixmap;
	bool myIsPressed;
};

inline bool ZLQtApplicationWindow::MenuMaskCalculator::shouldBeUpdated() { return myShouldBeUpdated; }

#endif /* __ZLQTAPPLICATIONWINDOW_H__ */

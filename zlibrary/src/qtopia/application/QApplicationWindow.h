/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#ifndef __QAPPLICATIONWINDOW_H__
#define __QAPPLICATIONWINDOW_H__

#include <vector>
#include <map>
#include <set>
#include <stack>

#include <qmainwindow.h>

#include <ZLApplication.h>

class QMenuBar;
class QPopupMenu;
class QtMenuAction;
class QPixmap;

class QApplicationWindow : public QMainWindow, public ZLApplicationWindow {
	Q_OBJECT

public:
	QApplicationWindow(ZLApplication *application);

	void fullScreenWorkaround();
	int veritcalAdjustment();

private:
	ZLViewWidget *createViewWidget();

	void addToolbarItem(ZLApplication::Toolbar::ItemPtr item);
	void initMenu();
	class MenuMaskCalculator : public ZLApplication::MenuVisitor {

	public:
		MenuMaskCalculator(QApplicationWindow &window);
		bool shouldBeUpdated();

	private:
		void processSubmenuBeforeItems(ZLApplication::Menubar::Submenu &submenu);
		void processSubmenuAfterItems(ZLApplication::Menubar::Submenu &submenu);
		void processItem(ZLApplication::Menubar::PlainItem &item);
		void processSepartor(ZLApplication::Menubar::Separator &separator);

	private:
		QApplicationWindow &myWindow;
		bool myFirstTime;
		bool myShouldBeUpdated;
		int myCounter;
	};
	class MenuUpdater : public ZLApplication::MenuVisitor {

	public:
		MenuUpdater(QApplicationWindow &window);

	private:
		void processSubmenuBeforeItems(ZLApplication::Menubar::Submenu &submenu);
		void processSubmenuAfterItems(ZLApplication::Menubar::Submenu &submenu);
		void processItem(ZLApplication::Menubar::PlainItem &item);
		void processSepartor(ZLApplication::Menubar::Separator &separator);

	private:
		QApplicationWindow &myWindow;
		std::stack<QPopupMenu*> myMenuStack;
		int myCounter;
	};
	void updateMenu();

	void refresh();
	void close();

	bool isFullKeyboardControlSupported() const;
	void grabAllKeys(bool grab);

	bool isFingerTapEventSupported() const;
	bool isMousePresented() const;
	bool isKeyboardPresented() const;

	void setCaption(const std::string &caption);

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	void focusInEvent(QFocusEvent *event);
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);

	void setToggleButtonState(const ZLApplication::Toolbar::ButtonItem &button);
	void setToolbarItemState(ZLApplication::Toolbar::ItemPtr item, bool visible, bool enabled);

private slots:
	void setDocument(const QString &fileName);

private:
	std::vector<bool> myToolbarMask;
	std::vector<bool> myMenuMask;
	std::map<int,QtMenuAction*> myMenuMap;

	bool myFullScreen;
	int myTitleHeight;

	int myVerticalDelta;

	QMenuBar *myToolBar;
	QPopupMenu *myMenu;

friend class MenuMaskCalculator;
friend class MenuUpdater;
friend class ToolBarButton;
};

class ToolBarButton : public QObject {
	Q_OBJECT

public:	
	ToolBarButton(QApplicationWindow &window, ZLApplication::Toolbar::ButtonItem &button);
	~ToolBarButton();

	QPixmap &pixmap();

	bool toggle();

public slots:
	void doActionSlot();

private:
	QApplicationWindow &myWindow;
	ZLApplication::Toolbar::ButtonItem &myButton;
	QPixmap *myReleasedPixmap;
	QPixmap *myPressedPixmap;
	bool myIsPressed;
};

inline bool QApplicationWindow::MenuMaskCalculator::shouldBeUpdated() { return myShouldBeUpdated; }

#endif /* __QAPPLICATIONWINDOW_H__ */

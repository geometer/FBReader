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

#include <map>

#include <qmainwindow.h>
#include <qaction.h>
#include <qcursor.h>

#include "../../../../core/src/desktop/application/ZLDesktopApplicationWindow.h"

class ZLQtApplicationWindow : public QMainWindow, public ZLDesktopApplicationWindow {
	Q_OBJECT

public:
	ZLQtApplicationWindow(ZLApplication *application);
	~ZLQtApplicationWindow();

private:
	ZLViewWidget *createViewWidget();
	void addToolbarItem(ZLToolbar::ItemPtr item);
	void init();
	void refresh();
	void close();

	void grabAllKeys(bool grab);

	void setCaption(const std::string &caption);

	void setHyperlinkCursor(bool hyperlink);

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event);

	void setToggleButtonState(const ZLToolbar::ButtonItem &button);
	void setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled);

private:
	class QToolBar *myToolBar;

friend class ZLQtToolBarAction;
	std::map<const ZLToolbar::Item*, class ZLQtToolBarAction*> myActions;
	std::map<ZLToolbar::ItemPtr,QWidget*> mySeparatorMap;

	bool myFullScreen;
	bool myWasMaximized;

	bool myCursorIsHyperlink;
	QCursor myStoredCursor;

friend class ZLQtViewWidgetPositionInfo;
};

class ZLQtToolBarAction : public QAction {
	Q_OBJECT

public:
	ZLQtToolBarAction(ZLQtApplicationWindow *parent, ZLToolbar::ButtonItem &item);

private slots:
	void onActivated();

private:
	ZLToolbar::ButtonItem &myItem;
};

#endif /* __ZLQTAPPLICATIONWINDOW_H__ */

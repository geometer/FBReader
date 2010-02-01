/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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
#include <qtoolbutton.h>
#include <qcursor.h>
#include <qlineedit.h>

#include "../../../../core/src/desktop/application/ZLDesktopApplicationWindow.h"

class ZLQtApplicationWindow : public QMainWindow, public ZLDesktopApplicationWindow {
	Q_OBJECT

public:
	ZLQtApplicationWindow(ZLApplication *application);
	~ZLQtApplicationWindow();

	void setFocusToMainWidget();

private:
	ZLViewWidget *createViewWidget();
	void addToolbarItem(ZLToolbar::ItemPtr item);
	void init();
	void processAllEvents();
	void close();

	void grabAllKeys(bool grab);

	void setCaption(const std::string &caption);

	void setHyperlinkCursor(bool hyperlink);

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event);

	void setToggleButtonState(const ZLToolbar::ToggleButtonItem &button);
	void setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled);

private:
	class QToolBar *myToolBar;

friend class ZLQtToolButton;
	std::map<const ZLToolbar::Item*,QWidget*> myItemToWidgetMap;
	std::map<const ZLToolbar::MenuButtonItem*,size_t> myPopupIdMap;

	bool myFullScreen;
	bool myWasMaximized;

	bool myCursorIsHyperlink;
	QCursor myStoredCursor;

friend class ZLQtViewWidgetPositionInfo;

private:
	class LineEditParameter : public QLineEdit, public VisualParameter {

	public:
		LineEditParameter(QToolBar *toolbar, ZLQtApplicationWindow &window, const ZLToolbar::ParameterItem &textFieldItem);

	private:
		void keyReleaseEvent(QKeyEvent *event);

		std::string internalValue() const;
		void internalSetValue(const std::string &value);
		void setValueList(const std::vector<std::string> &values) {}

	private:
		ZLQtApplicationWindow &myWindow;
		const std::string &myActionId;
	};
};

class ZLQtToolButton : public QToolButton {
	Q_OBJECT

public:
	ZLQtToolButton(ZLQtApplicationWindow &window, ZLToolbar::AbstractButtonItem &item);

private slots:
	void onActivated();

private:
	ZLQtApplicationWindow &myWindow;
	ZLToolbar::AbstractButtonItem &myItem;
};

#endif /* __ZLQTAPPLICATIONWINDOW_H__ */

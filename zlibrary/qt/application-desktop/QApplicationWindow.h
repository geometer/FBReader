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

#include <map>

#include <qmainwindow.h>
#include <qaction.h>

#include <abstract/ZLApplication.h>

class QApplicationWindow : public QMainWindow, public ZLApplicationWindow {
	Q_OBJECT

public:
	QApplicationWindow(ZLApplication *application);
	~QApplicationWindow();

private:
	ZLViewWidget *createViewWidget();
	void addToolbarItem(ZLApplication::Toolbar::ItemPtr item);
	void refresh();
	void close();

	bool isFullKeyboardControlSupported() const;
	void grabAllKeys(bool grab);

	bool isFingerTapEventSupported() const;

	void setCaption(const std::string &caption) { QMainWindow::setCaption(QString::fromUtf8(caption.c_str())); }

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);
	void fullScreenWorkaround();

	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event);

private:
	class QToolBar *myToolBar;

	friend class ToolBarAction;
	std::map<ZLApplication::Toolbar::ItemPtr, class ToolBarAction*> myActions;

	ZLIntegerRangeOption myWidthOption;
	ZLIntegerRangeOption myHeightOption;

	bool myFullScreen;
	bool myWasMaximized;
};

class ToolBarAction : public QAction {
	Q_OBJECT

public:
	ToolBarAction(QApplicationWindow *parent, const ZLApplication::Toolbar::ButtonItem &item);

private slots:
	void onActivated();

private:
	const ZLApplication::Toolbar::ButtonItem &myItem;
};


#endif /* __QAPPLICATIONWINDOW_H__ */

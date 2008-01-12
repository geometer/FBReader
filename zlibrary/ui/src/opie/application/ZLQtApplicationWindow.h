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

#include <qmainwindow.h>

#include "../../../../core/src/application/ZLApplicationWindow.h"

class ZLQtApplicationWindow : public QMainWindow, public ZLApplicationWindow { 
	Q_OBJECT

public:
	ZLQtApplicationWindow(ZLApplication *application);
	~ZLQtApplicationWindow();

private:
	ZLViewWidget *createViewWidget();
	void addToolbarItem(ZLApplication::Toolbar::ItemPtr item);
	void refresh();
	void close();

	void grabAllKeys(bool grab);

	void setCaption(const std::string &caption);

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	void timerEvent(QTimerEvent *event);
	void focusInEvent(QFocusEvent *event);
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);

private slots:
	void doActionSlot(int buttonNumber);
	void emptySlot() {}

private:
	bool myFullScreen;
	bool myCloseFlag;
	std::vector<bool> myToolbarMask;
	std::map<std::string,int> myActionIndices;
	std::map<int,std::string> myActionIds;
};

#endif /* __ZLQTAPPLICATIONWINDOW_H__ */

/*
 * FBReader -- electronic book reader
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

#ifndef __QFBREADER_H__
#define __QFBREADER_H__

#include <vector>
#include <map>
#include <set>

#include <qmainwindow.h>

#include "../common/fbreader/FBReader.h"

class QFBReader : public QMainWindow, public FBReader, public ZLApplicationView { 
	Q_OBJECT

public:
	QFBReader(const std::string& bookToOpen);
	~QFBReader();

private:
	void timerEvent(QTimerEvent *event);
	void focusInEvent(QFocusEvent *event);
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);

protected:
	void setWindowCaption(const std::string &caption);
	void setMode(ViewMode mode);
	void addToolbarItem(shared_ptr<Toolbar::Item> item);
	void refresh();
	void searchSlot();
	void toggleFullscreenSlot();
	bool isFullscreen() const;
	void quitSlot();

	bool isFullKeyboardControlSupported() const;
	void grabAllKeys(bool grab);

private slots:
	void doActionSlot(int buttonNumber);
	void emptySlot() {}

private:
	bool myFullScreen;
	bool myCloseFlag;
};

#endif /* __QFBREADER_H__ */

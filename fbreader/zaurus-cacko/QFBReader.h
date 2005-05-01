/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __QFBREADER_H__
#define __QFBREADER_H__

#include <vector>
#include <map>
#include <set>

#include <qmainwindow.h>

#include "../common/fbreader/FBReader.h"

class QFBReader : public QMainWindow, public FBReader { 
	Q_OBJECT

public:
	QFBReader();

	void fullScreenWorkaround();

private:
	void focusInEvent(QFocusEvent *event);
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);

protected:
	void setWindowCaption(const std::string &caption);
	void setMode(ViewMode mode);
	void addButton(ActionCode id, const std::string &name);
	void setButtonEnabled(ActionCode id, bool enable);
	void setButtonVisible(ActionCode id, bool visible);
	void cancelSlot();
	void searchSlot();
	void fullscreenSlot();
	bool isRotationSupported() const { return false; }

private slots:
	void doActionSlot(int buttonNumber);
	void emptySlot() {}

private:
	bool myFullScreen;
	int myTitleHeight;

	std::map<int,ActionCode> myKeyBindings;
	
	struct ButtonInfo {
		ActionCode Code;
		bool IsVisible;
		QPixmap *Pixmap;
	};
	std::vector<ButtonInfo> myButtons;
};

#endif /* __QFBREADER_H__ */

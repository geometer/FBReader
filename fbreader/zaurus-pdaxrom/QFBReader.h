/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __QFBREADER_H__
#define __QFBREADER_H__

#include <map>

#include <qmainwindow.h>

#include "../common/fbreader/FBReader.h"

class QFBReader : public QMainWindow, public FBReader { 
	Q_OBJECT

private:
	static std::string ImageDirectory;
	static QPixmap loadPixmap(const std::string &name);
	
public:
	QFBReader();
	~QFBReader();

	void enableMenuButtons();

private:
	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);

protected:
	void setWindowCaption(const std::string &caption);
	void setMode(ViewMode mode);

private:
	void cancelSlot();
	void fullscreenSlot();
	
private slots:
	void showCollectionSlot() { doAction(ACTION_SHOW_COLLECTION); }
	void showContentsSlot() { doAction(ACTION_SHOW_CONTENTS); }
	void showOptionsDialogSlot() { doAction(ACTION_SHOW_OPTIONS); }
	void undoSlot() { doAction(ACTION_UNDO); }
	void redoSlot() { doAction(ACTION_REDO); }
	void searchSlot();
	void findNextSlot() { doAction(ACTION_FIND_NEXT); }
	void findPreviousSlot() { doAction(ACTION_FIND_PREVIOUS); }

private:
	bool myFullScreen;
	bool myWasMaximized;

	int myBooksItemId;
	int mySettingsItemId;
	int myUndoItemId;
	int myRedoItemId;
	int myContentsItemId;
	int mySearchItemId;
	int myFindNextId;
	int myFindPreviousId;

	std::map<int,ActionCode> myKeyBindings;
};

#endif /* __QFBREADER_H__ */

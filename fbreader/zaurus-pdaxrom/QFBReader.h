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

#include <qmainwindow.h>
#include <qdatetime.h>

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

protected:
	void setWindowCaption(const std::string &caption) { setCaption(QString::fromUtf8(caption.c_str())); }
	void repaintView();
	void setMode(ViewMode mode);

private slots:
	void showCollectionSlot() { setMode(BOOK_COLLECTION_MODE); }
	void showContentsSlot() { setMode(CONTENTS_MODE); }
	void showOptionsDialogSlot();
	void undoSlot() { undoPage(); }
	void redoSlot() { redoPage(); }
	void increaseFontSlot() { increaseFont(); }
	void decreaseFontSlot() { decreaseFont(); }
	void scrollForwardSlot();
	void scrollBackwardSlot();
	void doFullScreenSlot();
	void cancelSlot();
	void searchSlot();
	void findNextSlot() { findNext(); }
	void findPreviousSlot() { findPrevious(); }

private:
	bool myFullScreen;
	bool myWasMaximized;
	QTime myLastScrollingTime;

	int myUndoItemId;
	int myRedoItemId;
	int myContentsItemId;
	int myFindNextId;
	int myFindPreviousId;
};

#endif /* __QFBREADER_H__ */

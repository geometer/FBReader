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

#include <qapplication.h>
#include <qpixmap.h>
#include <qmenubar.h>
#include <qaccel.h>
#include <qpe/resource.h>

#include <qlayout.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qdialog.h>

#include <abstract/ZLOptions.h>

#include "../common/description/BookDescription.h"
#include "../common/fbreader/BookTextView.h"
#include "../common/fbreader/FootnoteView.h"
#include "../common/fbreader/ContentsView.h"
#include "../common/fbreader/CollectionView.h"
#include "QFBReader.h"
#include "QViewWidget.h"
#include "QPaintContext.h"

QFBReader::QFBReader() : FBReader(new QPaintContext()) {
	setWFlags(getWFlags() | WStyle_Customize);

	myViewWidget = new QViewWidget(this);
	setCentralWidget((QViewWidget*)myViewWidget);

	setMode(BOOK_TEXT_MODE);

	QAccel *accelerator = new QAccel(this);
	accelerator->connectItem(accelerator->insertItem(Key_L), this, SLOT(showCollectionSlot()));
	accelerator->connectItem(accelerator->insertItem(Key_C), this, SLOT(showContentsSlot()));
	accelerator->connectItem(accelerator->insertItem(Key_F), this, SLOT(searchSlot()));
	accelerator->connectItem(accelerator->insertItem(Key_N), this, SLOT(findNextSlot()));
	accelerator->connectItem(accelerator->insertItem(Key_P), this, SLOT(findPreviousSlot()));
	accelerator->connectItem(accelerator->insertItem(Key_O), this, SLOT(showOptionsDialogSlot()));
	accelerator->connectItem(accelerator->insertItem(Key_I), this, SLOT(showHidePositionIndicatorSlot()));
	accelerator->connectItem(accelerator->insertItem(0x200f), this, SLOT(decreaseFontSlot()));
	accelerator->connectItem(accelerator->insertItem(0x2010), this, SLOT(increaseFontSlot()));
	accelerator->connectItem(accelerator->insertItem(Key_Left), this, SLOT(undoSlot()));
	accelerator->connectItem(accelerator->insertItem(Key_Right), this, SLOT(redoSlot()));
	accelerator->connectItem(accelerator->insertItem(Key_Up), this, SLOT(scrollBackwardSlot()));
	accelerator->connectItem(accelerator->insertItem(Key_Down), this, SLOT(scrollForwardSlot()));
	accelerator->connectItem(accelerator->insertItem(Key_Escape), this, SLOT(cancelSlot()));
	accelerator->connectItem(accelerator->insertItem(Key_Return), this, SLOT(doFullScreenSlot()));

	myFullScreen = false;
	myTitleHeight = -1;
}

void QFBReader::focusInEvent(QFocusEvent*) {
	if (myFullScreen && (size() != qApp->desktop()->size())) {
		showNormal();
		showFullScreen();
	}
}

void QFBReader::resizeEvent(QResizeEvent*) {
	if (myFullScreen && (size() != qApp->desktop()->size())) {
		int titleHeight = topData()->normalGeometry.top();
		if (titleHeight > 0) {
			myTitleHeight = titleHeight;
		}
		topData()->normalGeometry = QRect(0, 0, -1, -1);
		showNormal();
		showFullScreen();
	}
}

void QFBReader::doFullScreenSlot() {
	if (!myFullScreen) {
		myFullScreen = true;
		menuBar()->hide();
		showFullScreen();
	}
}

void QFBReader::cancelSlot() {
	if (myFullScreen) {
		myFullScreen = false;
		menuBar()->show();
		showNormal();
		if (myTitleHeight > 0) {
			move(1, myTitleHeight);
			myTitleHeight = -1;
		}
		setWFlags(getWFlags() | WStyle_Customize);
	} else if (QuitOnCancelOption.value() || (mode() != BOOK_TEXT_MODE)) {
		close();
	}
}

void QFBReader::setMode(ViewMode mode) {
	if (mode == myMode) {
		return;
	}
	myPreviousMode = myMode;
	myMode = mode;

	centralWidget()->hide();
	menuBar()->clear();
	switch (myMode) {
		case BOOK_TEXT_MODE:
			menuBar()->insertItem(Resource::loadPixmap("FBReader/books"), this, SLOT(showCollectionSlot()));
			menuBar()->insertItem(Resource::loadPixmap("FBReader/settings"), this, SLOT(showOptionsDialogSlot()));
			myUndoItemId = menuBar()->insertItem(Resource::loadPixmap("FBReader/leftarrow"), this, SLOT(undoSlot()));
			myRedoItemId = menuBar()->insertItem(Resource::loadPixmap("FBReader/rightarrow"), this, SLOT(redoSlot()));
			myContentsItemId = menuBar()->insertItem(Resource::loadPixmap("FBReader/contents"), this, SLOT(showContentsSlot()));
			menuBar()->insertItem(Resource::loadPixmap("FBReader/find"), this, SLOT(searchSlot()));
			myFindPreviousId = menuBar()->insertItem(Resource::loadPixmap("FBReader/findprev"), this, SLOT(findPreviousSlot()));
			myFindNextId = menuBar()->insertItem(Resource::loadPixmap("FBReader/findnext"), this, SLOT(findNextSlot()));
			myViewWidget->setView(myBookTextView);
			break;
		case CONTENTS_MODE:
			menuBar()->insertItem(Resource::loadPixmap("FBReader/books"), this, SLOT(showCollectionSlot()));
			menuBar()->insertItem(Resource::loadPixmap("FBReader/settings"), this, SLOT(showOptionsDialogSlot()));
			menuBar()->insertItem(Resource::loadPixmap("FBReader/find"), this, SLOT(searchSlot()));
			myFindPreviousId = menuBar()->insertItem(Resource::loadPixmap("FBReader/findprev"), this, SLOT(findPreviousSlot()));
			myFindNextId = menuBar()->insertItem(Resource::loadPixmap("FBReader/findnext"), this, SLOT(findNextSlot()));
			myViewWidget->setView(myContentsView);
			break;
		case FOOTNOTE_MODE:
			menuBar()->insertItem(Resource::loadPixmap("FBReader/settings"), this, SLOT(showOptionsDialogSlot()));
			myContentsItemId = menuBar()->insertItem(Resource::loadPixmap("FBReader/contents"), this, SLOT(showContentsSlot()));
			menuBar()->insertItem(Resource::loadPixmap("FBReader/find"), this, SLOT(searchSlot()));
			myFindPreviousId = menuBar()->insertItem(Resource::loadPixmap("FBReader/findprev"), this, SLOT(findPreviousSlot()));
			myFindNextId = menuBar()->insertItem(Resource::loadPixmap("FBReader/findnext"), this, SLOT(findNextSlot()));
			myViewWidget->setView(myFootnoteView);
			break;
		case BOOK_COLLECTION_MODE:
			menuBar()->insertItem(Resource::loadPixmap("FBReader/settings"), this, SLOT(showOptionsDialogSlot()));
			menuBar()->insertItem(Resource::loadPixmap("FBReader/find"), this, SLOT(searchSlot()));
			myFindPreviousId = menuBar()->insertItem(Resource::loadPixmap("FBReader/findprev"), this, SLOT(findPreviousSlot()));
			myFindNextId = menuBar()->insertItem(Resource::loadPixmap("FBReader/findnext"), this, SLOT(findNextSlot()));
			myCollectionView->fill();
			myViewWidget->setView(myCollectionView);
			break;
		case BOOKMARKS_MODE:
			break;
		case UNDEFINED_MODE:
			break;
	}
	setWindowCaption("FBReader - " + myViewWidget->view()->caption());
	centralWidget()->show();
}

void QFBReader::closeEvent(QCloseEvent *event) {
	if (mode() != BOOK_TEXT_MODE) {
		restorePreviousMode();
		event->ignore();
	} else {
		event->accept();
	}
}

void QFBReader::enableMenuButtons() {
	if (menuBar()->count() == 0) {
		return;
	}

	switch (mode()) {
		case BOOK_TEXT_MODE:
			menuBar()->setItemEnabled(myUndoItemId, myBookTextView->canUndoPageMove());
			menuBar()->setItemEnabled(myRedoItemId, myBookTextView->canRedoPageMove());
			menuBar()->setItemEnabled(myFindNextId, myBookTextView->canFindNext());
			menuBar()->setItemEnabled(myFindPreviousId, myBookTextView->canFindPrevious());
			menuBar()->setItemEnabled(myContentsItemId, !myContentsView->isEmpty());
			break;
		case CONTENTS_MODE:
			menuBar()->setItemEnabled(myFindNextId, myContentsView->canFindNext());
			menuBar()->setItemEnabled(myFindPreviousId, myContentsView->canFindPrevious());
			break;
		case FOOTNOTE_MODE:
			menuBar()->setItemEnabled(myFindNextId, myFootnoteView->canFindNext());
			menuBar()->setItemEnabled(myFindPreviousId, myFootnoteView->canFindPrevious());
			menuBar()->setItemEnabled(myContentsItemId, !myContentsView->isEmpty());
			break;
		case BOOK_COLLECTION_MODE:
			menuBar()->setItemEnabled(myFindNextId, myCollectionView->canFindNext());
			menuBar()->setItemEnabled(myFindPreviousId, myCollectionView->canFindPrevious());
			break;
		case BOOKMARKS_MODE:
			break;
		case UNDEFINED_MODE:
			break;
	}
}

void QFBReader::searchSlot() {
	QDialog findDialog(this, 0, true);
	findDialog.setCaption("Text search");
	findDialog.setSizeGripEnabled(true);
    
	QGridLayout *layout = new QGridLayout(&findDialog, -1, 3, 5, 5);

	QLineEdit *wordToSearch = new QLineEdit(&findDialog);
	wordToSearch->setMinimumWidth(width() / 3);
	wordToSearch->setText(QString::fromUtf8(SearchPatternOption.value().c_str()));
	layout->addMultiCellWidget(wordToSearch, 0, 0, 0, 2);

	QCheckBox *ignoreCase = new QCheckBox("&Ignore case", &findDialog, 0);
	ignoreCase->setChecked(SearchIgnoreCaseOption.value());
	layout->addMultiCellWidget(ignoreCase, 1, 1, 0, 2);

	QCheckBox *wholeText = new QCheckBox("In w&hole text", &findDialog, 0);
	wholeText->setChecked(SearchInWholeTextOption.value());
	layout->addMultiCellWidget(wholeText, 2, 2, 0, 2);

	QCheckBox *backward = new QCheckBox("&Backward", &findDialog, 0);
	backward->setChecked(SearchBackwardOption.value());
	layout->addMultiCellWidget(backward, 3, 3, 0, 2);

	QPushButton *b = new QPushButton("&Go!", &findDialog);
	layout->addWidget(b, 4, 1);
	b->setDefault(true);
	connect(b, SIGNAL(clicked()), &findDialog, SLOT(accept()));

	if (findDialog.exec()) {
		QString qPattern = wordToSearch->text().stripWhiteSpace();
		std::string pattern = (const char*)qPattern.utf8();
		SearchPatternOption.setValue(pattern);
		SearchIgnoreCaseOption.setValue(ignoreCase->isChecked());
		SearchInWholeTextOption.setValue(wholeText->isChecked());
		SearchBackwardOption.setValue(backward->isChecked());
		((TextView*)myViewWidget->view())->search(
			pattern, ignoreCase->isChecked(), wholeText->isChecked(), backward->isChecked()
		);
	}

	enableMenuButtons();
	fullScreenWorkaround();
}

void QFBReader::fullScreenWorkaround() {
	if (myFullScreen) {
		reparent(0, WType_TopLevel, QPoint(0,0));
		reparent(0, WType_TopLevel | WStyle_Customize | WStyle_NoBorderEx, QPoint(0,0));
		show();
	}
}

void QFBReader::setWindowCaption(const std::string &caption) {
	QString qCaption = QString::fromUtf8(caption.c_str());
	if (qCaption.length() > 60) {
		qCaption = qCaption.left(57) + "...";
	}
	setCaption(qCaption);
}

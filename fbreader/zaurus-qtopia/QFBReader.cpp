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
#include <qpe/resource.h>

#include <qlayout.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qdialog.h>

#include <abstract/ZLOptions.h>

#include <qtopia/FullScreenDialog.h>

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

	myKeyBindings[Key_L] = ACTION_SHOW_COLLECTION;
	myKeyBindings[Key_C] = ACTION_SHOW_CONTENTS;
	myKeyBindings[Key_F] = ACTION_SEARCH;
	myKeyBindings[Key_N] = ACTION_FIND_NEXT;
	myKeyBindings[Key_P] = ACTION_FIND_PREVIOUS;
	myKeyBindings[Key_O] = ACTION_SHOW_OPTIONS;
	myKeyBindings[Key_I] = ACTION_SHOW_BOOK_INFO;
	myKeyBindings[Key_D] = ACTION_SHOW_HIDE_POSITION_INDICATOR;
	myKeyBindings[Key_A] = ACTION_ADD_BOOK;
	myKeyBindings[0x200f] = ACTION_DECREASE_FONT;
	myKeyBindings[0x2010] = ACTION_INCREASE_FONT;
	myKeyBindings[Key_Left] = ACTION_UNDO;
	myKeyBindings[Key_Right] = ACTION_REDO;
	myKeyBindings[Key_Up] = ACTION_SCROLL_BACKWARD;
	myKeyBindings[Key_Down] = ACTION_SCROLL_FORWARD;
	myKeyBindings[Key_Escape] = ACTION_CANCEL;
	myKeyBindings[Key_Return] = ACTION_FULLSCREEN;

	myFullScreen = false;
	myTitleHeight = -1;

	createToolbar();
	connect(menuBar(), SIGNAL(activated(int)), this, SLOT(doActionSlot(int)));
	setMode(BOOK_TEXT_MODE);
}

void QFBReader::keyPressEvent(QKeyEvent *event) {
	std::map<int,ActionCode>::const_iterator it = myKeyBindings.find(event->key());
	if (it != myKeyBindings.end()) {
		doAction(it->second);
	}
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

void QFBReader::fullscreenSlot() {
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
	} else if (QuitOnCancelOption.value() || (myMode != BOOK_TEXT_MODE)) {
		close();
	}
}

void QFBReader::setMode(ViewMode mode) {
	if (mode == myMode) {
		return;
	}

	centralWidget()->hide();

	FBReader::setMode(mode);

	menuBar()->clear();
	for (std::vector<ButtonInfo>::const_iterator it = myButtons.begin(); it != myButtons.end(); it++) {
		if (it->IsVisible) {
			menuBar()->insertItem(*it->Pixmap, this, SLOT(emptySlot()), 0, it->Code);
		}
	}
	centralWidget()->show();
	enableMenuButtons();
}

void QFBReader::closeEvent(QCloseEvent *event) {
	if (myMode != BOOK_TEXT_MODE) {
		restorePreviousMode();
		event->ignore();
	} else {
		event->accept();
	}
}

void QFBReader::addButton(ActionCode id, const std::string &name) {
	myButtons.push_back(ButtonInfo());
	myButtons.back().Code = id;
	myButtons.back().IsVisible = true;
	myButtons.back().Pixmap = new QPixmap(Resource::loadPixmap(("FBReader/" + name).c_str()));
}

void QFBReader::setButtonEnabled(ActionCode id, bool enable) {
	if (menuBar()->findItem(id) != 0) {
		menuBar()->setItemEnabled(id, enable);
	}
}

void QFBReader::setButtonVisible(ActionCode id, bool visible) {
	for (std::vector<ButtonInfo>::iterator it = myButtons.begin(); it != myButtons.end(); it++) {
		if (it->Code == id) {
			it->IsVisible = visible;
			break;
		}
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

class InfoDialog : public FullScreenDialog {

public:
	InfoDialog();
	~InfoDialog() {}
};

InfoDialog::InfoDialog() : FullScreenDialog("FBReader -- Book Info") {
}

void QFBReader::doActionSlot(int buttonNumber) {
	doAction((ActionCode)buttonNumber);
}

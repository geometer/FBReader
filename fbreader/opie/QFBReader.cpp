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

#include <qpe/qpeapplication.h>
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
#include <qtopia/QViewWidget.h>
#include <qtopia/QPaintContext.h>
#include <qtopia/QKeyUtil.h>

#include "../common/description/BookDescription.h"
#include "../common/fbreader/BookTextView.h"
#include "../common/fbreader/FootnoteView.h"
#include "../common/fbreader/ContentsView.h"
#include "../common/fbreader/CollectionView.h"
#include "QFBReader.h"

QFBReader::QFBReader(const std::string& bookToOpen) : FBReader(new QPaintContext(), bookToOpen, false), myCloseFlag(false) {
	if (KeyboardControlOption.value()) {
		grabAllKeys(true);
	}
	myViewWidget = new QViewWidget(this, this, (ZLViewWidget::Angle)AngleStateOption.value());
	setCentralWidget((QViewWidget*)myViewWidget);

	myFullScreen = false;

	connect(menuBar(), SIGNAL(activated(int)), this, SLOT(doActionSlot(int)));
	setMode(BOOK_TEXT_MODE);
}

QFBReader::~QFBReader() {
	if (KeyboardControlOption.value()) {
		grabAllKeys(false);
	}
}

void QFBReader::keyPressEvent(QKeyEvent *event) {
	killTimers();
	doAction(QKeyUtil::keyName(event));
}

void QFBReader::focusInEvent(QFocusEvent*) {
	if (myFullScreen && (size() != qApp->desktop()->size())) {
		showNormal();
		showFullScreen();
	}
}

void QFBReader::resizeEvent(QResizeEvent*) {
	if (myFullScreen && (size() != qApp->desktop()->size())) {
		showNormal();
		showFullScreen();
	}
}

void QFBReader::toggleFullscreenSlot() {
	myFullScreen = !myFullScreen;
	if (myFullScreen) {
		menuBar()->hide();
		showNormal();
		showFullScreen();
	} else {
		menuBar()->show();
		showNormal();
		showMaximized();
	}
}

bool QFBReader::isFullscreen() const {
	return myFullScreen;
}

void QFBReader::quitSlot() {
	myCloseFlag = true;
  close();
}

void QFBReader::setMode(ViewMode mode) {
	if (mode == myMode) {
		return;
	}

	FBReader::setMode(mode);

	enableMenuButtons();
}

void QFBReader::timerEvent(QTimerEvent *) {
	if (myMode != BOOK_TEXT_MODE) {
		restorePreviousMode();
	} else {
		myCloseFlag = true;
		close();
	}
	killTimers();
}

void QFBReader::closeEvent(QCloseEvent *event) {
	if (myCloseFlag) {
		event->accept();
	} else {
		startTimer(50);
		event->ignore();
	}
}

void QFBReader::addToolbarItem(shared_ptr<Toolbar::Item>) {
}

void QFBReader::refresh() {
	const Toolbar::ItemVector &items = toolbar().items();
	if (toolbar().isVisibilityChanged()) {
		centralWidget()->hide();
		menuBar()->clear();
		for (Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
			if ((*it)->isButton()) {
				Toolbar::ButtonItem &buttonItem = (Toolbar::ButtonItem&)**it;
				if (buttonItem.isVisible()) {
					const QPixmap &pixmap = Resource::loadPixmap(("FBReader/" + buttonItem.iconName()).c_str());
					menuBar()->insertItem(pixmap, this, SLOT(emptySlot()), 0, (ActionCode)buttonItem.actionId());
				}
			}
		}
		centralWidget()->show();
	}
	for (Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->isButton()) {
			const Toolbar::ButtonItem &button = (const Toolbar::ButtonItem&)**it;
			int id = button.actionId();
			if (menuBar()->findItem(id) != 0) {
				menuBar()->setItemEnabled(id, button.isEnabled());
			}
		}
	}
	toolbar().reset();
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

	QCheckBox *thisSectionOnly = ((TextView*)myViewWidget->view())->hasMultiSectionModel() ?
		new QCheckBox("&This section only", &findDialog, 0) : 0;
	if (thisSectionOnly != 0) {
		thisSectionOnly->setChecked(SearchThisSectionOnlyOption.value());
		layout->addMultiCellWidget(thisSectionOnly, 4, 4, 0, 2);
	}

	QPushButton *b = new QPushButton("&Go!", &findDialog);
	layout->addWidget(b, 5, 1);
	b->setDefault(true);
	connect(b, SIGNAL(clicked()), &findDialog, SLOT(accept()));

	if (findDialog.exec()) {
		QString qPattern = wordToSearch->text().stripWhiteSpace();
		std::string pattern = (const char*)qPattern.utf8();
		SearchPatternOption.setValue(pattern);
		SearchIgnoreCaseOption.setValue(ignoreCase->isChecked());
		SearchInWholeTextOption.setValue(wholeText->isChecked());
		SearchBackwardOption.setValue(backward->isChecked());
		if (thisSectionOnly != 0) {
			SearchThisSectionOnlyOption.setValue(thisSectionOnly->isChecked());
		}
		((TextView*)myViewWidget->view())->search(
			pattern, ignoreCase->isChecked(), wholeText->isChecked(), backward->isChecked(), SearchThisSectionOnlyOption.value()
		);
	}
	repaintView();
	enableMenuButtons();
}

void QFBReader::setWindowCaption(const std::string &caption) {
	QString qCaption = QString::fromUtf8(caption.c_str());
	if (qCaption.length() > 60) {
		qCaption = qCaption.left(57) + "...";
	}
	setCaption(qCaption);
}

void QFBReader::doActionSlot(int buttonNumber) {
	doAction((ActionCode)buttonNumber);
}

bool QFBReader::isFullKeyboardControlSupported() const {
	return true;
}

void QFBReader::grabAllKeys(bool grab) {
	if (grab) {
		QPEApplication::grabKeyboard();
	} else {
		QPEApplication::ungrabKeyboard();
	}
}

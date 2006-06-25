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

QFBReader::QFBReader(const std::string& bookToOpen) : FBReader(new QPaintContext(), bookToOpen), myCloseFlag(false) {
	if (KeyboardControlOption.value()) {
		grabAllKeys(true);
	}
	myViewWidget = new QViewWidget(this, this, (ZLViewWidget::Angle)AngleStateOption.value());
	setCentralWidget((QViewWidget*)myViewWidget);

	myFullScreen = false;

	connect(menuBar(), SIGNAL(activated(int)), this, SLOT(doActionSlot(int)));
	initWindow(this);
	setMode(BOOK_TEXT_MODE);
}

QFBReader::~QFBReader() {
	if (KeyboardControlOption.value()) {
		grabAllKeys(false);
	}
}

void QFBReader::keyPressEvent(QKeyEvent *event) {
	killTimers();
	doActionByKey(QKeyUtil::keyName(event));
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

	bool isVisibilityChanged = false;
	if (myToolbarMask.size() != items.size()) {
		isVisibilityChanged = true;
		myToolbarMask.clear();
		myToolbarMask.assign(items.size(), false);
	}
	std::vector<bool>::iterator bt = myToolbarMask.begin();
	for (Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->isButton()) {
			const Toolbar::ButtonItem &button = (Toolbar::ButtonItem&)**it;
			if (application().isActionVisible(button.actionId()) != *bt) {
				*bt = !*bt;
				isVisibilityChanged = true;
			}
			++bt;
		}
	}
	if (isVisibilityChanged) {
		bt = myToolbarMask.begin();
		centralWidget()->hide();
		menuBar()->clear();
		for (Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
			if ((*it)->isButton()) {
				const Toolbar::ButtonItem &button = (Toolbar::ButtonItem&)**it;
				if (*bt) {
					const QPixmap &pixmap = Resource::loadPixmap(("FBReader/" + button.iconName()).c_str());
					menuBar()->insertItem(pixmap, this, SLOT(emptySlot()), 0, button.actionId());
				}
				++bt;
			}
		}
		centralWidget()->show();
	}

	for (Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->isButton()) {
			const Toolbar::ButtonItem &button = (const Toolbar::ButtonItem&)**it;
			int id = button.actionId();
			if (menuBar()->findItem(id) != 0) {
				menuBar()->setItemEnabled(id, application().isActionEnabled(id));
			}
		}
	}
}

void QFBReader::setCaption(const std::string &caption) {
	QString qCaption = QString::fromUtf8(caption.c_str());
	if (qCaption.length() > 60) {
		qCaption = qCaption.left(57) + "...";
	}
	QMainWindow::setCaption(qCaption);
}

void QFBReader::doActionSlot(int buttonNumber) {
	doAction(buttonNumber);
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

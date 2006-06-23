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

#include <qapplication.h>
#include <qpixmap.h>
#include <qmenubar.h>

#include <qlayout.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qdialog.h>

#include <abstract/ZLOptions.h>

#include <qt/QViewWidget.h>
#include <qt/QPaintContext.h>
#include <qt/QKeyUtil.h>

#include "../../common/description/BookDescription.h"
#include "../../common/fbreader/BookTextView.h"
#include "../../common/fbreader/FootnoteView.h"
#include "../../common/fbreader/ContentsView.h"
#include "../../common/fbreader/CollectionView.h"
#include "QFBReader.h"

static const std::string OPTIONS = "Options";

QFBReader::QFBReader(const std::string& bookToOpen) :
	FBReader(new QPaintContext(), bookToOpen),
	myWidthOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Width", 10, 800, 350),
	myHeightOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "Height", 10, 800, 350),
	myFullScreen(false),
	myWasMaximized(false) {

	setWFlags(getWFlags() | WStyle_Customize);

	myViewWidget = new QViewWidget(this, this, (ZLViewWidget::Angle)AngleStateOption.value());
	setCentralWidget((QViewWidget*)myViewWidget);

	connect(menuBar(), SIGNAL(activated(int)), this, SLOT(doActionSlot(int)));

	resize(myWidthOption.value(), myHeightOption.value());

	initWindow(this);

	setMode(BOOK_TEXT_MODE);
}

QFBReader::~QFBReader() {
	if (!isFullscreen()) {
		myWidthOption.setValue(width());
		myHeightOption.setValue(height());
	}
}

void QFBReader::keyPressEvent(QKeyEvent *event) {
	doActionByKey(QKeyUtil::keyName(event));
}

void QFBReader::toggleFullscreenSlot() {
	myFullScreen = !myFullScreen;
	if (myFullScreen) {
		myWasMaximized = isMaximized();
		menuBar()->hide();
		showFullScreen();
	} else {
		menuBar()->show();
		showNormal();
		if (myWasMaximized) {
			showMaximized();
		}
	}
}

bool QFBReader::isFullscreen() const {
	return myFullScreen;
}

void QFBReader::quitSlot() {
	close();
}

void QFBReader::closeEvent(QCloseEvent *event) {
	if (myMode != BOOK_TEXT_MODE) {
		restorePreviousMode();
		event->ignore();
	} else {
		event->accept();
	}
}

void QFBReader::addToolbarItem(Toolbar::ItemPtr item) {
	if (item->isButton()) {
		const Toolbar::ButtonItem &buttonItem = (const Toolbar::ButtonItem&)*item;
		menuBar()->insertItem(QPixmap((ImageDirectory + "/FBReader/" + buttonItem.iconName() + ".png").c_str()), this, SLOT(emptySlot()), 0, buttonItem.actionId());
	} else {
		// TODO: implement
	}
}

void QFBReader::refresh() {
	const Toolbar::ItemVector &items = toolbar().items();
	for (Toolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		if ((*it)->isButton()) {
			const Toolbar::ButtonItem &button = (const Toolbar::ButtonItem&)**it;
			int id = button.actionId();
			if (menuBar()->findItem(id) != 0) {
				menuBar()->setItemVisible(id, application().isActionVisible(id));
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

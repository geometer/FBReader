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

#include "ZLFullScreenDialog.h"
#include "ZLQtUtil.h"

#include <zapplication.h>
#include <zglobal.h>
#include <ezxutilcst.h>
#include <qwallpaper.h>

#include "../../qt/util/ZLQtKeyUtil.h"

ZLFullScreenDialog::ZLFullScreenDialog(const std::string &caption) : ZMainWidget(::qtString(caption), false, qApp->mainWidget(), NULL, WType_Modal | WType_TopLevel) {
	QWallpaper::setAppWallpaperMode(QWallpaper::UseSysWallpaper);
	QFont f(qApp->font());
	f.setPointSize(15);
	setFont(f);
	getTitleBarWidget()->setFont(f);
	myInLoop = false;
	UTIL_CST *cst = new UTIL_CST(this, ::qtButtonName(ZLResourceKey("ok")), "CST_Menu", "CST_Back");
	setCSTWidget(cst);
	connect(cst->getRightBtn(), SIGNAL(clicked()), this, SLOT(reject()));
	connect(cst->getMidBtn(), SIGNAL(clicked()), this, SLOT(accept()));
	cst->getLeftBtn()->setEnabled(false);
	cst->getLeftBtn()->setAccel(Key_F1);
}

ZLFullScreenDialog::~ZLFullScreenDialog() {
	QWallpaper::setAppWallpaperMode(QWallpaper::Off);
}

void ZLFullScreenDialog::resizeEvent(QResizeEvent * /* event */) {
}

ZLFullScreenDialog::DialogCode ZLFullScreenDialog::exec() {
	setResultCode(Rejected);
	show();
	myInLoop = true;
	qApp->enter_loop();
	return result();
}

void ZLFullScreenDialog::accept() {
	hide();
	if (myInLoop) {
		qApp->exit_loop();
	}
	setResultCode(Accepted);
	if (qApp->mainWidget() == this) {
		qApp->quit();
	}
}

void ZLFullScreenDialog::reject() {
	hide();
	if (myInLoop) {
		qApp->exit_loop();
	}
	setResultCode(Rejected);
	if (qApp->mainWidget() == this) {
		qApp->quit();
	}
}

void ZLFullScreenDialog::keyPressEvent(QKeyEvent *event) {
	if ((event == 0)) return;
	// if (ZLQtKeyUtil::keyName(event) == "<Cancel>") {
	if (event->key() == Key_F2) {
		reject();
	} else if (event->key() == Key_Enter || event->key() == Key_Return) {
		accept();
	} else {
		event->ignore();
	}
}

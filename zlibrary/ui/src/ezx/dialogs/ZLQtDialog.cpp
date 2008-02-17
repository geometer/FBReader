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

#include <zbasedialog.h>
#include <qvbox.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qstring.h>
#include <qscrollview.h>
#include <zglobal.h>
#include <ezxutilgraph.h>
#include <ezxutilpushbutton.h>
#include <zapplication.h>

#include "ZLQtDialog.h"
#include "ZLQtDialogContent.h"
#include "ZLQtDialogManager.h"
#include "ZLQtUtil.h"

ZLQtDialog::ZLQtDialog(QWidget *parent, const ZLResource &resource) : ZBaseDialog(parent, 0, true), myButtonNumber(0) {
	QFont f(qApp->font());
	f.setPointSize(15);
	setFont(f);

	QVBoxLayout *layout = new QVBoxLayout(this);
	QWidget *widget = new QVBox(this);
	layout->add(widget);
	
	QFrame *frmTitle = new QFrame(widget);
	frmTitle->setFixedHeight(25);
	QHBoxLayout *hblTitle = new QHBoxLayout(frmTitle);
	QLabel *lblTitle = new QLabel(::qtString("  " + resource[ZLDialogManager::DIALOG_TITLE].value()), frmTitle);
	hblTitle->addWidget(lblTitle);
	UTIL_Graph::makeTitle(lblTitle, 1);
	
	myTab = new ZLQtDialogContent(widget, resource);

	myButtonGroup = new QButtonGroup(this);
	layout->add(myButtonGroup);

	myButtonLayout = new QGridLayout(myButtonGroup, 1, 0, 0, 0);
}

ZLQtDialog::~ZLQtDialog() {
}

void ZLQtDialog::addButton(const QString &name, bool accept) {
	UTIL_PushButton *button = new UTIL_PushButton(myButtonGroup, 0, -1, -1);
	button->resize(ZGlobal::getDlgB2_1R().size());
	button->setText(name);
	myButtonLayout->addWidget(button, 0, myButtonNumber++);
	connect(button, SIGNAL(clicked()), this, accept ? SLOT(accept()) : SLOT(reject()));
}

void ZLQtDialog::addButton(const ZLResourceKey &key, bool accept) {
	addButton(::qtButtonName(key), accept);
}

bool ZLQtDialog::run() {
	((ZLQtDialogContent*)myTab)->close();
	return exec();
}

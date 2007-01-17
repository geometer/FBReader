/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <qdialog.h>
#include <qvbox.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlayout.h>

#include "ZLQtDialog.h"
#include "ZLQtDialogContent.h"
#include "ZLQtDialogManager.h"

ZLQtDialog::ZLQtDialog(const std::string &title) : QDialog(0, 0, true), myButtonNumber(0) {
	setCaption(title.c_str());

	QVBoxLayout *layout = new QVBoxLayout(this);
	QWidget *widget = new QVBox(this);
	layout->add(widget);
	myTab = new ZLQtDialogContent(widget);

	myButtonGroup = new QButtonGroup(this);
	layout->add(myButtonGroup);
	myButtonLayout = new QGridLayout(myButtonGroup, 1, 0, 8, 8);
}

ZLQtDialog::~ZLQtDialog() {
}

void ZLQtDialog::addButton(const std::string &text, bool accept) {
	QPushButton *button = new QPushButton(myButtonGroup);
	button->setText(text.c_str());
	myButtonLayout->addWidget(button, 0, myButtonNumber++);
	connect(button, SIGNAL(clicked()), this, accept ? SLOT(accept()) : SLOT(reject()));
}

bool ZLQtDialog::run() {
	((ZLQtDialogContent*)myTab)->close();
	bool code = exec();
	((ZLQtDialogManager&)ZLQtDialogManager::instance()).fullScreenWorkaround();
	return code;
}

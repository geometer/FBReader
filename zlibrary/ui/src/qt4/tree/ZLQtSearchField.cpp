/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <QtCore/QDebug>

#include <ZLibrary.h>
#include <ZLFile.h>

#include "ZLQtSearchField.h"

ZLQtSearchField::ZLQtSearchField(QWidget *parent) : QLineEdit(parent) {
	//TODO somehow make it feathered
	//TODO set active and non-active color by default of system

	myWaitingIcon = new ZLQtWaitingIcon(QSize(22,22), this);

	mySearchIcon = new QLabel(this);
	static std::string iconPath = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + "search_icon.png";
	QPixmap searchIcon = QPixmap(ZLFile(iconPath).path().c_str());
	mySearchIcon->setPixmap(searchIcon);
	mySearchIcon->setFixedSize(searchIcon.size());

	setFixedHeight(25);
	setFixedWidth(155);
	setPlaceholderText("type to search..."); // TODO add to resources;

	int frameWidth = 2;

	//TODO make external css file
	setStyleSheet(QString("QLineEdit:focus { border-style: solid; border-width: 2px; border-radius: 12px; border-color: #6F9FCF} ") +
				  QString("QLineEdit { border-style: solid; border-width: 1px; border-radius: 12px; border-color: #8D8D8D}") +
				  QString("QLineEdit { padding-left: %1px; } ").arg(mySearchIcon->sizeHint().width() + frameWidth + 1));

}

ZLQtWaitingIcon *ZLQtSearchField::getWaitingIcon() {
	return myWaitingIcon;
}

void ZLQtSearchField::resizeEvent(QResizeEvent *ev) {
	int frameWidth = 2;
	mySearchIcon->move(rect().left() + frameWidth + 3, (rect().bottom() - mySearchIcon->sizeHint().height())/2 + 3);

	QSize sz = myWaitingIcon->sizeHint();
	myWaitingIcon->move(rect().right() - frameWidth - sz.width() - 2, (rect().bottom() + 1 - sz.height())/2);
	QLineEdit::resizeEvent(ev);
}

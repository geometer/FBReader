/*
 * Copyright (C) 2008-2010 Geometer Plus <contact@geometerplus.com>
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

#include <QtGui/QBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QToolBar>
#include <QtGui/QKeyEvent>

#include "ZLQmlApplicationWindow.h"
#include "../util/ZLQtKeyUtil.h"

class ZLQtLineEdit : public QLineEdit {

public:
	ZLQtLineEdit(QToolBar *toolbar, ZLQmlApplicationWindow &window, ZLQmlApplicationWindow::LineEditParameter &parameter, const std::string &actionId);

private:
	void keyReleaseEvent(QKeyEvent *event);

private:
	ZLQmlApplicationWindow &myWindow;
	ZLQmlApplicationWindow::LineEditParameter &myParameter;
	const std::string myActionId;
};

ZLQtLineEdit::ZLQtLineEdit(QToolBar *toolbar, ZLQmlApplicationWindow &window, ZLQmlApplicationWindow::LineEditParameter &parameter, const std::string &actionId) : QLineEdit(toolbar), myWindow(window), myParameter(parameter), myActionId(actionId) {
}

void ZLQtLineEdit::keyReleaseEvent(QKeyEvent *event) {
	event->accept();
	const std::string key = ZLQtKeyUtil::keyName(event);
	if (key == "<Return>") {
		myWindow.application().doAction(myActionId);
		myWindow.setFocusToMainWidget();
	} else if (key == "<Esc>") {
		myParameter.restoreOldValue();
		myWindow.setFocusToMainWidget();
	}
}

ZLQmlApplicationWindow::LineEditParameter::LineEditParameter(QToolBar *toolbar, ZLQmlApplicationWindow &window, const ZLToolbar::ParameterItem &textFieldItem) {
	myEdit = new ZLQtLineEdit(toolbar, window, *this, textFieldItem.actionId());
	myEdit->setAlignment(Qt::AlignHCenter);
	myEdit->setMaxLength(textFieldItem.maxWidth());
	myEdit->setFixedWidth(textFieldItem.maxWidth() * 10 + 10);
	myEdit->setFocusPolicy(Qt::ClickFocus);
	myEdit->setToolTip(QString::fromUtf8(textFieldItem.tooltip().c_str()));
	myAction = toolbar->addWidget(myEdit);
	// I don't understand why this code does work but it does.
	//QBoxLayout *layout = (QBoxLayout*)toolbar->layout();
	//layout->addStretch();
}

QAction *ZLQmlApplicationWindow::LineEditParameter::action() const {
	return myAction;
}

std::string ZLQmlApplicationWindow::LineEditParameter::internalValue() const {
	return (const char*)myEdit->text().toUtf8();
}

void ZLQmlApplicationWindow::LineEditParameter::internalSetValue(const std::string &value) {
	myEdit->setText(QString::fromUtf8(value.c_str()));
}

void ZLQmlApplicationWindow::LineEditParameter::restoreOldValue() {
	VisualParameter::restoreOldValue();
}

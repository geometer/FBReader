/*
 * Copyright (C) 2004-2011 Geometer Plus <contact@geometerplus.com>
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

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QWidget>
#include <QtGui/QLayout>

#include "ZLQmlDialogContent.h"
#include "ZLQmlOptionView.h"

ZLQmlDialogContent::ZLQmlDialogContent(const ZLResource &resource, QObject *parent)
    : QObject(parent), ZLDialogContent(resource) {
	myTitle = QString::fromStdString(ZLDialogContent::displayName());
	myRowCounter = 0;
}

ZLQmlDialogContent::~ZLQmlDialogContent() {
}

void ZLQmlDialogContent::addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option) {
	createViewByEntry(name, tooltip, option, 0, 12);
	++myRowCounter;
}

void ZLQmlDialogContent::addOptions(const std::string &name0, const std::string &tooltip0, ZLOptionEntry *option0, const std::string &name1, const std::string &tooltip1, ZLOptionEntry *option1) {
	createViewByEntry(name0, tooltip0, option0, 0, 5);
	createViewByEntry(name1, tooltip1, option1, 7, 12);
	++myRowCounter;
}

QObjectList ZLQmlDialogContent::items() const {
	return myItems;
}

QString ZLQmlDialogContent::title() const {
	return myTitle;
}

void ZLQmlDialogContent::createViewByEntry(const std::string &name, const std::string &tooltip, ZLOptionEntry *option, int fromColumn, int toColumn) {
	if (option == 0)
		return;
	Q_UNUSED(fromColumn);
	Q_UNUSED(toColumn);
	ZLQmlOptionView *view = 0;
	switch (option->kind()) {
	case ZLOptionEntry::BOOLEAN:
		view = new ZLQmlBooleanOptionView(name, tooltip, option, this);
		break;
	case ZLOptionEntry::BOOLEAN3:
		view = new ZLQmlBoolean3OptionView(name, tooltip, option, this);
		break;
	case ZLOptionEntry::MULTILINE:
	case ZLOptionEntry::STRING:
	case ZLOptionEntry::PASSWORD:
	case ZLOptionEntry::PATH:
		view = new ZLQmlTextOptionView(name, tooltip, option, this);
		break;
	case ZLOptionEntry::CHOICE:
		view = new ZLQmlChoiceOptionView(name, tooltip, option, this);
		break;
	case ZLOptionEntry::SPIN:
		view = new ZLQmlSpinOptionView(name, tooltip, option, this);
		break;
	case ZLOptionEntry::COMBO:
		view = new ZLQmlComboOptionView(name, tooltip, option, this);
		break;
	case ZLOptionEntry::COLOR:
		view = new ZLQmlColorOptionView(name, tooltip, option, this);
		break;
	case ZLOptionEntry::KEY:
//		view = new KeyOptionView(name, tooltip, option, this);
		break;
	case ZLOptionEntry::ORDER:
//		view = new OrderOptionView(name, tooltip, option, this);
		break;
	case ZLOptionEntry::STATIC:
		view = new ZLQmlStaticTextOptionView(name, tooltip, option, this);
		break;
	}

	if (view) {
		myItems << view;
		view->setVisible(option->isVisible());
		addView(view);
		emit itemsChanged(myItems);
	}
}

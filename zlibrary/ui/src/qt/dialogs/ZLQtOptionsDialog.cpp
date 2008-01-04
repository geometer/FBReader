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

#include <qlayout.h>
#include <qpushbutton.h>

#include <ZLDialogManager.h>

#include "ZLQtOptionsDialog.h"
#include "ZLQtDialogContent.h"
#include "ZLQtUtil.h"

ZLQtOptionsDialog::ZLQtOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) : QTabDialog(0, 0, true), ZLDesktopOptionsDialog(resource, applyAction) {
	setCaption(::qtString(ZLOptionsDialog::caption()));
	setOkButton(::qtButtonName(ZLDialogManager::OK_BUTTON));
	setHelpButton(::qtButtonName(ZLDialogManager::CANCEL_BUTTON));
	connect(this, SIGNAL(helpButtonPressed()), this, SLOT(reject()));
	if (showApplyButton) {
		setApplyButton(::qtButtonName(ZLDialogManager::APPLY_BUTTON));
		connect(this, SIGNAL(applyButtonPressed()), this, SLOT(apply()));
	}
}

void ZLQtOptionsDialog::apply() {
	ZLOptionsDialog::accept();
}

ZLDialogContent &ZLQtOptionsDialog::createTab(const ZLResourceKey &key) {
	ZLQtDialogContent *tab = new ZLQtDialogContent(this, tabResource(key));
	addTab(tab->widget(), ::qtString(tab->displayName()));
	myTabs.push_back(tab);
	return *tab;
}

const std::string &ZLQtOptionsDialog::selectedTabKey() const {
	QWidget *currentTab = currentPage();
	for (std::vector<shared_ptr<ZLDialogContent> >::const_iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		if (((ZLQtDialogContent&)**it).widget() == currentTab) {
			return (*it)->key();
		}
	}
	return myTabs[0]->key();
}

void ZLQtOptionsDialog::selectTab(const ZLResourceKey &key) {
	for (std::vector<shared_ptr<ZLDialogContent> >::const_iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		if ((*it)->key() == key.Name) {
			showPage(((ZLQtDialogContent&)**it).widget());
			break;
		}
	}
}

bool ZLQtOptionsDialog::runInternal() {
	for (std::vector<shared_ptr<ZLDialogContent> >::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		((ZLQtDialogContent&)**it).close();
	}
	return exec() == QDialog::Accepted;
}

void ZLQtOptionsDialog::resizeEvent(QResizeEvent *event) {
	QTabDialog::resizeEvent(event);
	emit resized(event->size());
}


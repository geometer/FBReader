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

#include <algorithm>
#include <qscrollview.h>
#include <qfont.h>
#include <qcstring.h>
#include <zapplication.h>
#include <ezxutilcst.h>

#include "ZLQtOptionsDialog.h"
#include "ZLQtDialogManager.h"
#include "ZLQtDialogContent.h"
#include "ZLQtUtil.h"

MyQTabWidget::MyQTabWidget(QWidget *parent) : QWidgetStack(parent) {
}

int MyQTabWidget::currentPageIndex() const {
	return id(visibleWidget());
}

ZLQtOptionsDialog::ZLQtOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction) : ZLFullScreenDialog(std::string("X3")), ZLOptionsDialog(resource, applyAction) {
	QWidget *title = getTitleBarWidget();
	if (title->isA("QLabel")) {
		((QLabel *)title)->setText(::qtString(ZLOptionsDialog::caption()));
	}

	myTabWidget = new MyQTabWidget(this);
	setContentWidget(myTabWidget);

	UTIL_CST *cst = (UTIL_CST*)getCSTWidget();
	myMenu = new QPopupMenu(cst);
	QFont f(qApp->font());
	f.setPointSize(15);
	myMenu->setFont(f);
	connect(myMenu, SIGNAL(activated(int)), this, SLOT(selectTab(int)));
	cst->getLeftBtn()->setEnabled(true);
	cst->getLeftBtn()->setPopup(myMenu);
}

ZLQtOptionsDialog::~ZLQtOptionsDialog() {
}

ZLDialogContent &ZLQtOptionsDialog::createTab(const ZLResourceKey &key) {
	QScrollView *sv = new QScrollView(myTabWidget);
	sv->setResizePolicy(QScrollView::AutoOneFit);
	sv->setFrameStyle(QFrame::NoFrame);
	
	ZLQtDialogContent *tab = new ZLQtDialogContent(sv->viewport(), tabResource(key));
	sv->addChild(tab->widget());
	myMenu->insertItem(::qtString(tab->displayName()), -1, myTabs.size());
	myTabWidget->addWidget(sv, myTabs.size());
	myTabs.push_back(tab);

	if(myTabs.size() == 1) {
		raiseTab(0);
	}

	return *tab;
}

const std::string &ZLQtOptionsDialog::selectedTabKey() const {
	return myTabs[myTabWidget->currentPageIndex()]->key();
}

void ZLQtOptionsDialog::raiseTab(int i) {
	myTabWidget->raiseWidget(i);
	QWidget *title = getTitleBarWidget();
	if(title->isA("QLabel")) {
		QString s = ::qtString(ZLOptionsDialog::caption());
		s.append(" - ");
		s.append(::qtString((*myTabs[i]).displayName()));
		s.replace(QRegExp("[<]"), "&lt;");
		s.replace(QRegExp("[>]"), "&gt;");
		((QLabel *)title)->setText(s);
	}
}

void ZLQtOptionsDialog::selectTab(const ZLResourceKey &key) {
	for (std::vector<shared_ptr<ZLDialogContent> >::const_iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		if ((*it)->key() == key.Name) {
			raiseTab(it - myTabs.begin());
			break;
		}
	}
}

void ZLQtOptionsDialog::selectTab(int i) {
	raiseTab(myMenu->indexOf(i));
}

bool ZLQtOptionsDialog::runInternal() {
	for (std::vector<shared_ptr<ZLDialogContent> >::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		((ZLQtDialogContent&)**it).close();
	}
	return exec();
}

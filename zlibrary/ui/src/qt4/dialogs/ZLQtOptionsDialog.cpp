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

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLayout>
#include <QtGui/QListWidget>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QResizeEvent>

#include <ZLDialogManager.h>

#include "ZLQtOptionsDialog.h"
#include "ZLQtDialogContent.h"
#include "ZLQtUtil.h"
#include "../util/ZLQtImageUtil.h"

ZLQtOptionsDialog::ZLQtOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction) : QDialog(qApp->activeWindow()), ZLDesktopOptionsDialog(resource, applyAction) {
	setModal(true);
	setWindowTitle(::qtString(caption()));

	myTabWidget = new QTabWidget(this);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(
		QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel
	);

	QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
	okButton->setText(::qtButtonName(ZLDialogManager::OK_BUTTON));
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

	QPushButton *cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
	cancelButton->setText(::qtButtonName(ZLDialogManager::CANCEL_BUTTON));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QPushButton *applyButton = buttonBox->button(QDialogButtonBox::Apply);
	applyButton->setText(::qtButtonName(ZLDialogManager::APPLY_BUTTON));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));

	if (parent() == 0) {
		QDesktopWidget *desktop = qApp->desktop();
		if (desktop != 0) {
			move((desktop->width() - width()) / 2, (desktop->height() - height()) / 2);
		}
	}

	QListWidget *categoryList = new QListWidget(this);
	QPixmap pixmap = ZLQtImageUtil::pixmap("fbreader.png");
	categoryList->setIconSize(pixmap.size());
	for (int i = 0; i < 4; ++i) {
		QListWidgetItem *item = new QListWidgetItem(categoryList);
		item->setIcon(QIcon(pixmap));
		item->setText("Category");
		categoryList->addItem(item);
	}

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(categoryList, 0, 0, 1, 1);
	layout->addWidget(myTabWidget,  0, 1, 1, 1);
	layout->addWidget(buttonBox,    1, 0, 1, 2);
	layout->setColumnStretch(1, 4);
	setLayout(layout);
}

void ZLQtOptionsDialog::apply() {
	ZLOptionsDialog::accept();
}

ZLDialogContent &ZLQtOptionsDialog::createTab(const ZLResourceKey &key) {
	ZLQtDialogContent *tab = new ZLQtDialogContent(new QWidget(myTabWidget), tabResource(key));
	myTabWidget->addTab(tab->widget(), ::qtString(tab->displayName()));
	myTabs.push_back(tab);
	return *tab;
}

const std::string &ZLQtOptionsDialog::selectedTabKey() const {
	return myTabs[myTabWidget->currentIndex()]->key();
}

void ZLQtOptionsDialog::selectTab(const ZLResourceKey &key) {
	for (std::vector<shared_ptr<ZLDialogContent> >::const_iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		if ((*it)->key() == key.Name) {
			myTabWidget->setCurrentWidget(((ZLQtDialogContent&)**it).widget());
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

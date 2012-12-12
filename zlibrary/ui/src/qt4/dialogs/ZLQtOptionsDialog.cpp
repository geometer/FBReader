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

#include <algorithm>

#include <QtGui/QApplication>
#include <QtGui/QLayout>
#include <QtGui/QStackedLayout>
#include <QtGui/QListWidget>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QPushButton>
#include <QtGui/QResizeEvent>

#include <ZLDialogManager.h>

#include "ZLQtOptionsDialog.h"
#include "ZLQtDialogContent.h"
#include "ZLQtUtil.h"
#include "../util/ZLQtImageUtil.h"

class ZLQtOptionsListItem : public QListWidgetItem {

public:
	ZLQtOptionsListItem(QListWidget *parent, const std::string &name, QTabWidget *page);

public:
	const std::string Name;
	std::vector<std::string> TabNames;
	QTabWidget * const Page;
	ZLStringOption SelectedTabNameOption;
};

ZLQtOptionsListItem::ZLQtOptionsListItem(QListWidget *parent, const std::string &name, QTabWidget *page) : QListWidgetItem(parent), Name(name), Page(page), SelectedTabNameOption(ZLCategoryKey::LOOK_AND_FEEL, "Preferences", "selectedTab_" + Name, "") {
}

static ZLQtOptionsListItem *findByName(QListWidget &list, const std::string &name) {
	for (int i = 0; i < list.count(); ++i) {
		ZLQtOptionsListItem *item = (ZLQtOptionsListItem*)list.item(i);
		if (item->Name == name) {
			return item;
		}
	}
	return 0;
}

ZLQtOptionsDialog::ZLQtOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction) : QDialog(qApp->activeWindow()), ZLOptionsDialog(resource, applyAction), myGeometryOptions("Preferences_"), mySelectedTabOption(ZLCategoryKey::LOOK_AND_FEEL, "Preferences", "selectedPage", "") {
	setModal(true);
	setWindowTitle(::qtString(caption()));

	myStack = new QStackedLayout();

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

	myCategoryList = new QListWidget(this);
	QPixmap pixmap = ZLQtImageUtil::pixmap("fbreader.png");
	myCategoryList->setIconSize(pixmap.size());

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(myCategoryList, 0, 0, 1, 1);
	layout->addLayout(myStack,        0, 1, 1, 1);
	layout->addWidget(buttonBox,      1, 0, 1, 2);
	layout->setColumnStretch(1, 4);
	setLayout(layout);
}

void ZLQtOptionsDialog::selectPage(int index) {
	mySelectedTabOption.setValue(((ZLQtOptionsListItem*)myCategoryList->item(index))->Name);
	myStack->setCurrentIndex(index);
}

void ZLQtOptionsDialog::selectTab(int index) {
	const std::string pageName = mySelectedTabOption.value();
	ZLQtOptionsListItem *item = ::findByName(*myCategoryList, pageName);
	if (item != 0) {
		item->SelectedTabNameOption.setValue(item->TabNames[index]);
	}
}

void ZLQtOptionsDialog::apply() {
	ZLOptionsDialog::accept();
}

ZLDialogContent &ZLQtOptionsDialog::createTab(const ZLResourceKey &pageKey, const ZLResourceKey &tabKey) {
	ZLQtOptionsListItem *item = ::findByName(*myCategoryList, pageKey.Name);
	QTabWidget *page;
	if (item != 0) {
		page = item->Page;
	} else {
		page = new QTabWidget(this);
		myStack->addWidget(page);

		item = new ZLQtOptionsListItem(myCategoryList, pageKey.Name, page);
		item->setIcon(ZLQtImageUtil::pixmap("fbreader.png"));
		item->setText(QString::fromUtf8(myResource["pages"][pageKey].value().c_str()));
		myCategoryList->addItem(item);
	}
	ZLQtDialogContent *tab = new ZLQtDialogContent(new QWidget(page), myResource[tabKey]);
	item->TabNames.push_back(tabKey.Name);
	page->addTab(tab->widget(), ::qtString(tab->displayName()));
	myTabs.push_back(tab);
	return *tab;
}

bool ZLQtOptionsDialog::run() {
	myGeometryOptions.setToWidget(*this);

	connect(myCategoryList, SIGNAL(currentRowChanged(int)), this, SLOT(selectPage(int)));
	const std::string selectedTabName = mySelectedTabOption.value();
	myCategoryList->setCurrentRow(0);
	for (int i = 0; i < myCategoryList->count(); ++i) {
		ZLQtOptionsListItem *item = (ZLQtOptionsListItem*)myCategoryList->item(i);
		if (item->Name == selectedTabName) {
			myCategoryList->setCurrentRow(i);
			break;
		}
	}

	for (int i = 0; i < myCategoryList->count(); ++i) {
		ZLQtOptionsListItem *item = (ZLQtOptionsListItem*)myCategoryList->item(i);
		std::vector<std::string>::const_iterator it =
			std::find(item->TabNames.begin(), item->TabNames.end(), item->SelectedTabNameOption.value());
		item->Page->setCurrentIndex(it != item->TabNames.end() ? it - item->TabNames.begin() : 0);
		connect(item->Page, SIGNAL(currentChanged(int)), this, SLOT(selectTab(int)));
	}

	for (std::vector<shared_ptr<ZLDialogContent> >::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		((ZLQtDialogContent&)**it).close();
	}
	return exec() == QDialog::Accepted;
}

void ZLQtOptionsDialog::resizeEvent(QResizeEvent* event) {
	QDialog::resizeEvent(event);
	myGeometryOptions.getFromWidget(*this);
}

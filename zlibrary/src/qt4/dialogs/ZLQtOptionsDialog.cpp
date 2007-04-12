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

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QResizeEvent>

#include "ZLQtOptionsDialog.h"
#include "ZLQtDialogContent.h"

ZLQtOptionsDialog::ZLQtOptionsDialog(const std::string &id, const std::string &caption, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) : QDialog(qApp->activeWindow()), ZLDesktopOptionsDialog(id, applyAction) {
	setModal(true);
	setWindowTitle(caption.c_str());
	QVBoxLayout *layout = new QVBoxLayout(this);

	myTabWidget = new QTabWidget(this);
	layout->addWidget(myTabWidget);

	QWidget *group = new QWidget(this);
	layout->addWidget(group);
	QGridLayout *buttonLayout = new QGridLayout(group);
	buttonLayout->setColumnStretch(0, 3);

	QPushButton *okButton = new QPushButton(group);
	okButton->setText("&Ok");
	buttonLayout->addWidget(okButton, 0, 1);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

	QPushButton *cancelButton = new QPushButton(group);
	cancelButton->setText("&Cancel");
	buttonLayout->addWidget(cancelButton, 0, 2);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	if (showApplyButton) {
		QPushButton *applyButton = new QPushButton(group);
		applyButton->setText("&Apply");
		buttonLayout->addWidget(applyButton, 0, 3);
		connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));
	}

	if (parent() == 0) {
		QDesktopWidget *desktop = qApp->desktop();
		if (desktop != 0) {
			move((desktop->width() - width()) / 2, (desktop->height() - height()) / 2);
		}
	}
}

void ZLQtOptionsDialog::apply() {
	ZLOptionsDialog::accept();
}

ZLDialogContent &ZLQtOptionsDialog::createTab(const std::string &name) {
	ZLQtDialogContent *tab = new ZLQtDialogContent(new QWidget(myTabWidget), name);
	myTabWidget->addTab(tab->widget(), name.c_str());
	myTabs.push_back(tab);
	return *tab;
}

const std::string &ZLQtOptionsDialog::selectedTabName() const {
	return myTabs[myTabWidget->currentIndex()]->name();
}

void ZLQtOptionsDialog::selectTab(const std::string &name) {
	for (std::vector<shared_ptr<ZLDialogContent> >::const_iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		if ((*it)->name() == name) {
			myTabWidget->setCurrentWidget(((ZLQtDialogContent&)**it).widget());
			break;
		}
	}
}

bool ZLQtOptionsDialog::run() {
	for (std::vector<shared_ptr<ZLDialogContent> >::iterator it = myTabs.begin(); it != myTabs.end(); ++it) {
		((ZLQtDialogContent&)**it).close();
	}
	return exec() == QDialog::Accepted;
}

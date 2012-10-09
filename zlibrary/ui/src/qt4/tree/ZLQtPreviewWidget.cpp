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

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>

#include <QtCore/QDebug>

#include <ZLNetworkManager.h>
#include <ZLStringUtil.h>

#include "../image/ZLQtImageUtils.h"

#include "ZLQtPreviewWidget.h"


ZLQtButtonAction::ZLQtButtonAction(shared_ptr<ZLTreeAction> action,QWidget *parent) :
	QPushButton(parent), myAction(action) {
	connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

void ZLQtButtonAction::onClicked() {
	if (myAction.isNull() || !myAction->makesSense()) {
		return;
	}
	myAction->run();
}

ZLQtPreviewWidget::ZLQtPreviewWidget(QWidget *parent) : QWidget(parent) {
	myPicLabel = new QLabel;
	myPicLabel->setMaximumSize(300,300);
	myPicLabel->setMinimumSize(77,77);

	myTitleLabel = new QLabel;
	myAuthorLabel = new QLabel;
	myCategoriesLabel = new QLabel;
	mySummaryLabel = new QLabel;

	myActionsWidget = new QWidget;


	QVBoxLayout *previewLayout = new QVBoxLayout;
	QHBoxLayout *actionsLayout = new QHBoxLayout;
	myActionsWidget->setLayout(actionsLayout);

	previewLayout->addWidget(myPicLabel);
	previewLayout->addWidget(myTitleLabel);
	previewLayout->addWidget(myAuthorLabel);
	previewLayout->addWidget(myCategoriesLabel);
	previewLayout->addWidget(mySummaryLabel);
	previewLayout->addStretch();
	previewLayout->addWidget(myActionsWidget);
	setLayout(previewLayout);
}

void ZLQtPreviewWidget::fill(const ZLTreePageInfo &info) {
	clear();

	shared_ptr<const ZLImage> image = info.image();
	if (!image.isNull()) {
		ZLNetworkManager::Instance().perform(image->synchronizationData());
		QPixmap pixmap = ZLQtImageUtils::ZLImageToQPixmap(image);
		myPicLabel->setPixmap(pixmap);
	}

	//TODO scrolling for annotation
	//TODO implement 'condition' attribute support for ZLResource
	//TODO maybe add labels here, not at constructor
	static const ZLResource &resource = ZLResource::resource("bookInfo");
	static std::string VALUE_1 = "value 1";
	static QString colon = "<b>%1</b> %2";

	if (!info.title().empty()) {
		static QString title = QString::fromStdString(resource["title"].value());
		myTitleLabel->setText(colon.arg(title, QString::fromStdString(info.title())));
	}

	if (info.authors().size() > 0) {
		static QString authors = QString::fromStdString(resource["authors"].value());
		static QString author = QString::fromStdString(resource["authors"][VALUE_1].value());
		QString labelText = info.authors().size() == 1 ? author : authors;
		myAuthorLabel->setText(colon.arg(labelText, QString::fromStdString(ZLStringUtil::join(info.authors(), ", "))));
	}

	if (info.tags().size() > 0) {
		static QString tags = QString::fromStdString(resource["tags"].value());
		static QString tag = QString::fromStdString(resource["tags"][VALUE_1].value());
		QString labelText = info.tags().size() == 1 ? tag : tags;
		myCategoriesLabel->setText(colon.arg(labelText, QString::fromStdString(ZLStringUtil::join(info.tags(), ", "))));
	}

	if (!info.summary().empty()) {
		static QString annotation = QString::fromStdString(resource["annotation"].value());
		mySummaryLabel->setText(colon.arg(annotation, QString::fromStdString(info.summary())));
	}

	myTitleLabel->setWordWrap(true);
	myAuthorLabel->setWordWrap(true);
	myCategoriesLabel->setWordWrap(true);
	mySummaryLabel->setWordWrap(true);

	foreach(shared_ptr<ZLTreeAction> action, info.actions()) {
		if (!action->makesSense()) {
			continue;
		}
		QPushButton *actionButton = new ZLQtButtonAction(action);
		QString text = QString::fromStdString(info.actionText(action));
		actionButton->setText(text);
		myButtons.push_back(actionButton);
		myActionsWidget->layout()->addWidget(actionButton);
	}

}

void ZLQtPreviewWidget::clear() {
	myPicLabel->setPixmap(QPixmap());
	myTitleLabel->clear();
	myAuthorLabel->clear();
	myCategoriesLabel->clear();
	mySummaryLabel->clear();
	foreach(QPushButton *button, myButtons) {
		delete button;
	}
	myButtons.clear();
}

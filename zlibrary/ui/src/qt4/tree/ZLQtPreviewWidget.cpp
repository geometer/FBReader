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
#include <QtGui/QScrollBar>
#include <QtGui/QGraphicsDropShadowEffect>

#include <QtCore/QDebug>

#include <ZLNetworkManager.h>
#include <ZLStringUtil.h>

#include "QtWaitingSpinner.h"
#include "../image/ZLQtImageUtils.h"

#include "ZLQtPreviewWidget.h"

class ZLQtLabelAction : public QLabel {

public:
	ZLQtLabelAction(shared_ptr<ZLTreeAction> action, QWidget *parent=0, Qt::WindowFlags f=0);

protected:
	 void mousePressEvent(QMouseEvent *ev);

private:
	shared_ptr<ZLTreeAction> myAction;
};

ZLQtLabelAction::ZLQtLabelAction(shared_ptr<ZLTreeAction> action,QWidget *parent, Qt::WindowFlags f) :
	QLabel(parent, f), myAction(action) {
	setCursor(Qt::PointingHandCursor);
	QPalette p = palette();
	p.setColor(QPalette::WindowText, QColor(33, 96, 180)); //blue color
	setPalette(p);
}

void ZLQtLabelAction::mousePressEvent(QMouseEvent *) {
	if (myAction.isNull() || !myAction->makesSense()) {
		return;
	}
	myAction->run();
}


ZLQtButtonAction::ZLQtButtonAction(shared_ptr<ZLTreeAction> action,QWidget *parent) :
	QPushButton(parent), myAction(action) {
	connect(this, SIGNAL(clicked()), this, SLOT(onClicked()), Qt::QueuedConnection); //QueuedConnection used for sending clicked() signal for UI at first
	setAttribute(Qt::WA_LayoutUsesWidgetRect);
}

void ZLQtButtonAction::onClicked() {
	if (myAction.isNull() || !myAction->makesSense()) {
		return;
	}
	myAction->run();
}

ZLQtPreviewWidget::ZLQtPreviewWidget(QWidget *parent) : QWidget(parent), myCurrentNode(0) {
	QSizePolicy policy = QSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
	//policy.setHorizontalStretch(2);
	setSizePolicy(policy);
}

void ZLQtPreviewWidget::show(ZLTreeNode *node) {
	clear();
	myCurrentNode = node;
	if (ZLTreePageNode *pageNode = zlobject_cast<ZLTreePageNode*>(node)) {
		if (myDownloadingNodes.contains(pageNode)) {
			fillWaitingIcon();
			return;
		}
		fillPageInfo(pageNode);
	} else if (const ZLTreeTitledNode *titledNode = zlobject_cast<const ZLTreeTitledNode*>(node)) {
		fillCatalog(titledNode);
	}
}

void ZLQtPreviewWidget::refresh() {
	if (myCurrentNode) {
		show(myCurrentNode);
	}
}

class PageInfoLoaderListener : public ZLNetworkRequest::Listener {
	public:
		PageInfoLoaderListener(ZLQtPreviewWidget &previewWidget, ZLTreePageNode *node) :
			myPreviewWidget(previewWidget), myNode(node) {
			myPreviewWidget.myDownloadingNodes.insert(node);
		}
		void finished(const std::string &error) {
			myPreviewWidget.myDownloadingNodes.remove(myNode);
			if (myPreviewWidget.myCurrentNode == myNode) {
				myPreviewWidget.refresh();
			}
		}
	private:
		ZLQtPreviewWidget &myPreviewWidget;
		ZLTreePageNode *myNode;
};

void ZLQtPreviewWidget::fillPageInfo(ZLTreePageNode *node) {
	if (myDownloadingNodes.contains(node)) {
		fillWaitingIcon();
		return;
	}
	shared_ptr<ZLTreePageInfo> info = node->getPageInfo();
	if (info.isNull()) {
		return;
	}
	if (!info->isPageInfoLoaded()) {
		fillWaitingIcon();
		info->loadAll(new PageInfoLoaderListener(*this, node));
		return;
	}
	setBasicLayout();
	QWidget *widget = new ZLQtPageWidget(*info);
	widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	layout()->addWidget(widget);
}


void ZLQtPreviewWidget::setBasicLayout() {
	QHBoxLayout *layout = new QHBoxLayout;
	layout->setSizeConstraint(QLayout::SetMinimumSize);
	layout->setContentsMargins(0,0,0,0);
	setLayout(layout);
}


void ZLQtPreviewWidget::fillCatalog(const ZLTreeTitledNode *node) {
	setBasicLayout();
	QWidget *widget = new ZLQtCatalogPageWidget(node);
	widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	layout()->addWidget(widget);
}

void ZLQtPreviewWidget::fillWaitingIcon() {
	setBasicLayout();
	QtWaitingSpinner *icon = new QtWaitingSpinner(12, 26, 8, 36);
	icon->start();
	layout()->addWidget(icon);
}


void ZLQtPreviewWidget::clear() {
	myCurrentNode = 0;
	qDeleteAll(children());

}

ZLTreeNode *ZLQtPreviewWidget::getCurrentNode() const {
	return myCurrentNode;
}

QSize ZLQtPreviewWidget::sizeHint() const {
	QSize hint = QWidget::sizeHint();
//	qDebug() << Q_FUNC_INFO << hint << minimumSize();
	return hint;
}

ZLQtAbstractPageWidget::ZLQtAbstractPageWidget(QWidget *parent) : QWidget(parent) { }

void ZLQtAbstractPageWidget::onActionActivated() {
	foreach(QPushButton* button, myButtonActions) {
		button->setEnabled(false);
	}
}

ZLQtPageWidget::ZLQtPageWidget(const ZLTreePageInfo &info, QWidget *parent) : ZLQtAbstractPageWidget(parent) {
	//TODO fix it: if element is absent, there's a empty space instead it. Looks bad.
	createElements();
	setInfo(info);
}

void ZLQtPageWidget::createElements() {
	myPicLabel = new QLabel;

  // the code inside #ifndef does crash on MacOS,
  // see https://bugreports.qt-project.org/browse/QTBUG-24792
#ifndef Q_OS_MAC
	QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect;
	effect->setBlurRadius(12);
	effect->setOffset(3);
	myPicLabel->setGraphicsEffect(effect);
#endif
//	myPicLabel->setMaximumSize(300,300);
//	myPicLabel->setMinimumSize(77,77);

	myTitleLabel = new QLabel;
	myAuthorLabel = new QLabel;
	myCategoriesLabel = new QLabel;
	mySummaryTitleLabel = new QLabel;
	mySummaryLabel = new QLabel;

	myScrollArea = new QScrollArea;
	myScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	myScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	myScrollArea->setWidgetResizable(true);
	myScrollArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

	mySummaryLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	mySummaryLabel->setMargin(3);
	//mySummaryLabel->setMaximumHeight(200);
	mySummaryLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
	//mySummaryLabel->setIndent(3);

	myRelatedWidget = new QWidget;

	myActionsWidget = new QWidget;

	QVBoxLayout *layout = new QVBoxLayout;
	QWidget *containerWidget = new QWidget;

	QVBoxLayout *previewLayout = new QVBoxLayout;
	QHBoxLayout *actionsLayout = new QHBoxLayout;
	QVBoxLayout *relatedLayout = new QVBoxLayout;
	myActionsWidget->setLayout(actionsLayout);
	previewLayout->setSizeConstraint(QLayout::SetMinimumSize);

	layout->setSizeConstraint(QLayout::SetMinimumSize);

	QHBoxLayout *picLayout = new QHBoxLayout;
	picLayout->addStretch();
	picLayout->addWidget(myPicLabel);
	picLayout->addStretch();

	previewLayout->addLayout(picLayout);
	previewLayout->addWidget(myTitleLabel);
	previewLayout->addWidget(myAuthorLabel);
	previewLayout->addWidget(myCategoriesLabel);
	previewLayout->addWidget(mySummaryTitleLabel);
	previewLayout->addWidget(mySummaryLabel);
	previewLayout->addWidget(myRelatedWidget);
	//previewLayout->addStretch();

	myRelatedWidget->setLayout(relatedLayout);
	containerWidget->setLayout(previewLayout);
	myScrollArea->setWidget(containerWidget);

	layout->setContentsMargins(0,0,0,0);

	layout->addWidget(myScrollArea);
	layout->addWidget(myActionsWidget);
	setLayout(layout);
}

void ZLQtPageWidget::setInfo(const ZLTreePageInfo &info) {
	shared_ptr<const ZLImage> image = info.image();
	if (!image.isNull()) {
		QPixmap pixmap = ZLQtImageUtils::ZLImageToQPixmap(image);
		//TODO implement resizable pixmap widget
		const int maxPreviewWidth = 300;
		if (pixmap.height() > maxPreviewWidth || pixmap.width() > maxPreviewWidth) {
			pixmap = pixmap.scaled(maxPreviewWidth, maxPreviewWidth, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}
		myPicLabel->setPixmap(pixmap);
	}

	//TODO implement 'condition'-like attributes support for ZLResource
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
		mySummaryLabel->setText(QString::fromStdString(info.summary()));
		mySummaryTitleLabel->setText(QString("<b>%1</b>").arg(annotation));
//		mySummaryScrollArea->show();
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
		myActionsWidget->layout()->addWidget(actionButton);
		connect(actionButton, SIGNAL(clicked()), this, SLOT(onActionActivated()));
		myButtonActions.push_back(actionButton);
	}

	//TODO check case if I will load all links at the same time
	foreach(shared_ptr<ZLTreeAction> action, info.relatedActions()) {
		if (!action->makesSense()) {
			continue;
		}
		QLabel *actionLabel = new ZLQtLabelAction(action);
		QString text = QString::fromStdString(info.actionText(action));
		actionLabel->setText(QString("<big><u>%1</u></big>").arg(text));
		actionLabel->setWordWrap(true);
		myRelatedWidget->layout()->addWidget(actionLabel);
	}
}

ZLQtCatalogPageWidget::ZLQtCatalogPageWidget(const ZLTreeTitledNode *node, QWidget *parent) : ZLQtAbstractPageWidget(parent) {
	createElements();
	setInfo(node);
}

void ZLQtCatalogPageWidget::createElements() {
	myPicLabel = new QLabel;
	myTitleLabel = new QLabel;
	mySubtitleLabel = new QLabel;

	mySubtitleLabel->setWordWrap(true);
	myTitleLabel->setAlignment(Qt::AlignCenter);
	mySubtitleLabel->setAlignment(Qt::AlignCenter);

	myActionsWidget = new QWidget;

	QVBoxLayout *previewLayout = new QVBoxLayout;
	previewLayout->setSizeConstraint(QLayout::SetMinimumSize);

	QHBoxLayout *strechActionsLayout = new QHBoxLayout;
	strechActionsLayout->addStretch();
	strechActionsLayout->addWidget(myActionsWidget);
	strechActionsLayout->addStretch();
	QVBoxLayout *actionsLayout = new QVBoxLayout; myActionsWidget->setLayout(actionsLayout);

	QHBoxLayout *picLayout = new QHBoxLayout;
	picLayout->addStretch();
	picLayout->addWidget(myPicLabel);
	picLayout->addStretch();

	previewLayout->addStretch();
	previewLayout->addLayout(picLayout);
	previewLayout->addWidget(myTitleLabel);
	previewLayout->addWidget(mySubtitleLabel);
	previewLayout->addLayout(strechActionsLayout);
	previewLayout->addStretch();

	setLayout(previewLayout);
}

void ZLQtCatalogPageWidget::setInfo(const ZLTreeTitledNode *node) {
	shared_ptr<const ZLImage> image = node->image();
	if (!image.isNull()) {

		//TODO make async image downloading

		ZLNetworkManager::Instance().perform(image->synchronizationData());
		QPixmap pixmap = ZLQtImageUtils::ZLImageToQPixmap(image);
		//TODO implement resizable pixmap widget
		const int maxPreviewWidth = 300;
		if (pixmap.height() > maxPreviewWidth || pixmap.width() > maxPreviewWidth) {
			pixmap = pixmap.scaled(maxPreviewWidth, maxPreviewWidth, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}
		myPicLabel->setPixmap(pixmap);
	}

	if (!node->title().empty()) {
		myTitleLabel->setText(QString("<b>%1</b>").arg(QString::fromStdString(node->title())));
	}
	if (!node->subtitle().empty()) {
		mySubtitleLabel->setText(QString::fromStdString(node->subtitle()));
	}

	foreach(shared_ptr<ZLTreeAction> action, node->actions()) {
		if (!action->makesSense()) {
			continue;
		}
		QPushButton *actionButton = new ZLQtButtonAction(action);
		QString text = QString::fromStdString(node->actionText(action));
		actionButton->setText(text);
		myActionsWidget->layout()->addWidget(actionButton);
		connect(actionButton, SIGNAL(clicked()), this, SLOT(onActionActivated()));
		myButtonActions.push_back(actionButton);
		//TODO maybe buttons should not be too big
		//actionButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

	}

}

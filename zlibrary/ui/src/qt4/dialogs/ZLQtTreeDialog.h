/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLQTTREEDIALOG_H__
#define __ZLQTTREEDIALOG_H__

#include <QtCore/QStack>
#include <QtCore/QSet>
#include <QtCore/QMap>

#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>

#include <ZLTreeDialog.h>

#include "../tree/ZLQtItemsListWidget.h"
#include "../tree/ZLQtPreviewWidget.h"
#include "../tree/ZLQtSearchField.h"
#include "../tree/ZLTreeDataProvider.h"
#include "../util/ZLQtMainWindow.h"

class ZLQtToolbarButton;
class ZLTreeDataProvider;

class ZLQtTreeDialog : public ZLQtMainWindow, public ZLTreeDialog {
	Q_OBJECT

public:
    ZLQtTreeDialog(const std::string &windowName, const ZLResource &resource, ZLTreeDataProvider *dataProvider);

public:
	void run(ZLTreeNode *rootNode);

protected:
	void resizeEvent(QResizeEvent *event);

public: //listener methods
	void onExpandRequest(ZLTreeNode *node);
	void onCloseRequest();
	void onNodeUpdated(ZLTreeNode *node);

	void onDownloadingStarted(ZLTreeNode *node);
	void onDownloadingStopped(ZLTreeNode *node);

	void onSearchStarted(ZLTreeNode *node);
	void onSearchStopped(ZLTreeNode *node);

	void onRefresh();

	void onMoreChildrenRequest(ZLTreeNode *node);

public:
	void onChildrenLoaded(ZLTreeNode *node, bool checkLast, bool success);
	void onMoreChildrenLoaded(bool success);

private:
	void updateAll();
	void updateWaitingIcons();
	void updateNavigationButtons();

private: //TODO maybe these functions can be used as a typical updateAll()
	void saveShowParameters();
	void setupShowParameters();

private Q_SLOTS:
	void onNodeClicked(ZLQtTreeItem* item);
	void onNodeDoubleClicked(ZLQtTreeItem* item);
	void onBackButton();
	void onForwardButton();
	void onSearchField();
	void onMoreChildren();

private:
	ZLTreeNode *myRootNode;
    ZLTreeDataProvider *myDataProvider;

private:
	ZLQtToolbarButton *myBackButton;
	ZLQtToolbarButton *myForwardButton;
	ZLQtSearchField *mySearchField;
//	QScrollArea *myScrollArea;
	ZLQtItemsListWidget *myListWidget;
	ZLQtPreviewWidget *myPreviewWidget;

	QStack<ZLTreeNode*> myBackHistory;
	QStack<ZLTreeNode*> myForwardHistory;

	struct ShowParameter {
		int sliderPosition;
		int activeItemNumber;
	};

	QMap<ZLTreeNode*, ShowParameter> myShowParameters;

private:
//	QList<shared_ptr<ZLNetworkRequest::Listener> > myListeners;
    QSet<ZLTreeNode *> myDownloadingNodes;
    const ZLTreeNode *myLastClickedNode; //used to 'last clicked item shows first after downloading'
    const ZLTreeNode *myLastClickedSearchNode; //used to 'last clicked item shows first after downloading'

};

#endif /* __ZLQTTREEDIALOG_H__ */

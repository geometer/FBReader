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

#ifndef __ZLQTTREEDIALOG_H__
#define __ZLQTTREEDIALOG_H__

#include <QtCore/QStack>
#include <QtCore/QSet>
#include <QtCore/QMap>

#include <QtGui/QMainWindow>
#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>

#include <ZLTreeDialog.h>

#include "../tree/ZLQtItemsListWidget.h"
#include "../tree/ZLQtPreviewWidget.h"
#include "../tree/ZLQtSearchField.h"

class ZLQtToolbarButton;

class ZLQtTreeDialog : public QMainWindow, public ZLTreeDialog {
	Q_OBJECT
public:
	ZLQtTreeDialog(const ZLResource &resource);

public:
	void run(ZLTreeNode *rootNode);

protected:
	QSize sizeHint() const;
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
	class ChildrenRequestListener : public ZLNetworkRequest::Listener {
		public:
			ChildrenRequestListener(ZLQtTreeDialog *dialog, ZLTreeNode *node, bool moreMode);
			void finished(const std::string &error);
//			const ZLTreeNode *getNode() const;

		private:
			ZLQtTreeDialog *myTreeDialog;
			ZLTreeNode *myNode;
			bool myMoreMode;
	};

//	QList<shared_ptr<ZLNetworkRequest::Listener> > myListeners;
	QSet<ZLTreeNode *> myDownloadingNodes;
	const ZLTreeNode *myLastClickedNode; //used to 'last clicked item shows first after downloading'
	const ZLTreeNode *myLastClickedSearchNode; //used to 'last clicked item shows first after downloading'

friend class ChildrenRequestListener;

};

#endif /* __ZLQTTREEDIALOG_H__ */

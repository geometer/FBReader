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

#include <QtGui/QDialog>
#include <QtGui/QScrollArea>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>

#include <ZLTreeDialog.h>

#include "../tree/ZLQtItemsListWidget.h"
#include "../tree/ZLQtPreviewWidget.h"

class ZLQtTreeDialog : public QDialog, public ZLTreeDialog {
	Q_OBJECT
public:
	ZLQtTreeDialog(const ZLResource &resource, QWidget *parent = 0);

public:
	void run(ZLTreeNode *rootNode);

protected:
	QSize sizeHint() const;
	void resizeEvent(QResizeEvent *event);

public: //listener methods
	void onExpandRequest(ZLTreeNode *node);
	void onCloseRequest();
	void onNodeBeginInsert(ZLTreeNode *parent, size_t index);
	void onNodeEndInsert();
	void onNodeBeginRemove(ZLTreeNode *parent, size_t index);
	void onNodeEndRemove();
	void onNodeUpdated(ZLTreeNode *node);

	void onDownloadingStarted(ZLTreeNode *node);
	void onDownloadingStopped(ZLTreeNode *node);

	void onRefresh();

public:
	void onChildrenLoaded(const ZLTreeNode *node, bool checkLast, bool success);

private:
	void updateAll();
	void updateWaitingIcons();
	void updateBackButton();

private Q_SLOTS:
	void onNodeClicked(ZLQtTreeItem* item);
	void onNodeDoubleClicked(ZLQtTreeItem* item);
	void onBackButton();


private:
	ZLTreeNode *myRootNode;

private:
	QPushButton *myBackButton;
	QLineEdit *mySearchField;
//	QScrollArea *myScrollArea;
	ZLQtItemsListWidget *myListWidget;
	ZLQtPreviewWidget *myPreviewWidget;

	QStack<const ZLTreeNode*> myHistoryStack;

private:

	class ChildrenRequestListener : public ZLNetworkRequest::Listener {
		public:
			ChildrenRequestListener(ZLQtTreeDialog *dialog, const ZLTreeNode *node);
			void finished(const std::string &error);
//			const ZLTreeNode *getNode() const;

		private:
			ZLQtTreeDialog *myTreeDialog;
			const ZLTreeNode *myNode;
	};

//	QList<shared_ptr<ZLNetworkRequest::Listener> > myListeners;
	QSet<ZLTreeNode *> myDownloadingNodes;
	const ZLTreeNode *myLastClickedNode; //used to 'last clicked item shows first after downloading'

friend class ChildrenRequestListener;
};

#endif /* __ZLQTTREEDIALOG_H__ */

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

#include <QtGui/QDialog>
#include <QtGui/QScrollArea>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include "../tree/ZLQtItemsListWidget.h"
#include "../tree/ZLQtPreviewWidget.h"

#include <ZLTreeDialog.h>

class ZLQtTreeDialog : public QDialog, public ZLTreeDialog {
	Q_OBJECT
public:
	ZLQtTreeDialog(const ZLResource &resource, QWidget *parent = 0);

public:
	void run(ZLTreeNode *rootNode);

private Q_SLOTS:
	void onNodeEntered(const ZLTreeNode* node);
	void onBackButton();


private:
	const ZLTreeNode *myRootNode;

private:
	QPushButton *myBackButton;
	QLineEdit *mySearchField;
	QScrollArea *myScrollArea;
	ZLQtItemsListWidget *myListWidget;
	ZLQtPreviewWidget *myPreviewWidget;

	QStack<const ZLTreeNode*> myHistoryStack;

};





#endif /* __ZLQTTREEDIALOG_H__ */

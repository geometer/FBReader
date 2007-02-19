/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <iostream>

#include <ZLApplication.h>
#include <ZLFile.h>

#include "ZLWin32SelectionDialog.h"

#include "../w32widgets/W32Container.h"
#include "../w32widgets/W32Control.h"

#include "../application/ZLWin32ApplicationWindow.h"

const int ICON_SIZE = 22;

ZLWin32SelectionDialog::ZLWin32SelectionDialog(ZLWin32ApplicationWindow &window, const std::string &caption, ZLTreeHandler &handler) : ZLDesktopSelectionDialog(handler), myWindow(window), myPanel(myWindow.mainWindow(), caption) {
	myPanel.setResizeable(true);
	myPanel.setExitOnOk(false);
	myPanel.addListener(this);

	W32VBorderBox *panelBox = new W32VBorderBox();
	myPanel.setElement(panelBox);

	myLineEditor = new W32LineEditor("");
	myLineEditor->setVisible(true);
	myLineEditor->setEnabled(!handler.isOpenHandler());
	panelBox->setTopElement(myLineEditor);

	myTreeView = new W32TreeView(ICON_SIZE);
	myTreeView->setVisible(true);
	myTreeView->addListener(this);
	panelBox->setCenterElement(myTreeView);

	const short charHeight = myPanel.charDimension().Height;
	panelBox->setSpacing(charHeight / 2);
	panelBox->setMargins(charHeight / 2, charHeight / 2, charHeight / 2, charHeight / 2);

	W32HBox *buttonBox = new W32HBox();
	panelBox->setBottomElement(buttonBox);
	buttonBox->setHomogeneous(true);
	buttonBox->setAlignment(W32HBox::RIGHT);
	buttonBox->setSpacing(charHeight / 2);
	buttonBox->setMargins(charHeight / 2, charHeight / 2, charHeight / 2, charHeight / 2);
	myOkButton = new W32PushButton("&Ok");
	myOkButton->addListener(this);
	buttonBox->addElement(myOkButton);
	buttonBox->addElement(new W32PushButton("&Cancel", W32PushButton::CANCEL_BUTTON));
	buttonBox->setVisible(true);

	update();
}

ZLWin32SelectionDialog::~ZLWin32SelectionDialog() {
	for(std::map<std::string,HBITMAP>::iterator it = myBitmaps.begin(); it != myBitmaps.end(); ++it) {
		// TODO:
		//ReleaseImage(it->second);
	}
}

HBITMAP ZLWin32SelectionDialog::getBitmap(const ZLTreeNodePtr node) {
	const std::string &pixmapName = node->pixmapName();
	std::map<std::string,HBITMAP>::const_iterator it = myBitmaps.find(pixmapName);
	if (it == myBitmaps.end()) {
		std::string imageFileName = ZLApplication::ApplicationImageDirectory() + ZLApplication::FileNameDelimiter + pixmapName + ".bmp";
		ZLFile file(imageFileName);
		HBITMAP bitmap = (HBITMAP)LoadImageA(0, file.path().c_str(), IMAGE_BITMAP, ICON_SIZE, ICON_SIZE, LR_LOADFROMFILE);
		myBitmaps[pixmapName] = bitmap;
		return bitmap;
	} else {
		return it->second;
	}
}

void ZLWin32SelectionDialog::updateStateLine() {
	myLineEditor->setText(handler().stateDisplayName());
}

void ZLWin32SelectionDialog::updateList() {
	myTreeView->clear();

	const std::vector<ZLTreeNodePtr> &nodes = handler().subnodes();
	if (nodes.empty()) {
		return;
	}

	for (std::vector<ZLTreeNodePtr>::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
		myTreeView->insert((*it)->displayName(), getBitmap(*it));
	}
}

void ZLWin32SelectionDialog::selectItem(int index) {
	myTreeView->select(index);
}

bool ZLWin32SelectionDialog::run() {
	myWindow.blockMouseEvents(true);
	bool result = myPanel.runDialog();
	myWindow.blockMouseEvents(false);
	return result;
}

void ZLWin32SelectionDialog::exitDialog() {
	myPanel.endDialog(true);
}

void ZLWin32SelectionDialog::setSize(int width, int height) {
	//gtk_window_resize(GTK_WINDOW(myDialog), width, height);
}

int ZLWin32SelectionDialog::width() const {
	/*
	int _width;
	gtk_window_get_size(GTK_WINDOW(myDialog), &_width, 0);
	return _width;
	*/
	return 200;
}

int ZLWin32SelectionDialog::height() const {
	/*
	int _height;
	gtk_window_get_size(GTK_WINDOW(myDialog), 0, &_height);
	return _height;
	*/
	return 200;
}

void ZLWin32SelectionDialog::onEvent(const std::string &event, W32EventSender &sender) {
	if ((event == W32StandaloneDialogPanel::OK_EVENT) ||
			(event == W32TreeView::ITEM_DOUBLE_CLICKED_EVENT) ||
			(event == W32PushButton::RELEASED_EVENT)) {
		if (handler().isOpenHandler()) {
			int index = myTreeView->selectedIndex();
			const std::vector<ZLTreeNodePtr> &nodes = handler().subnodes();
			if ((index >= 0) && (index < (int)nodes.size())) {
				runNode(nodes[index]);
			}
		} else {
			//runState(gtk_entry_get_text(myStateLine));	
		}
	} else if (event == W32TreeView::ITEM_SELECTED_EVENT) {
		int index = myTreeView->selectedIndex();
		const std::vector<ZLTreeNodePtr> &nodes = handler().subnodes();
		if ((index >= 0) && (index < (int)nodes.size())) {
			myOkButton->setEnabled(!nodes[index]->isFolder());
		}
	}
}

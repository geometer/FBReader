/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLApplication.h>
#include <ZLibrary.h>
#include <ZLFile.h>

#include <ZLDialogManager.h>

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
	panelBox->setSpacing(4);
	panelBox->setMargins(4, 4, 4, 4);
	myPanel.setElement(panelBox);

	myLineEditor = new W32LineEditor("");
	myLineEditor->setVisible(true);
	myLineEditor->setEnabled(!handler.isOpenHandler());
	panelBox->setTopElement(myLineEditor);

	myTreeView = new W32TreeView(ICON_SIZE);
	myTreeView->setVisible(true);
	myTreeView->addListener(this);
	panelBox->setCenterElement(myTreeView);

	W32HBox *buttonBox = new W32HBox();
	panelBox->setBottomElement(buttonBox);
	buttonBox->setHomogeneous(true);
	buttonBox->setAlignment(W32HBox::RIGHT);
	buttonBox->setSpacing(4);
	buttonBox->setMargins(4, 4, 4, 4);
	myOkButton = new W32PushButton(ZLDialogManager::buttonName(ZLDialogManager::OK_BUTTON));
	myOkButton->addListener(this);
	buttonBox->addElement(myOkButton);
	buttonBox->addElement(new W32PushButton(ZLDialogManager::buttonName(ZLDialogManager::CANCEL_BUTTON), W32PushButton::CANCEL_BUTTON));
	buttonBox->setVisible(true);

	update();
}

ZLWin32SelectionDialog::~ZLWin32SelectionDialog() {
	for(std::map<std::string,HICON>::iterator it = myIcons.begin(); it != myIcons.end(); ++it) {
		DestroyIcon(it->second);
	}
}

HICON ZLWin32SelectionDialog::getIcon(const ZLTreeNodePtr node) {
	const std::string &pixmapName = node->pixmapName();
	std::map<std::string,HICON>::const_iterator it = myIcons.find(pixmapName);
	if (it == myIcons.end()) {
		std::string imageFileName = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + pixmapName + ".ico";
		ZLFile file(imageFileName);
		HICON icon = (HICON)LoadImageA(0, file.path().c_str(), IMAGE_ICON, ICON_SIZE, ICON_SIZE, LR_LOADFROMFILE);
		myIcons[pixmapName] = icon;
		return icon;
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
		myTreeView->insert((*it)->displayName(), getIcon(*it));
	}
}

void ZLWin32SelectionDialog::selectItem(int index) {
	myTreeView->select(index);
}

bool ZLWin32SelectionDialog::run() {
	myWindow.blockMouseEvents(true);
	bool result = myPanel.runDialog(myWidth, myHeight);
	W32Widget::Size size = myPanel.size();
	myWidth = size.Width;
	myHeight = size.Height;
	myWindow.blockMouseEvents(false);
	return result;
}

void ZLWin32SelectionDialog::exitDialog() {
	myPanel.endDialog(true);
}

void ZLWin32SelectionDialog::setSize(int width, int height) {
	myWidth = width;
	myHeight = height;
}

int ZLWin32SelectionDialog::width() const {
	return myWidth;
}

int ZLWin32SelectionDialog::height() const {
	return myHeight;
}

void ZLWin32SelectionDialog::onEvent(const std::string &event, W32EventSender&) {
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

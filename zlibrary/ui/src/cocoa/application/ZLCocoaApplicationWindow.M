/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#import <Cocoa/Cocoa.h>

#include <ZLibrary.h>

#include "ZLCocoaApplicationWindow.h"

#import "ZLCocoaToolbarDelegate.h"

//#include "../util/ZLCocoaKeyUtil.h"
//#include "../util/ZLCocoaSignalUtil.h"
//#include "../dialogs/ZLCocoaDialogManager.h"
#include "../view/ZLCocoaViewWidget.h"

ZLCocoaApplicationWindow::ZLCocoaApplicationWindow(ZLApplication *application) : ZLApplicationWindow(application), myWindow(0) {
}

void ZLCocoaApplicationWindow::init() {
	myWindow = [[[NSApplication sharedApplication] windows] objectAtIndex:0];

	ZLApplicationWindow::init();

	/*
	NSMenu *menu = [[NSApplication sharedApplication] mainMenu];
	NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:@"Hello"
																								action:0
																				 keyEquivalent:@""];
	std::cerr << [[menu itemArray] count] << "\n";
	[item setHidden:NO];
	[menu addItem:item];
	[item setHidden:NO];
	std::cerr << [[menu itemArray] count] << "\n";
	[menu update];
	std::cerr << [[menu itemArray] count] << "\n";
	*/
}

ZLCocoaApplicationWindow::~ZLCocoaApplicationWindow() {
}

void ZLCocoaApplicationWindow::refresh() {
	ZLApplicationWindow::refresh();
}

void ZLCocoaApplicationWindow::processAllEvents() {
}

void ZLCocoaApplicationWindow::setToggleButtonState(const ZLToolbar::ToggleButtonItem &button) {
}

void ZLCocoaApplicationWindow::setFullscreen(bool fullscreen) {
}

bool ZLCocoaApplicationWindow::isFullscreen() const {
	return false;
}

void ZLCocoaApplicationWindow::addToolbarItem(ZLToolbar::ItemPtr item) {
	addToolbarItem(item, -1);
}

void ZLCocoaApplicationWindow::addToolbarItem(ZLToolbar::ItemPtr item, int index) {
	NSToolbar *toolbar = [(NSWindow*)myWindow toolbar];
	NSString *identifier = 0;

	switch (item->type()) {
		case ZLToolbar::Item::TEXT_FIELD:
		{
			const ZLToolbar::ParameterItem &parameterItem =
				(const ZLToolbar::ParameterItem&)*item;
			identifier = [NSString stringWithUTF8String:parameterItem.actionId().c_str()];
			if (index == -1) {
				ZLCocoaToolbarDelegate *delegate =
					(ZLCocoaToolbarDelegate*)[toolbar delegate];
				NSString *label = [NSString stringWithUTF8String:parameterItem.label().c_str()];
				NSString *tooltip = [NSString stringWithUTF8String:parameterItem.tooltip().c_str()];
				[delegate addTextItemWithIdentifier:identifier label:label tooltip:tooltip];
			}
			break;
		}
		case ZLToolbar::Item::COMBO_BOX:
			break;
		case ZLToolbar::Item::SEARCH_FIELD:
		{
			const ZLToolbar::ParameterItem &parameterItem =
				(const ZLToolbar::ParameterItem&)*item;
			identifier = [NSString stringWithUTF8String:parameterItem.actionId().c_str()];
			if (index == -1) {
				ZLCocoaToolbarDelegate *delegate =
					(ZLCocoaToolbarDelegate*)[toolbar delegate];
				NSString *label = [NSString stringWithUTF8String:parameterItem.label().c_str()];
				NSString *tooltip = [NSString stringWithUTF8String:parameterItem.tooltip().c_str()];
				[delegate addSearchItemWithIdentifier:identifier label:label tooltip:tooltip];
			}
			break;
		}
		case ZLToolbar::Item::PLAIN_BUTTON:
		case ZLToolbar::Item::TOGGLE_BUTTON:
		case ZLToolbar::Item::MENU_BUTTON:
		{
			const ZLToolbar::AbstractButtonItem &button =
				(const ZLToolbar::AbstractButtonItem&)*item;
			identifier = [NSString stringWithUTF8String:button.actionId().c_str()];
			if (index == -1) {
				ZLCocoaToolbarDelegate *delegate =
					(ZLCocoaToolbarDelegate*)[toolbar delegate];
				NSString *label = [NSString stringWithUTF8String:button.label().c_str()];
				NSString *tooltip = [NSString stringWithUTF8String:button.tooltip().c_str()];
				[delegate addButtonItemWithIdentifier:identifier label:label tooltip:tooltip];
			}
			break;
		}
		case ZLToolbar::Item::SEPARATOR:
			identifier = NSToolbarSeparatorItemIdentifier;
			break;
		case ZLToolbar::Item::FILL_SEPARATOR:
			identifier = NSToolbarFlexibleSpaceItemIdentifier;
			break;
	}

	if (identifier != 0) {
		if (index >= 0) {
			[toolbar insertItemWithItemIdentifier:identifier
																		atIndex:index];
		} else {
			[toolbar insertItemWithItemIdentifier:identifier
																		atIndex:[[toolbar items] count]];
			myToolbarItems.push_back(std::make_pair(item, true));
		}
	}
}

void ZLCocoaApplicationWindow::setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled) {
	NSToolbar *toolbar = [(NSWindow*)myWindow toolbar];

	int index = 0;
	std::pair<ZLToolbar::ItemPtr,bool> *pair = 0;
	for (std::vector<std::pair<ZLToolbar::ItemPtr,bool> >::iterator it = myToolbarItems.begin(); it != myToolbarItems.end(); ++it) {
		if (it->first == item) {
			pair = &*it;
			break;
		}
		if (it->second) {
			++index;
		}
	}
	if (pair != 0) {
		if (pair->second != visible) {
			pair->second = visible;
			if (visible) {
				addToolbarItem(item, index);
			} else {
				[toolbar removeItemAtIndex:index];
			}
		}
		if (visible) {
			[[[toolbar items] objectAtIndex:index] setTarget:enabled ? [toolbar delegate] : nil];
		}
	}
}

ZLViewWidget *ZLCocoaApplicationWindow::createViewWidget() {
	return new ZLCocoaViewWidget(&application(), (ZLView::Angle)application().AngleStateOption.value());
}

void ZLCocoaApplicationWindow::close() {
	[(NSWindow*)myWindow close];
}

void ZLCocoaApplicationWindow::grabAllKeys(bool) {
}

void ZLCocoaApplicationWindow::setHyperlinkCursor(bool hyperlink) {
}

void ZLCocoaApplicationWindow::setCaption(const std::string &caption) {
	[(NSWindow*)myWindow setTitle:[NSString stringWithUTF8String:caption.c_str()]];
}

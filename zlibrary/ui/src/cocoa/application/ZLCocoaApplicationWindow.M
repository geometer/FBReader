/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#import <Cocoa/Cocoa.h>

#include <ZLibrary.h>

#include "ZLCocoaApplicationWindow.h"

//#include "../util/ZLCocoaKeyUtil.h"
//#include "../util/ZLCocoaSignalUtil.h"
//#include "../dialogs/ZLCocoaDialogManager.h"
#include "../view/ZLCocoaViewWidget.h"

ZLCocoaApplicationWindow::ZLCocoaApplicationWindow(ZLApplication *application) : ZLApplicationWindow(application) {
}

#include <iostream>

void ZLCocoaApplicationWindow::init() {
	ZLApplicationWindow::init();
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
}

void ZLCocoaApplicationWindow::setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled) {
}

ZLViewWidget *ZLCocoaApplicationWindow::createViewWidget() {
	return new ZLCocoaViewWidget(&application(), (ZLView::Angle)application().AngleStateOption.value());
}

void ZLCocoaApplicationWindow::close() {
}

void ZLCocoaApplicationWindow::grabAllKeys(bool) {
}

void ZLCocoaApplicationWindow::setHyperlinkCursor(bool hyperlink) {
}

void ZLCocoaApplicationWindow::setCaption(const std::string &caption) {
}

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

#include <ZLibrary.h>

#include "../application/ZLCocoaApplicationWindow.h"

#import "ZLCocoaAppDelegate.h"

@implementation ZLCocoaAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
	((ZLCocoaApplicationWindow&)ZLCocoaApplicationWindow::Instance()).restoreSizeAndPosition(window);
}

- (void)applicationWillTerminate:(NSNotification*)notification {
	((ZLCocoaApplicationWindow&)ZLCocoaApplicationWindow::Instance()).saveSizeAndPosition(window);
	ZLApplication::deleteInstance();
	ZLibrary::shutdown();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
	return YES;
}

@end

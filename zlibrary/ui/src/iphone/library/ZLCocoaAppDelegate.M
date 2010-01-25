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

#include <ZLibrary.h>
#include <ZLApplication.h>

#include "../application/ZLCocoaApplicationWindow.h"

#import "ZLCocoaAppDelegate.h"
#import "../../../../../iphone/Classes/iphoneViewController.h"

@implementation ZLCocoaAppDelegate

@synthesize window;
@synthesize viewController;

- (void)applicationDidFinishLaunching:(UIApplication*)application {    
	ZLApplication::Instance().initWindow();

	[window addSubview:viewController.view];
	[window makeKeyAndVisible];
}

- (void)applicationWillTerminate:(UIApplication*)application {
	ZLApplication::deleteInstance();
	ZLibrary::shutdown();
}

- (void)dealloc {
	[viewController release];
	[window release];
	[super dealloc];
}

@end

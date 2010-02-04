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
#include <ZLFile.h>

#import "ZLCocoaToolbarDelegate.h"

@implementation ZLCocoaToolbarDelegate

- (id)init {
	self = [super init];
	if (self) {
		myItems = [[NSMutableDictionary alloc] init];
		mySelectableItems = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void)addSearchItemWithIdentifier:(NSString*)identifier label:(NSString*)label tooltip:(NSString*)tooltip {
	NSToolbarItem *item = [[[NSToolbarItem alloc] initWithItemIdentifier:identifier] autorelease];

	NSSearchField *searchField = [[[NSSearchField alloc] init] autorelease];
	[item setView:searchField];
	//[item setView:[[[NSSearchField alloc] init] autorelease]];
	//[item setMinSize:[[item view] bounds].size];
	//[item setMaxSize:[[item view] bounds].size];
	[item setMinSize:NSMakeSize(200, 30)];
	[item setMaxSize:NSMakeSize(200, 30)];

	[item setLabel:label];
	[item setToolTip:tooltip];
	[item setTarget:self];
	[item setAction:@selector(doAction:)];
	[myItems setObject:item forKey:identifier];
}

- (void)addTextItemWithIdentifier:(NSString*)identifier label:(NSString*)label tooltip:(NSString*)tooltip {
	NSToolbarItem *item = [[[NSToolbarItem alloc] initWithItemIdentifier:identifier] autorelease];

	NSTextField *textField = [[[NSTextField alloc] init] autorelease];
	[item setView:textField];
	//[item setView:[[[NSSearchField alloc] init] autorelease]];
	//[item setMinSize:[[item view] bounds].size];
	//[item setMaxSize:[[item view] bounds].size];
	[item setMinSize:NSMakeSize(200, 30)];
	[item setMaxSize:NSMakeSize(200, 30)];

	[item setLabel:label];
	[item setToolTip:tooltip];
	[item setTarget:self];
	[item setAction:@selector(doAction:)];
	[myItems setObject:item forKey:identifier];
}

- (void)addButtonItemWithIdentifier:(NSString*)identifier label:(NSString*)label tooltip:(NSString*)tooltip {
	NSToolbarItem *item = [[[NSToolbarItem alloc] initWithItemIdentifier:identifier] autorelease];
	ZLFile file(ZLibrary::ApplicationImageDirectory() + "/" + [identifier UTF8String] + ".png");
	[item setImage:[[NSImage alloc] initWithContentsOfFile:[NSString stringWithUTF8String:file.path().c_str()]]];
	[item setLabel:label];
	[item setToolTip:tooltip];
	[item setTarget:self];
	[item setAction:@selector(doAction:)];
	[myItems setObject:item forKey:identifier];
}

- (NSToolbarItem*)toolbar:(NSToolbar*)toolbar itemForItemIdentifier:(NSString*)identifier willBeInsertedIntoToolbar:(BOOL)flag {
	return [myItems objectForKey:identifier];
}

- (NSArray*)toolbarAllowedItemIdentifiers:(NSToolbar*)toolbar {
	return [myItems allKeys];
}

- (NSArray*)toolbarDefaultItemIdentifiers:(NSToolbar*)toolbar {
	return [myItems allKeys];
}

- (NSArray*)toolbarSelectableItemIdentifiers:(NSToolbar*)toolbar {
	return mySelectableItems;
}

- (void)toolbarWillAddItem:(NSNotification*)notification {
}

- (void)toolbarDidRemoveItem:(NSNotification*)notification {
}

- (void)doAction:(id)sender {
	ZLApplication::Instance().doAction([[sender itemIdentifier] UTF8String]);
}

@end

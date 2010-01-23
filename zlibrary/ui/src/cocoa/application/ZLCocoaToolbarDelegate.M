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

#import "ZLCocoaToolbarDelegate.h"

@implementation ZLCocoaToolbarDelegate

- (id)init {
	self = [super init];
	if (self) {
		myItems = [NSMutableDictionary new];
		NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:@"item0"];
		[item setLabel:@"Item"];
		[myItems setObject:item forKey:@"item0"];
	}
	return self;
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
	return [myItems allKeys];
}

- (void)toolbarWillAddItem:(NSNotification*)notification {
}

- (void)toolbarDidRemoveItem:(NSNotification*)notification {
}

@end

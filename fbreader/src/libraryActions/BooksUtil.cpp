/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLDialogManager.h>
#include <ZLStringUtil.h>

#include "BooksUtil.h"

#include "../library/Library.h"
#include "../library/Tag.h"
#include "../fbreader/FBReader.h"

void BooksUtil::removeTag(shared_ptr<Tag> tag) {
	ZLResourceKey boxKey("removeTagBox");
	const std::string message =
		ZLStringUtil::printf(ZLDialogManager::dialogMessage(boxKey), tag->fullName());
	enum { REMOVE_TAG, REMOVE_SUBTREE, DONT_REMOVE } code = DONT_REMOVE;

	Library &library = Library::Instance();
	if (library.hasSubtags(tag)) {
		if (library.hasBooks(tag)) {
			switch (ZLDialogManager::Instance().questionBox(boxKey, message,
								ZLResourceKey("thisOnly"),
								ZLResourceKey("withSubtags"),
								ZLDialogManager::CANCEL_BUTTON
							)) {
				case 0:
					code = REMOVE_TAG;
					break;
				case 1:
					code = REMOVE_SUBTREE;
					break;
			}
		} else {
			if (ZLDialogManager::Instance().questionBox(boxKey, message,
				ZLResourceKey("withSubtags"), ZLDialogManager::CANCEL_BUTTON) == 0) {
				code = REMOVE_SUBTREE;
			}
		}
	} else {
		if (ZLDialogManager::Instance().questionBox(boxKey, message,
			ZLDialogManager::YES_BUTTON, ZLDialogManager::CANCEL_BUTTON) == 0) {
			code = REMOVE_TAG;
		}
	}
	if (code != DONT_REMOVE) {
		library.removeTag(tag, code == REMOVE_SUBTREE);
		// TODO: select current node (?) again
		FBReader::Instance().refreshWindow();
	}
}

void BooksUtil::collectTagsFromLibrary(TagList &tags) {
	const TagList &lTags = Library::Instance().tags();
	TagSet tagSet;

	for (TagList::const_iterator it = lTags.begin(); it != lTags.end(); ++it) {
		shared_ptr<Tag> tag = *it;
		if (tag.isNull()) {
			tagSet.insert(tag);
			tags.push_back(tag);
		} else {
			TagList tagStack;
			do {
				tagStack.push_back(tag);
				tag = tag->parent();
			} while (!tag.isNull() && tagSet.find(tag) == tagSet.end());
			tagSet.insert(tagStack.begin(), tagStack.end());
			tags.insert(tags.end(), tagStack.rbegin(), tagStack.rend());
		}
	}
}

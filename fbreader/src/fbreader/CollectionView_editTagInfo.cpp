/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
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
#include <ZLDialog.h>

#include "CollectionView.h"

#include "../options/FBOptions.h"
#include "../collection/BookCollection.h"

const std::string CollectionView::SpecialTagAllBooks = ",AllBooks,";
const std::string CollectionView::SpecialTagNoTagsBooks = ",NoTags,";

void CollectionView::editTagInfo(const std::string &tag) {
	if (tag.empty()) {
		return;
	}

	shared_ptr<ZLDialog> dialog = ZLDialogManager::instance().createDialog(ZLResourceKey("editTagInfoDialog"));

	ZLBooleanOption cloneTagOption(FBCategoryKey::TAGS, "TagInfo", "Clone", false);
	ZLStringOption tagNameOption(FBCategoryKey::TAGS, "TagInfo", "Name", tag);
	ZLBooleanOption includeSubtagsOption(FBCategoryKey::TAGS, "TagInfo", "IncludeSubtags", true);

	dialog->addOption(ZLResourceKey("name"), tagNameOption);
	dialog->addOption(ZLResourceKey("includeSubtags"), includeSubtagsOption);

	dialog->addButton(ZLDialogManager::OK_BUTTON, true);
	dialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);

	if (dialog->run()) {
	}
}

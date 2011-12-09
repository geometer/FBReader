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

#include <algorithm>

#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>
#include <ZLFile.h>
#include <ZLLanguageList.h>
#include <ZLStringUtil.h>

#include <optionEntries/ZLStringInfoEntry.h>
#include <optionEntries/ZLSimpleOptionEntry.h>
#include <optionEntries/ZLLanguageOptionEntry.h>

#include "MobileBookInfoDialog.h"

#include "../library/Library.h"
#include "../encodingOption/EncodingOptionEntry.h"
#include "../library/Book.h"
#include "../library/Tag.h"
#include "../library/Author.h"
#include "../tree/FBNode.h"

class MobileDisplayNameEntry : public ZLStaticTextOptionEntry {

public:
	MobileDisplayNameEntry(const std::string& value);
	const std::string &initialValue() const;
	void setValue(const std::string& value);
private:
	std::string myValue;
};

MobileDisplayNameEntry::MobileDisplayNameEntry(const std::string &value) : myValue(value) {
}

const std::string& MobileDisplayNameEntry::initialValue() const {
	return myValue;
}

MobileBookInfoDialog::MobileBookInfoDialog(shared_ptr<Book> book) : myBook(book) {
	//TODO add fullscreen for dialog
	myDialog = ZLDialogManager::Instance().createDialog(resourceKey());
        fillContent(*myDialog, LocalBookInfo(myBook),true);
	myDialog->addButton(ZLResourceKey("back"), true);
}


void MobileBookInfoDialog::fillContent(ZLDialogContent &content, const AbstractBookInfo &info, bool showCover) {
        doFillContent(content, info, showCover);
}

void MobileBookInfoDialog::fillContent(ZLDialog &content, const AbstractBookInfo &info, bool showCover) {
        doFillContent(content, info, showCover);
}

template <typename T>
void MobileBookInfoDialog::doFillContent(T &content, const AbstractBookInfo &book, bool showCover) {

        if (showCover) {
            content.addOption(ZLResourceKey("cover"), new ZLPictureOptionEntry(book.image()));
        }

	content.addOption(ZLResourceKey("title"), new MobileDisplayNameEntry(book.title()));
	
        if (!book.authors().empty()) {
            std::string authors = ZLStringUtil::join(book.authors(), FBNode::COMMA_JOIN_SEPARATOR);
            content.addOption(ZLResourceKey("authorDisplayName"), new MobileDisplayNameEntry(authors));
        }
	
	if (!book.tags().empty()) {
		std::string tags = ZLStringUtil::join(book.tags(), "; ");
		content.addOption(ZLResourceKey("tags"), new MobileDisplayNameEntry(tags));
	}
	
        if (!book.language().empty()) {
            std::string language = ZLLanguageList::languageName(book.language());
            content.addOption(ZLResourceKey("language"), new MobileDisplayNameEntry(language));
        }
	
	if (!book.seriesTitle().empty())
		content.addOption(ZLResourceKey("seriesTitle"), new MobileDisplayNameEntry(book.seriesTitle()));
	
	std::string filePath = book.file();
	if (!filePath.empty())
		content.addOption(ZLResourceKey("file"), new MobileDisplayNameEntry(filePath));
}

ZLResourceKey MobileBookInfoDialog::resourceKey() {
	return ZLResourceKey("MobileInfoDialog");
}

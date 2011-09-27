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

class TagJoinFunctor {
public:
	std::string operator()(const shared_ptr<Tag> tag) const;
};

std::string TagJoinFunctor::operator()(const shared_ptr<Tag> tag) const {
	return tag->fullName();
}

class AuthorJoinFunctor {
public:
	std::string operator()(const shared_ptr<Author> author) const;
};

std::string AuthorJoinFunctor::operator()(const shared_ptr<Author> author) const {
	return author->name();
}

MobileBookInfoDialog::MobileBookInfoDialog(shared_ptr<Book> book) : myBook(book) {
	//TODO add fullscreen for dialog
	myDialog = ZLDialogManager::Instance().createDialog(ZLResourceKey("MobileInfoDialog"));
	myDialog->addOption(ZLResourceKey("title"), new MobileDisplayNameEntry(myBook->title()));

	std::string authors = ZLStringUtil::join(myBook->authors(), ", ", AuthorJoinFunctor());
	myDialog->addOption(ZLResourceKey("authorDisplayName"), new MobileDisplayNameEntry(authors));

	std::string tags = ZLStringUtil::join(myBook->tags(), "; ", TagJoinFunctor());
	myDialog->addOption(ZLResourceKey("tags"), new MobileDisplayNameEntry(tags));

	std::string language = ZLLanguageList::languageName(myBook->language());
	myDialog->addOption(ZLResourceKey("language"), new MobileDisplayNameEntry(language));

	//TODO add seriers showing
//	std::string series;
//	const std::vector<std::string> & seriesList = MobileSeriesTitleEntry(*this).values();
//	for (size_t i=0; i < seriesList.size(); ++i) {
//		//TODO add separator here
//		series += seriesList.at(i);
//	}
//	myTab->addOption(ZLResourceKey("seriesTitle"), new MobileDisplayNameEntry(myBook->seriesTitle()));

	myDialog->addOption(ZLResourceKey("file"),	new MobileDisplayNameEntry(ZLFile::fileNameToUtf8(book->file().path())));

	myDialog->addButton(ZLResourceKey("back"),true);
}

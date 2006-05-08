/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <cctype>
#include <algorithm>

#include <abstract/ZLOptions.h>
#include <abstract/ZLOptionsDialog.h>
#include <abstract/ZLOptionEntry.h>
#include <abstract/ZLFSManager.h>

#include "../../textview/TextView.h"
#include "../../textview/TextStyle.h"
#include "../../textview/TextStyleOptions.h"

#include "RtfFormat.h"

const std::string OPTION_ConvertStyles = "CreateContentsTable";

void RtfInfoPage::ComboOptionEntry::onValueChange(const std::string &selectedValue) {
//	for (std::map<ZLOptionEntry*,std::string>::const_iterator it = myPage.myEntries.begin(); it != myPage.myEntries.end(); it++) {
//		it->first->setVisible(it->second == selectedValue);
//	}
}

RtfFormat::RtfFormat(const std::string &fileName) :
    ConvertStylesOption(fileName, OPTION_ConvertStyles, false) {
}

RtfInfoPage::RtfInfoPage(ZLOptionsDialog &dialog, const std::string &fileName, const std::string &tabName, bool showContentsEntry) : myFormat(fileName) {
	ZLOptionsDialogTab *tab = dialog.createTab(tabName);
	
	myConvertStyles = new ZLSimpleBooleanOptionEntry("Convert styles", myFormat.ConvertStylesOption);
	tab->addOption(myConvertStyles);

	myStyleComboEntry = new ComboOptionEntry(*this, "Internal style:", "Base");
//	myComboEntry->addValue(myComboEntry->initialValue());

	TextStyleCollection &collection = TextStyleCollection::instance();
	const int STYLES_NUMBER = 22;
	TextKind styles[STYLES_NUMBER] = { REGULAR, TITLE, SECTION_TITLE, SUBTITLE, CONTENTS_TABLE_ENTRY, RECENT_BOOK_LIST, LIBRARY_AUTHOR_ENTRY, LIBRARY_BOOK_ENTRY, ANNOTATION, EPIGRAPH, AUTHOR, DATE, POEM_TITLE, STANZA, VERSE, CITE, HYPERLINK, FOOTNOTE, EMPHASIS, STRONG, PREFORMATTED, CODE };
	for (int i = 0; i < STYLES_NUMBER; i++) {
		const TextStyleDecoration *decoration = collection.decoration(styles[i]);
		if (decoration != 0) {
			myStyleComboEntry->addValue(decoration->name());
		}
	}
	tab->addOption(myStyleComboEntry);
}

RtfInfoPage::~RtfInfoPage() {
}

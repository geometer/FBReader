/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <ZLOptionsDialog.h>
#include <ZLPaintContext.h>

#include <optionEntries/ZLSimpleOptionEntry.h>

#include "StyleOptionsPage.h"

#include "../textview/TextView.h"
#include "../textview/TextStyle.h"
#include "../textview/TextStyleOptions.h"

static const std::string BOLD_STRING = "Bold";
static const std::string ITALIC_STRING = "Italic";
static const std::string FAMILY_STRING = "Family";

StyleOptionsPage::StyleOptionsPage(ZLDialogContent &dialogTab, ZLPaintContext &context) {
	myComboEntry = new ComboOptionEntry(*this, "Options For", "Base");
	myComboEntry->addValue(myComboEntry->initialValue());

	TextStyleCollection &collection = TextStyleCollection::instance();
	const int STYLES_NUMBER = 32;
	TextKind styles[STYLES_NUMBER] = { REGULAR, TITLE, SECTION_TITLE, SUBTITLE, H1, H2, H3, H4, H5, H6, CONTENTS_TABLE_ENTRY, RECENT_BOOK_LIST, LIBRARY_AUTHOR_ENTRY, LIBRARY_BOOK_ENTRY, ANNOTATION, EPIGRAPH, AUTHOR, DATE, POEM_TITLE, STANZA, VERSE, CITE, HYPERLINK, FOOTNOTE, ITALIC, EMPHASIS, BOLD, STRONG, DEFINITION, DEFINITION_DESCRIPTION, PREFORMATTED, CODE };
	for (int i = 0; i < STYLES_NUMBER; ++i) {
		const TextStyleDecoration *decoration = collection.decoration(styles[i]);
		if (decoration != 0) {
			myComboEntry->addValue(decoration->name());
		}
	}
	dialogTab.addOption(myComboEntry);

	{
		const std::string &name = myComboEntry->initialValue();
		BaseTextStyle &baseStyle = collection.baseStyle();

		registerEntries(dialogTab,
			new ZLFontFamilyOptionEntry(FAMILY_STRING, baseStyle.FontFamilyOption, context),
			new ZLSimpleBooleanOptionEntry(BOLD_STRING, baseStyle.BoldOption),
			name
		);

		registerEntries(dialogTab,
			new ZLSimpleSpinOptionEntry("Size", baseStyle.FontSizeOption, 2),
			new ZLSimpleBooleanOptionEntry(ITALIC_STRING, baseStyle.ItalicOption),
			name
		);

		registerEntry(dialogTab,
			new ZLSimpleBooleanOptionEntry("Auto Hyphenations", baseStyle.AutoHyphenationOption),
			name
		);
	}

	for (int i = 0; i < STYLES_NUMBER; ++i) {
		TextStyleDecoration *decoration = collection.decoration(styles[i]);
		if (decoration != 0) {
			const std::string &name = decoration->name();

			registerEntries(dialogTab,
				new FontFamilyWithBaseOptionEntry(FAMILY_STRING, decoration->FontFamilyOption, context),
				new ZLSimpleBoolean3OptionEntry(BOLD_STRING, decoration->BoldOption),
				name
			);

			registerEntries(dialogTab,
				new ZLSimpleSpinOptionEntry("Size Difference", decoration->FontSizeDeltaOption, 2),
				new ZLSimpleBoolean3OptionEntry(ITALIC_STRING, decoration->ItalicOption),
				name
			);

			registerEntries(dialogTab,
				new ZLSimpleBoolean3OptionEntry("Allow Hyphenations", decoration->AllowHyphenationsOption),
				0,
				name
			);
		}
	}

	myComboEntry->onValueSelected(0);
}

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

static const ZLResourceKey KEY_BOLD("bold");
static const ZLResourceKey KEY_ITALIC("italic");
static const ZLResourceKey KEY_FONTFAMILY("fontFamily");
static const ZLResourceKey KEY_FONTSIZE("fontSize");
static const ZLResourceKey KEY_FONTSIZEDIFFERENCE("fontSizeDifference");
static const ZLResourceKey KEY_ALLOWHYPHENATIONS("allowHyphenations");
static const ZLResourceKey KEY_AUTOHYPHENATIONS("autoHyphenations");
static const ZLResourceKey KEY_DUMMY("");

StyleOptionsPage::StyleOptionsPage(ZLDialogContent &dialogTab, ZLPaintContext &context) {
	myComboEntry = new ComboOptionEntry(*this, "Base");
	myComboEntry->addValue(myComboEntry->initialValue());

	TextStyleCollection &collection = TextStyleCollection::instance();
	TextKind styles[] = { REGULAR, TITLE, SECTION_TITLE, SUBTITLE, H1, H2, H3, H4, H5, H6, CONTENTS_TABLE_ENTRY, RECENT_BOOK_LIST, LIBRARY_AUTHOR_ENTRY, LIBRARY_BOOK_ENTRY, ANNOTATION, EPIGRAPH, AUTHOR, DATE, POEM_TITLE, STANZA, VERSE, CITE, INTERNAL_HYPERLINK, EXTERNAL_HYPERLINK, FOOTNOTE, ITALIC, EMPHASIS, BOLD, STRONG, DEFINITION, DEFINITION_DESCRIPTION, PREFORMATTED, CODE };
	const int STYLES_NUMBER = sizeof(styles) / sizeof(TextKind);
	for (int i = 0; i < STYLES_NUMBER; ++i) {
		const TextStyleDecoration *decoration = collection.decoration(styles[i]);
		if (decoration != 0) {
			myComboEntry->addValue(decoration->name());
		}
	}
	dialogTab.addOption(ZLResourceKey("optionsFor"), myComboEntry);

	{
		const std::string &name = myComboEntry->initialValue();
		BaseTextStyle &baseStyle = collection.baseStyle();

		registerEntries(dialogTab,
			KEY_FONTFAMILY, new ZLFontFamilyOptionEntry(baseStyle.FontFamilyOption, context),
			KEY_BOLD, new ZLSimpleBooleanOptionEntry(baseStyle.BoldOption),
			name
		);

		registerEntries(dialogTab,
			KEY_FONTSIZE, new ZLSimpleSpinOptionEntry(baseStyle.FontSizeOption, 2),
			KEY_ITALIC, new ZLSimpleBooleanOptionEntry(baseStyle.ItalicOption),
			name
		);

		registerEntry(dialogTab,
			KEY_AUTOHYPHENATIONS, new ZLSimpleBooleanOptionEntry(baseStyle.AutoHyphenationOption),
			name
		);
	}

	for (int i = 0; i < STYLES_NUMBER; ++i) {
		TextStyleDecoration *decoration = collection.decoration(styles[i]);
		if (decoration != 0) {
			const std::string &name = decoration->name();

			registerEntries(dialogTab,
				KEY_FONTFAMILY, new FontFamilyWithBaseOptionEntry(decoration->FontFamilyOption, context),
				KEY_BOLD, new ZLSimpleBoolean3OptionEntry(decoration->BoldOption),
				name
			);

			registerEntries(dialogTab,
				KEY_FONTSIZEDIFFERENCE, new ZLSimpleSpinOptionEntry(decoration->FontSizeDeltaOption, 2),
				KEY_ITALIC, new ZLSimpleBoolean3OptionEntry(decoration->ItalicOption),
				name
			);

			registerEntries(dialogTab,
				KEY_ALLOWHYPHENATIONS, new ZLSimpleBoolean3OptionEntry(decoration->AllowHyphenationsOption),
				KEY_DUMMY, 0,
				name
			);
		}
	}

	myComboEntry->onValueSelected(0);
}

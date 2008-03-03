/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLTextView.h>
#include <ZLTextStyle.h>
#include <ZLTextStyleOptions.h>

#include "StyleOptionsPage.h"

#include "../bookmodel/FBTextKind.h"

static const ZLResourceKey KEY_STYLE("style");
static const ZLResourceKey KEY_BASE("Base");

static const ZLResourceKey KEY_BOLD("bold");
static const ZLResourceKey KEY_ITALIC("italic");
static const ZLResourceKey KEY_FONTFAMILY("fontFamily");
static const ZLResourceKey KEY_FONTSIZE("fontSize");
static const ZLResourceKey KEY_FONTSIZEDIFFERENCE("fontSizeDifference");
static const ZLResourceKey KEY_ALLOWHYPHENATIONS("allowHyphenations");
static const ZLResourceKey KEY_AUTOHYPHENATIONS("autoHyphenations");
static const ZLResourceKey KEY_DUMMY("");

StyleOptionsPage::StyleOptionsPage(ZLDialogContent &dialogTab, ZLPaintContext &context) {
	const ZLResource &styleResource = ZLResource::resource(KEY_STYLE);

	myComboEntry = new ComboOptionEntry(*this, styleResource[KEY_BASE].value());
	myComboEntry->addValue(myComboEntry->initialValue());

	ZLTextStyleCollection &collection = ZLTextStyleCollection::instance();
	ZLTextKind styles[] = { REGULAR, TITLE, SECTION_TITLE, SUBTITLE, H1, H2, H3, H4, H5, H6, CONTENTS_TABLE_ENTRY, RECENT_BOOK_LIST, LIBRARY_AUTHOR_ENTRY, LIBRARY_BOOK_ENTRY, ANNOTATION, EPIGRAPH, AUTHOR, DATEKIND, POEM_TITLE, STANZA, VERSE, CITE, INTERNAL_HYPERLINK, EXTERNAL_HYPERLINK, FOOTNOTE, ITALIC, EMPHASIS, BOLD, STRONG, DEFINITION, DEFINITION_DESCRIPTION, PREFORMATTED, CODE };
	const int STYLES_NUMBER = sizeof(styles) / sizeof(ZLTextKind);
	for (int i = 0; i < STYLES_NUMBER; ++i) {
		const ZLTextStyleDecoration *decoration = collection.decoration(styles[i]);
		if (decoration != 0) {
			myComboEntry->addValue(styleResource[decoration->name()].value());
		}
	}
	dialogTab.addOption(ZLResourceKey("optionsFor"), myComboEntry);

	{
		const std::string &name = myComboEntry->initialValue();
		ZLTextBaseStyle &baseStyle = collection.baseStyle();

		registerEntry(dialogTab,
			KEY_FONTFAMILY, new ZLFontFamilyOptionEntry(baseStyle.FontFamilyOption, context),
			name
		);

		registerEntry(dialogTab,
			KEY_FONTSIZE, new ZLSimpleSpinOptionEntry(baseStyle.FontSizeOption, 2),
			name
		);

		registerEntry(dialogTab,
			KEY_BOLD, new ZLSimpleBooleanOptionEntry(baseStyle.BoldOption),
			name
		);

		registerEntry(dialogTab,
			KEY_ITALIC, new ZLSimpleBooleanOptionEntry(baseStyle.ItalicOption),
			name
		);

		registerEntry(dialogTab,
			KEY_AUTOHYPHENATIONS, new ZLSimpleBooleanOptionEntry(baseStyle.AutoHyphenationOption),
			name
		);
	}

	for (int i = 0; i < STYLES_NUMBER; ++i) {
		ZLTextStyleDecoration *decoration = collection.decoration(styles[i]);
		if (decoration != 0) {
			const std::string &name = styleResource[decoration->name()].value();

			registerEntry(dialogTab,
				KEY_FONTFAMILY, new ZLTextFontFamilyWithBaseOptionEntry(decoration->FontFamilyOption, dialogTab.resource(KEY_FONTFAMILY), context),
				name
			);

			registerEntry(dialogTab,
				KEY_FONTSIZEDIFFERENCE, new ZLSimpleSpinOptionEntry(decoration->FontSizeDeltaOption, 2),
				name
			);

			registerEntry(dialogTab,
				KEY_BOLD, new ZLSimpleBoolean3OptionEntry(decoration->BoldOption),
				name
			);

			registerEntry(dialogTab,
				KEY_ITALIC, new ZLSimpleBoolean3OptionEntry(decoration->ItalicOption),
				name
			);

			registerEntry(dialogTab,
				KEY_ALLOWHYPHENATIONS, new ZLSimpleBoolean3OptionEntry(decoration->AllowHyphenationsOption),
				name
			);
		}
	}

	myComboEntry->onValueSelected(0);
}

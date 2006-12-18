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

#include <ZLOptionsDialog.h>

#include <optionEntries/ZLSimpleOptionEntry.h>

#include "FormatOptionsPage.h"

#include "../textview/TextView.h"
#include "../textview/TextStyle.h"
#include "../textview/TextStyleOptions.h"

FormatOptionsPage::FormatOptionsPage(ZLDialogContent &dialogTab) {
	myComboEntry = new ComboOptionEntry(*this, "Options For", "Base");
	myComboEntry->addValue(myComboEntry->initialValue());

	TextStyleCollection &collection = TextStyleCollection::instance();
	const int STYLES_NUMBER = 18;
	TextKind styles[STYLES_NUMBER] = { REGULAR, TITLE, SECTION_TITLE, SUBTITLE, H1, H2, H3, H4, H5, H6, ANNOTATION, EPIGRAPH, PREFORMATTED, AUTHOR, DATE, POEM_TITLE, STANZA, VERSE };
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
			new LineSpacingOptionEntry(baseStyle.LineSpaceOption, false),
			0,//new ZLSimpleSpinOptionEntry("First Line Indent", baseStyle.firstLineIndentDeltaOption(), -300, 300, 1),
			name
		);

		registerEntries(dialogTab,
			new AlignmentOptionEntry(baseStyle.AlignmentOption, false),
			0,
			name
		);
	}

	for (int i = 0; i < STYLES_NUMBER; ++i) {
		TextStyleDecoration *d = collection.decoration(styles[i]);
		if ((d != 0) && (d->isFullDecoration())) {
			FullTextStyleDecoration *decoration = (FullTextStyleDecoration*)d;
			const std::string &name = decoration->name();
			
			registerEntries(dialogTab,
				new ZLSimpleSpinOptionEntry("Space Before", decoration->SpaceBeforeOption, 1),
				new ZLSimpleSpinOptionEntry("Left Indent", decoration->LeftIndentOption, 1),
				name
			);
			
			registerEntries(dialogTab,
				new ZLSimpleSpinOptionEntry("Space After", decoration->SpaceAfterOption, 1),
				new ZLSimpleSpinOptionEntry("Right Indent", decoration->RightIndentOption, 1),
				name
			);
			
			registerEntries(dialogTab,
				new LineSpacingOptionEntry(decoration->LineSpaceOption, true),
				new ZLSimpleSpinOptionEntry("First Line Indent", decoration->FirstLineIndentDeltaOption, 1),
				name
			);

			registerEntries(dialogTab,
				new AlignmentOptionEntry(decoration->AlignmentOption, true),
				0,
				name
			);
		}
	}

	myComboEntry->onValueSelected(0);
}

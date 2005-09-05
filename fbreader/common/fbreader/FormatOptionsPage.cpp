/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <abstract/ZLOptionsDialog.h>

#include "FormatOptionsPage.h"

#include "../textview/TextView.h"
#include "../textview/TextStyle.h"
#include "../textview/TextStyleOptions.h"

FormatOptionsPage::FormatOptionsPage(ZLOptionsDialogTab *dialogTab) {
	myComboEntry = new ComboOptionEntry(*this, "Options For", "Base");
	myComboEntry->addValue(myComboEntry->initialValue());

	TextStyleCollection &collection = TextStyleCollection::instance();
	const int STYLES_NUMBER = 12;
	TextKind styles[STYLES_NUMBER] = { REGULAR, TITLE, SECTION_TITLE, SUBTITLE, ANNOTATION, EPIGRAPH, PREFORMATTED, AUTHOR, DATE, POEM_TITLE, STANZA, VERSE };
	for (int i = 0; i < STYLES_NUMBER; i++) {
		const TextStyleDecoration *decoration = collection.decoration(styles[i]);
		if (decoration != 0) {
			myComboEntry->addValue(decoration->name());
		}
	}
	dialogTab->addOption(myComboEntry);

	{
		const std::string &name = myComboEntry->initialValue();
		BaseTextStyle &baseStyle = (BaseTextStyle&)*collection.baseStyle();

		registerEntries(dialogTab,
			new LineSpacingOptionEntry(baseStyle.lineSpaceOption(), false),
			0,//new ZLSimpleSpinOptionEntry("First Line Indent", baseStyle.firstLineIndentDeltaOption(), -300, 300, 1),
			name
		);

		registerEntries(dialogTab,
			new AlignmentOptionEntry(baseStyle.alignmentOption(), false),
			0,
			name
		);
	}

	for (int i = 0; i < STYLES_NUMBER; i++) {
		TextStyleDecoration *d = collection.decoration(styles[i]);
		if ((d != 0) && (d->isFullDecoration())) {
			FullTextStyleDecoration *decoration = (FullTextStyleDecoration*)d;
			const std::string &name = decoration->name();
			
			registerEntries(dialogTab,
				new ZLSimpleSpinOptionEntry("Space Before", decoration->spaceBeforeOption(), 1),
				new ZLSimpleSpinOptionEntry("Left Indent", decoration->leftIndentOption(), 1),
				name
			);
			
			registerEntries(dialogTab,
				new ZLSimpleSpinOptionEntry("Space After", decoration->spaceAfterOption(), 1),
				new ZLSimpleSpinOptionEntry("Right Indent", decoration->rightIndentOption(), 1),
				name
			);
			
			registerEntries(dialogTab,
				new LineSpacingOptionEntry(decoration->lineSpaceOption(), true),
				new ZLSimpleSpinOptionEntry("First Line Indent", decoration->firstLineIndentDeltaOption(), 1),
				name
			);

			registerEntries(dialogTab,
				new AlignmentOptionEntry(decoration->alignmentOption(), true),
				0,
				name
			);
		}
	}

	myComboEntry->onValueChange(myComboEntry->initialValue());
}

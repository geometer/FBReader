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

#include <optionEntries/ZLSimpleOptionEntry.h>

#include <ZLTextStyle.h>
#include <ZLTextStyleOptions.h>

#include "FormatOptionsPage.h"

#include "../bookmodel/FBTextKind.h"

static const ZLResourceKey KEY_STYLE("style");
static const ZLResourceKey KEY_BASE("Base");

static const ZLResourceKey KEY_DUMMY("");
static const ZLResourceKey KEY_LINESPACING("lineSpacing");
static const ZLResourceKey KEY_FIRSTLINEINDENT("firstLineIndent");
static const ZLResourceKey KEY_ALIGNMENT("alignment");
static const ZLResourceKey KEY_SPACEBEFORE("spaceBefore");
static const ZLResourceKey KEY_SPACEAFTER("spaceAfter");
static const ZLResourceKey KEY_LEFTINDENT("leftIndent");
static const ZLResourceKey KEY_RIGHTINDENT("rightIndent");

FormatOptionsPage::FormatOptionsPage(ZLDialogContent &dialogTab) {
	const ZLResource &styleResource = ZLResource::resource(KEY_STYLE);

	myComboEntry = new ComboOptionEntry(*this, styleResource[KEY_BASE].value());
	myComboEntry->addValue(myComboEntry->initialValue());

	ZLTextStyleCollection &collection = ZLTextStyleCollection::instance();
	ZLTextKind styles[] = { REGULAR, TITLE, SECTION_TITLE, SUBTITLE, H1, H2, H3, H4, H5, H6, ANNOTATION, EPIGRAPH, PREFORMATTED, AUTHOR, DATEKIND, POEM_TITLE, STANZA, VERSE };
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

		registerEntries(dialogTab,
			KEY_LINESPACING, new ZLTextLineSpaceOptionEntry(baseStyle.LineSpacePercentOption, dialogTab.resource(KEY_LINESPACING), false),
			KEY_DUMMY, 0,//new ZLSimpleSpinOptionEntry("First Line Indent", baseStyle.firstLineIndentDeltaOption(), -300, 300, 1),
			name
		);

		registerEntries(dialogTab,
			KEY_ALIGNMENT, new ZLTextAlignmentOptionEntry(baseStyle.AlignmentOption, dialogTab.resource(KEY_ALIGNMENT), false),
			KEY_DUMMY, 0,
			name
		);
	}

	for (int i = 0; i < STYLES_NUMBER; ++i) {
		ZLTextStyleDecoration *d = collection.decoration(styles[i]);
		if ((d != 0) && (d->isFullDecoration())) {
			ZLTextFullStyleDecoration *decoration = (ZLTextFullStyleDecoration*)d;
			const std::string &name = styleResource[decoration->name()].value();
			
			registerEntries(dialogTab,
				KEY_SPACEBEFORE, new ZLSimpleSpinOptionEntry(decoration->SpaceBeforeOption, 1),
				KEY_LEFTINDENT, new ZLSimpleSpinOptionEntry(decoration->LeftIndentOption, 1),
				name
			);
			
			registerEntries(dialogTab,
				KEY_SPACEAFTER, new ZLSimpleSpinOptionEntry(decoration->SpaceAfterOption, 1),
				KEY_RIGHTINDENT, new ZLSimpleSpinOptionEntry(decoration->RightIndentOption, 1),
				name
			);
			
			registerEntries(dialogTab,
				KEY_LINESPACING, new ZLTextLineSpaceOptionEntry(decoration->LineSpacePercentOption, dialogTab.resource(KEY_LINESPACING), true),
				KEY_FIRSTLINEINDENT, new ZLSimpleSpinOptionEntry(decoration->FirstLineIndentDeltaOption, 1),
				name
			);

			registerEntries(dialogTab,
				KEY_ALIGNMENT, new ZLTextAlignmentOptionEntry(decoration->AlignmentOption, dialogTab.resource(KEY_ALIGNMENT), true),
				KEY_DUMMY, 0,
				name
			);
		}
	}

	myComboEntry->onValueSelected(0);
}

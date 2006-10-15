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

#include <abstract/ZLOptionsDialog.h>

#include "ColorOptionsPage.h"

#include "../textview/TextView.h"
#include "../textview/TextStyle.h"

ColorOptionsPage::ColorOptionsPage(ZLDialogContent &dialogTab) {
	myComboEntry = new ComboOptionEntry(*this, "Color For", "Background");
	myComboEntry->addValue("Background");
	myComboEntry->addValue("Regular Text");
	myComboEntry->addValue("Hyperlink Text");
	myComboEntry->addValue("Selected Text");
	myComboEntry->addValue("Tree Lines");
	myComboEntry->addValue("Position Indicator");

	dialogTab.addOptions(myComboEntry, 0);

	BaseTextStyle &baseStyle = TextStyleCollection::instance().baseStyle();
	registerEntry(dialogTab,
		new ZLSimpleColorOptionEntry(baseStyle.BackgroundColorOption),
		myComboEntry->values()[0]
	);
	registerEntry(dialogTab,
		new ZLSimpleColorOptionEntry(baseStyle.RegularTextColorOption),
		myComboEntry->values()[1]
	);
	registerEntry(dialogTab,
		new ZLSimpleColorOptionEntry(baseStyle.HyperlinkTextColorOption),
		myComboEntry->values()[2]
	);
	registerEntry(dialogTab,
		new ZLSimpleColorOptionEntry(baseStyle.SelectedTextColorOption),
		myComboEntry->values()[3]
	);
	registerEntry(dialogTab,
		new ZLSimpleColorOptionEntry(baseStyle.TreeLinesColorOption),
		myComboEntry->values()[4]
	);
	registerEntry(dialogTab,
		new ZLSimpleColorOptionEntry(TextStyleCollection::instance().indicatorStyle().ColorOption),
		myComboEntry->values()[5]
	);

	myComboEntry->onValueChange(myComboEntry->initialValue());
}

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

#include <ZLOptions.h>
#include <ZLDialogContent.h>

#include "ColorOptionsPage.h"

#include "../textview/TextStyle.h"

class ColorOptionEntry : public ZLSimpleColorOptionEntry {

public:
	ColorOptionEntry(const std::string &keyName, ZLColorOption &option);
	const std::string KeyName;
};

ColorOptionEntry::ColorOptionEntry(const std::string &keyName, ZLColorOption &option) :
	ZLSimpleColorOptionEntry(option), KeyName(keyName) {
}

ColorOptionsPage::ColorOptionsPage(ZLDialogContent &dialogTab) {
	BaseTextStyle &baseStyle = TextStyleCollection::instance().baseStyle();

	typedef std::vector<ColorOptionEntry*> ColorVector;
	ColorVector colorOptions;
	colorOptions.push_back(new ColorOptionEntry("Background", baseStyle.BackgroundColorOption));
	colorOptions.push_back(new ColorOptionEntry("Regular Text", baseStyle.RegularTextColorOption));
	colorOptions.push_back(new ColorOptionEntry("Hyperlink Text", baseStyle.HyperlinkTextColorOption));
	colorOptions.push_back(new ColorOptionEntry("Selected Text", baseStyle.SelectedTextColorOption));
	colorOptions.push_back(new ColorOptionEntry("Tree Lines", baseStyle.TreeLinesColorOption));
	colorOptions.push_back(new ColorOptionEntry("Position Indicator", TextStyleCollection::instance().indicatorStyle().ColorOption));
		
	myComboEntry = new ComboOptionEntry(*this, "Color For", colorOptions[0]->KeyName);
	for (ColorVector::const_iterator it = colorOptions.begin(); it != colorOptions.end(); ++it) {
		myComboEntry->addValue((*it)->KeyName);
	}

	dialogTab.addOptions(myComboEntry, 0);

	for (ColorVector::const_iterator it = colorOptions.begin(); it != colorOptions.end(); ++it) {
		registerEntry(dialogTab, *it, (*it)->KeyName);
	}

	myComboEntry->onValueChange(myComboEntry->initialValue());
}

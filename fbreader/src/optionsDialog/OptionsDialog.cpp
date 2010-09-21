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

#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLPaintContext.h>
#include <ZLEncodingConverter.h>

#include <optionEntries/ZLColorOptionBuilder.h>
#include <optionEntries/ZLSimpleOptionEntry.h>
#include <optionEntries/ZLToggleBooleanOptionEntry.h>

#include <ZLTextView.h>
#include <ZLTextStyle.h>
#include <ZLTextStyleCollection.h>
#include <ZLTextStyleOptions.h>

#include "OptionsDialog.h"
#include "FormatOptionsPage.h"
#include "StyleOptionsPage.h"
#include "NetworkLibraryPage.h"

#include "../fbreader/FBReader.h"
#include "../options/FBOptions.h"
#include "../fbreader/BookTextView.h"

#include "../network/NetworkLinkCollection.h"
#include "../library/Library.h"
#include "../external/ProgramCollection.h"
#include "../formats/FormatPlugin.h"



OptionsDialog::OptionsDialog() : AbstractOptionsDialog(ZLResourceKey("OptionsDialog"), true) {
	FBReader &fbreader = FBReader::Instance();

	ZLOptionsDialog &dialog = this->dialog();

	ZLDialogContent &cssTab = dialog.createTab(ZLResourceKey("CSS"));
	cssTab.addOption(ZLResourceKey("overrideSpecifiedFonts"), ZLTextStyleCollection::Instance().OverrideSpecifiedFontsOption);

	ZLDialogContent &marginTab = dialog.createTab(ZLResourceKey("Margins"));
	FBOptions &options = FBOptions::Instance();
	marginTab.addOptions(
		ZLResourceKey("left"), new ZLSimpleSpinOptionEntry(options.LeftMarginOption, 1),
		ZLResourceKey("right"), new ZLSimpleSpinOptionEntry(options.RightMarginOption, 1)
	);
	marginTab.addOptions(
		ZLResourceKey("top"), new ZLSimpleSpinOptionEntry(options.TopMarginOption, 1),
		ZLResourceKey("bottom"), new ZLSimpleSpinOptionEntry(options.BottomMarginOption, 1)
	);

	myFormatPage = new FormatOptionsPage(dialog.createTab(ZLResourceKey("Format")));
	myStylePage = new StyleOptionsPage(dialog.createTab(ZLResourceKey("Styles")), *fbreader.context());

	ZLDialogContent &colorsTab = dialog.createTab(ZLResourceKey("Colors"));
	ZLResourceKey colorKey("colorFor");
	const ZLResource &resource = colorsTab.resource(colorKey);
	ZLColorOptionBuilder builder;
	const std::string BACKGROUND = resource["background"].value();
	builder.addOption(BACKGROUND, options.BackgroundColorOption);
	builder.addOption(resource["selectionBackground"].value(), options.colorOption(ZLTextStyle::SELECTION_BACKGROUND));
	builder.addOption(resource["text"].value(), options.RegularTextColorOption);
	builder.addOption(resource["internalLink"].value(), options.colorOption("internal"));
	builder.addOption(resource["externalLink"].value(), options.colorOption("external"));
	builder.addOption(resource["bookLink"].value(), options.colorOption("book"));
	builder.addOption(resource["highlighted"].value(), options.colorOption(ZLTextStyle::HIGHLIGHTED_TEXT));
	builder.addOption(resource["treeLines"].value(), options.colorOption(ZLTextStyle::TREE_LINES));
	builder.addOption(resource["indicator"].value(), (FBView::commonIndicatorInfo().ColorOption));
	builder.setInitial(BACKGROUND);
	colorsTab.addOption(colorKey, builder.comboEntry());
	colorsTab.addOption("", "", builder.colorEntry());
}

#include <ZLOptionsDialog.h>

#include "PreferencesMenuBuilder.h"

#include "MobileOptionsDialog.h"

MobileOptionsDialog::MobileOptionsDialog() : AbstractOptionsDialog(ZLResourceKey("MobileOptionsDialog"), true) {
	addItem(PreferencesMenuCode::DIRECTORIES);
	//addItem(PreferencesMenuCode::APPEARANCE);
	addItem(PreferencesMenuCode::TEXT);
	addItem(PreferencesMenuCode::COLORS);
	addItem(PreferencesMenuCode::MARGINS);
	addItem(PreferencesMenuCode::SCROLLBAR);
	//addItem(PreferencesMenuCode::DISPLAY);
	addItem(PreferencesMenuCode::SCROLLING);
	//addItem(PreferencesMenuCode::TAPZONES);
	//addItem(PreferencesMenuCode::DICTIONARY);
	//addItem(PreferencesMenuCode::IMAGES);
	//addItem(PreferencesMenuCode::CANCEL_MENU);
}

void MobileOptionsDialog::addItem(std::string item) {
	PreferencesMenuBuilder::addOptionsBySection(item, this->dialog().createTab(ZLResourceKey(item)));
}

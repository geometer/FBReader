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

ScrollingZonesEntry::ScrollingZonesEntry(const ZLResource &resource, ZLIntegerRangeOption &option) : myOption(option) {
        myValues.push_back(resource["left-right"].value());
        myValues.push_back(resource["up-bottom"].value());
}

const std::string &ScrollingZonesEntry::initialValue() const {
        return myValues[myOption.value()];
}

const std::vector<std::string> &ScrollingZonesEntry::values() const {
        return myValues;
}

void ScrollingZonesEntry::onAccept(const std::string &value) {
        for (size_t index = 0; index != myValues.size(); ++index) {
                if (myValues[index] == value) {
                        myOption.setValue(index);
                        break;
                }
        }
}

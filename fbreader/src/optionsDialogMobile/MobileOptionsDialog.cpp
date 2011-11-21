#include <algorithm>

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

const std::vector<std::string>& AbstractFontStyleEntry::values() const {
    return myValues;
}

void AbstractFontStyleEntry::onAccept(const std::string &value) {
    size_t i;
    for (i=0; i<myValues.size(); ++i) {
        if (myValues[i] == value) {
            break;
        }
    }
    if (i == myValues.size()) {
        //strange situation, that should not never happens
        return;
    }
    _onAccept(i);
}

FontStyleEntry::FontStyleEntry(const ZLResource &resource, ZLBooleanOption& boldOption, ZLBooleanOption& italicOption)
    : myBoldOption(boldOption), myItalicOption(italicOption) {
    myValues.push_back(resource["regular"].value());
    myValues.push_back(resource["bold"].value());
    myValues.push_back(resource["italic"].value());
    myValues.push_back(resource["boldItalic"].value());
}

const std::string& FontStyleEntry::initialValue() const {
    FontStyle fontStyle = REGULAR;
    if (myBoldOption.value()) {
        if (myItalicOption.value()) {
            fontStyle = BOLD_ITALIC;
        } else {
            fontStyle = BOLD;
        }
    } else {
        if (myItalicOption.value()) {
            fontStyle = ITALIC;
        }
    }
    return myValues.at(fontStyle);
}

void FontStyleEntry::_onAccept(size_t i) {
    if (i == REGULAR) {
        myBoldOption.setValue(false);
        myItalicOption.setValue(false);
    } else if (i == BOLD) {
        myBoldOption.setValue(true);
        myItalicOption.setValue(false);
    } else if (i == ITALIC) {
        myBoldOption.setValue(false);
        myItalicOption.setValue(true);
    } else if (i == BOLD_ITALIC){
        myBoldOption.setValue(true);
        myItalicOption.setValue(true);
    }
}

FontStyleBoolean3Entry:: FontStyleBoolean3Entry(const ZLResource &resource, ZLBoolean3Option &option) : myOption(option) {
    myValues.push_back(resource["on"].value());
    myValues.push_back(resource["off"].value());
    myValues.push_back(resource["unchanged"].value());
}

const std::string& FontStyleBoolean3Entry::initialValue() const {
    FontBoolean3Style fontStyle = UNCHANGED;
    if (myOption.value() == B3_FALSE) {
        fontStyle = OFF;
    } else if (myOption.value() == B3_TRUE) {
        fontStyle = ON;
    }
    return myValues.at(fontStyle);
}

void FontStyleBoolean3Entry::_onAccept(size_t i) {
    if (i == ON) {
        myOption.setValue(B3_TRUE);
    } else if (i == OFF) {
        myOption.setValue(B3_FALSE);
    } else if (i == UNCHANGED) {
        myOption.setValue(B3_UNDEFINED);
    }
}

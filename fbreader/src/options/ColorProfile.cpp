#include "ColorProfile.h"

const std::string ColorProfile::DAY("defaultLight");
const std::string ColorProfile::NIGHT("defaultDark");

static const std::string COLORS = "Colors";

ZLColorOption &ColorProfile::colorOption(const std::string &style) {
	std::map<std::string,shared_ptr<ZLColorOption> >::const_iterator it =
		myColorOptions.find(style);
	return it != myColorOptions.end() ? *it->second : *RegularTextColorOption;
}

shared_ptr<ZLColorOption> ColorProfile::createOption(std::string profileName, std::string optionName, int r, int g, int b) {
	return new ZLColorOption(ZLCategoryKey::LOOK_AND_FEEL, COLORS, profileName + ':' + optionName, ZLColor(r, g, b));
}

ColorProfile::ColorProfile(std::string name) {
	//TODO all strings should be const static std::string
	if (name == NIGHT) {
		BackgroundColorOption = createOption(name, "Background", 0, 0, 0);
		RegularTextColorOption = createOption(name, "Text", 192,192,192);
		myColorOptions["internal"] = createOption(name, "Hyperlink", 60, 142, 224);
		myColorOptions["external"] = createOption(name, "ExternalHyperlink", 60, 142, 224);
		myColorOptions["book"] = createOption(name, "BookHyperlink", 60, 142, 224);
		myColorOptions[ZLTextStyle::SELECTION_BACKGROUND] = createOption(name, "SelectionBackground", 82, 131, 194);
		myColorOptions[ZLTextStyle::HIGHLIGHTED_TEXT] = createOption(name, "SelectedText",  255, 255, 220);
		myColorOptions[ZLTextStyle::TREE_LINES] = createOption(name, "TreeLines", 127, 127, 127);
	} else {
		BackgroundColorOption = createOption(name, "Background", 255, 255, 255);
		RegularTextColorOption = createOption(name, "Text", 0,0,0);
		myColorOptions["internal"] = createOption(name, "Hyperlink", 33, 96, 180);
		myColorOptions["external"] = createOption(name, "ExternalHyperlink", 33, 96, 180);
		myColorOptions["book"] = createOption(name, "BookHyperlink", 23, 68, 128);
		myColorOptions[ZLTextStyle::SELECTION_BACKGROUND] = createOption(name, "SelectionBackground", 82, 131, 194);
		myColorOptions[ZLTextStyle::HIGHLIGHTED_TEXT] = createOption(name, "SelectedText",  60, 139, 255);
		myColorOptions[ZLTextStyle::TREE_LINES] = createOption(name, "TreeLines", 127, 127, 127);
	}
}




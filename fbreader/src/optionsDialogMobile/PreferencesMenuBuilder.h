#ifndef __PREFERENCESMENUBUILDER_H__
#define __PREFERENCESMENUBUILDER_H__

#include <string>
#include <vector>

#include <ZLDialogContent.h>
#include <ZLOptionEntry.h>

#include "PreferencesMenuBuilder.h"

class PreferencesMenuCode {
public:
	const static std::string PREFERENCES;
	const static std::string DIRECTORIES;
	const static std::string APPEARANCE;
	const static std::string TEXT;
	const static std::string COLORS;
	const static std::string MARGINS;
	const static std::string SCROLLBAR;
	const static std::string DISPLAY;
	const static std::string SCROLLING;
	const static std::string TAPZONES;
	const static std::string DICTIONARY;
	const static std::string IMAGES;
	const static std::string CANCEL_MENU;
};

class PreferencesMenuBuilder {
public:
    static bool addOptionsBySection(std::string section, ZLDialogContent& content);

    static void addDirectoriesOptions(ZLDialogContent& content);
    static void addAppearanceOptions(ZLDialogContent& content);
    static void addTextOptions(ZLDialogContent& content);
    static void addColorsOptions(ZLDialogContent& content);
    static void addMarginsOptions(ZLDialogContent& content);
    static void addScrollBarOptions(ZLDialogContent& content);
    static void addDisplayOptions(ZLDialogContent& content);
    static void addScrollingOptions(ZLDialogContent& content);
    static void addTapZonesOptions(ZLDialogContent& content);
    static void addDictionaryOptions(ZLDialogContent& content);
    static void addImagesOptions(ZLDialogContent& content);
    static void addCancelMenuOptions(ZLDialogContent& content);
private: //disable constructor
	PreferencesMenuBuilder() { }
};

#endif /* __PREFERENCESMENUBUILDER_H__ */

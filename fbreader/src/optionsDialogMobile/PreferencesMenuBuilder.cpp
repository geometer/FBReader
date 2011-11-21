#include <string>

#include <ZLOptionsDialog.h>
#include <ZLOptionEntry.h>
#include <ZLTextView.h>
#include <ZLTextStyle.h>
#include <ZLTextStyleCollection.h>
#include <ZLTextStyleOptions.h>
#include <ZLPaintContext.h>

#include <optionEntries/ZLSimpleKeyOptionEntry.h>
#include <optionEntries/ZLSimpleOptionEntry.h>
#include <optionEntries/ZLToggleBooleanOptionEntry.h>
#include <optionEntries/ZLColorOptionBuilder.h>
#include <optionEntries/ZLLanguageOptionEntry.h>

#include "../options/FBOptions.h"
#include "../options/FBTextStyle.h"

#include "../fbreader/FBReader.h"
#include "../fbreader/FBView.h"
#include "../fbreader/BookTextView.h"

#include "../bookmodel/FBTextKind.h"

#include "../formats/FormatPlugin.h"
#include "../encodingOption/EncodingOptionEntry.h"

#include "../optionsDialog/library/LibraryOptionsDialog.h"
#include "../optionsDialog/reading/ReadingOptionsDialog.h"
#include "../optionsDialog/system/SystemOptionsDialog.h"
#include "../optionsDialog/network/NetworkOptionsDialog.h"

#include "../network/NetworkLinkCollection.h"

#include "MobileOptionsDialog.h"
#include "PreferencesMenuBuilder.h"

const std::string PreferencesMenuCode::DIRECTORIES("directories");
const std::string PreferencesMenuCode::APPEARANCE("appearance");
const std::string PreferencesMenuCode::TEXT("text");
const std::string PreferencesMenuCode::COLORS("colors");
const std::string PreferencesMenuCode::MARGINS("margins");
const std::string PreferencesMenuCode::SCROLLBAR("scrollBar");
const std::string PreferencesMenuCode::DISPLAY("display");
const std::string PreferencesMenuCode::SCROLLING("scrolling");
const std::string PreferencesMenuCode::TAPZONES("tapZones");
const std::string PreferencesMenuCode::DICTIONARY("dictionary");
const std::string PreferencesMenuCode::IMAGES("images");
const std::string PreferencesMenuCode::CANCEL_MENU("cancelMenu");

class OptionsPageComboOptionEntry : public ZLComboOptionEntry {
public:
		OptionsPageComboOptionEntry(const std::string &initialValue);
		const std::string &initialValue() const;
		const std::vector<std::string> &values() const;
		void onAccept(const std::string&);
		void onValueSelected(int index);
		void addValue(const std::string &value);
public: // removed from OptionsPage; reason: to get rid of Options Pages classes
		void registerEntry(ZLDialogContent &tab, const ZLResourceKey &entryKey, ZLOptionEntry *entry, const std::string &name);
protected:
		std::vector<std::string> myValues;
		std::string myInitialValue;
private:
		std::vector< std::pair<ZLOptionEntry*,std::string> > myEntries;
};

OptionsPageComboOptionEntry::OptionsPageComboOptionEntry(const std::string &initialValue) : myInitialValue(initialValue) {}
const std::string &OptionsPageComboOptionEntry::initialValue() const { return myInitialValue; }
const std::vector<std::string> &OptionsPageComboOptionEntry::values() const { return myValues; }
void OptionsPageComboOptionEntry::onAccept(const std::string&) {}
void OptionsPageComboOptionEntry::addValue(const std::string &value) { myValues.push_back(value); }

void OptionsPageComboOptionEntry::onValueSelected(int index) {
        const std::string &selectedValue = values()[index];
        int count = 0;
        for (std::vector< std::pair<ZLOptionEntry*,std::string> >::const_iterator it = myEntries.begin(); it != myEntries.end(); ++it, ++count) {
            it->first->setVisible( it->second == selectedValue);
        }
}

void OptionsPageComboOptionEntry::registerEntry(ZLDialogContent &tab, const ZLResourceKey &entryKey, ZLOptionEntry *entry, const std::string &name) {
        if (entry != 0) {
                entry->setVisible(false);
                myEntries.push_back( std::pair<ZLOptionEntry*,std::string>(entry,name));
        }
        tab.addOption(entryKey, entry);
}

 bool PreferencesMenuBuilder::addOptionsBySection(std::string section, ZLDialogContent& content) {
	 if (section == PreferencesMenuCode::DIRECTORIES) {
         addDirectoriesOptions(content);
	 } else if (section == PreferencesMenuCode::APPEARANCE) {
         addAppearanceOptions(content);
	 } else if (section == PreferencesMenuCode::TEXT) {
         addTextOptions(content);
	 } else if (section == PreferencesMenuCode::COLORS) {
         addColorsOptions(content);
	 } else if (section == PreferencesMenuCode::MARGINS) {
         addMarginsOptions(content);
	 } else if (section == PreferencesMenuCode::SCROLLBAR) {
         addScrollBarOptions(content);
	 } else if (section == PreferencesMenuCode::DISPLAY) {
         addDisplayOptions(content);
	 } else if (section == PreferencesMenuCode::SCROLLING) {
         addScrollingOptions(content);
	 } else if (section == PreferencesMenuCode::TAPZONES) {
         addTapZonesOptions(content);
	 } else if (section == PreferencesMenuCode::DICTIONARY) {
         addDictionaryOptions(content);
	 } else if (section == PreferencesMenuCode::IMAGES) {
         addImagesOptions(content);
	 } else if (section == PreferencesMenuCode::CANCEL_MENU) {
         addCancelMenuOptions(content);
     } else {
         return false;
     }
     return true;
 }

 void PreferencesMenuBuilder::addDirectoriesOptions(ZLDialogContent& content) {
     Library &library = Library::Instance();
	 ZLPathOptionEntry* entry = new ZLPathOptionEntry(library.PathOption);
	 content.addOption(ZLResourceKey("books"), entry );
     //content.addOption(ZLResourceKey("downloadDirectory"), NetworkLinkCollection::Instance().DirectoryOption);
 }

 void PreferencesMenuBuilder::addAppearanceOptions(ZLDialogContent& content) { }

 void PreferencesMenuBuilder::addTextOptions(ZLDialogContent& content) {
     static const ZLResourceKey KEY_STYLE("style");
     static const ZLResourceKey KEY_BASE("Base");

     static const ZLResourceKey KEY_DUMMY("");
     static const ZLResourceKey KEY_LINESPACING("lineSpacing");
     static const ZLResourceKey KEY_FIRSTLINEINDENT("firstLineIndent");
     static const ZLResourceKey KEY_ALIGNMENT("alignment");
     static const ZLResourceKey KEY_SPACEBEFORE("spaceBefore");
     static const ZLResourceKey KEY_SPACEAFTER("spaceAfter");
     static const ZLResourceKey KEY_STARTINDENT("leftIndent");
     static const ZLResourceKey KEY_ENDINDENT("rightIndent");

     static const ZLResourceKey KEY_FONTSTYLE("fontStyle");
     static const ZLResourceKey KEY_BOLD("bold");
     static const ZLResourceKey KEY_ITALIC("italic");
     static const ZLResourceKey KEY_FONTFAMILY("font");
     static const ZLResourceKey KEY_FONTSIZE("fontSize");
     static const ZLResourceKey KEY_FONTSIZEDIFFERENCE("fontSizeDifference");
     static const ZLResourceKey KEY_ALLOWHYPHENATIONS("allowHyphenations");
     static const ZLResourceKey KEY_AUTOHYPHENATIONS("autoHyphenations");

     const ZLResource &styleResource = ZLResource::resource(KEY_STYLE);

	 OptionsPageComboOptionEntry* myComboEntry = new OptionsPageComboOptionEntry(styleResource[KEY_BASE].value());
     myComboEntry->addValue(myComboEntry->initialValue());

     ZLTextStyleCollection &collection = ZLTextStyleCollection::Instance();
	 ZLTextKind styles[] = { REGULAR, TITLE, SECTION_TITLE, SUBTITLE, H1, H2, H3, H4, H5, H6, ANNOTATION, EPIGRAPH, PREFORMATTED, AUTHOR, DATEKIND, POEM_TITLE, STANZA, VERSE };
     const int STYLES_NUMBER = sizeof(styles) / sizeof(ZLTextKind);
     for (int i = 0; i < STYLES_NUMBER; ++i) {
             const ZLTextStyleDecoration *decoration = collection.decoration(styles[i]);
             if (decoration != 0) {
                     myComboEntry->addValue(styleResource[decoration->name()].value());
             }
     }
     content.addOption(ZLResourceKey("optionsFor"), myComboEntry);


     const ZLPaintContext& context = *(FBReader::Instance().context());

     {
             const std::string &name = myComboEntry->initialValue();
             FBTextStyle &baseStyle = FBTextStyle::Instance();

             myComboEntry->registerEntry(content,
                     KEY_FONTFAMILY, new ZLFontFamilyOptionEntry(baseStyle.FontFamilyOption, context),
                     name
             );

             myComboEntry->registerEntry(content,
                     KEY_FONTSIZE, new ZLSimpleSpinOptionEntry(baseStyle.FontSizeOption, 2),
                     name
             );

             myComboEntry->registerEntry(content,
                     KEY_FONTSTYLE,
                     new FontStyleEntry(content.resource(KEY_FONTSTYLE),
                                        baseStyle.BoldOption, baseStyle.ItalicOption),
                     name
             );

//             myComboEntry->registerEntry(content,
//                     KEY_BOLD, new ZLSimpleBooleanOptionEntry(baseStyle.BoldOption),
//                     name
//             );

//             myComboEntry->registerEntry(content,
//                     KEY_ITALIC, new ZLSimpleBooleanOptionEntry(baseStyle.ItalicOption),
//                     name
//             );

             myComboEntry->registerEntry(content,
                     KEY_LINESPACING, new ZLTextLineSpaceOptionEntry(baseStyle.LineSpacePercentOption, content.resource(KEY_LINESPACING), false),
                     name
             );

             myComboEntry->registerEntry(content,
                     KEY_ALIGNMENT, new ZLTextAlignmentOptionEntry(baseStyle.AlignmentOption, content.resource(KEY_ALIGNMENT), false),
                     name
             );

             myComboEntry->registerEntry(content,
                     KEY_AUTOHYPHENATIONS, new ZLSimpleBooleanOptionEntry(collection.AutoHyphenationOption),
                     name
             );

     }

     for (int i = 0; i < STYLES_NUMBER; ++i) {
             ZLTextStyleDecoration *d = collection.decoration(styles[i]);
             if ((d != 0) && (d->isFullDecoration())) {
                     ZLTextFullStyleDecoration *decoration = (ZLTextFullStyleDecoration*)d;
                     const std::string &name = styleResource[decoration->name()].value();

                     myComboEntry->registerEntry(content,
                             KEY_FONTFAMILY, new ZLTextFontFamilyWithBaseOptionEntry(decoration->FontFamilyOption, content.resource(KEY_FONTFAMILY), context),
                             name
                     );

                     myComboEntry->registerEntry(content,
                             KEY_FONTSIZEDIFFERENCE, new ZLSimpleSpinOptionEntry(decoration->FontSizeDeltaOption, 2),
                             name
                     );

                     myComboEntry->registerEntry(content,
                             KEY_BOLD, new FontStyleBoolean3Entry(content.resource(KEY_BOLD), decoration->BoldOption),
                             name
                     );

                     myComboEntry->registerEntry(content,
                             KEY_ITALIC, new FontStyleBoolean3Entry(content.resource(KEY_ITALIC), decoration->ItalicOption),
                             name
                     );

//                     myComboEntry->registerEntry(content,
//                             KEY_BOLD, new ZLSimpleBoolean3OptionEntry(decoration->BoldOption),
//                             name
//                     );

//                     myComboEntry->registerEntry(content,
//                             KEY_ITALIC, new ZLSimpleBoolean3OptionEntry(decoration->ItalicOption),
//                             name
//                     );

                     myComboEntry->registerEntry(content,
                             KEY_ALLOWHYPHENATIONS, new ZLSimpleBoolean3OptionEntry(decoration->AllowHyphenationsOption),
                             name
                     );

                     myComboEntry->registerEntry(content,
                             KEY_ALIGNMENT, new ZLTextAlignmentOptionEntry(decoration->AlignmentOption, content.resource(KEY_ALIGNMENT), true),
                             name
                     );

                     myComboEntry->registerEntry(content,
                             KEY_SPACEBEFORE, new ZLSimpleSpinOptionEntry(decoration->SpaceBeforeOption, 1),
                             name
                     );

                     myComboEntry->registerEntry(content,
                             KEY_SPACEAFTER, new ZLSimpleSpinOptionEntry(decoration->SpaceAfterOption, 1),
                             name
                     );

                     myComboEntry->registerEntry(content,
                             KEY_STARTINDENT, new ZLSimpleSpinOptionEntry(decoration->LineStartIndentOption, 1),
                             name
                     );

                     myComboEntry->registerEntry(content,
                             KEY_ENDINDENT, new ZLSimpleSpinOptionEntry(decoration->LineEndIndentOption, 1),
                             name
                     );

                     myComboEntry->registerEntry(content,
                             KEY_FIRSTLINEINDENT, new ZLSimpleSpinOptionEntry(decoration->FirstLineIndentDeltaOption, 1),
                             name
                     );

                     myComboEntry->registerEntry(content,
                             KEY_LINESPACING, new ZLTextLineSpaceOptionEntry(decoration->LineSpacePercentOption, content.resource(KEY_LINESPACING), true),
                             name
                     );

             }
     }

     myComboEntry->onValueSelected(0);
 }

 void PreferencesMenuBuilder::addColorsOptions(ZLDialogContent& content) {
     FBOptions &options = FBOptions::Instance();
     ZLColorOptionBuilder builder;
     const std::string BACKGROUND = content.resource( ZLResourceKey("background")).value();
	 builder.addOption(BACKGROUND, options.backgroundColorOption());
     builder.addOption(content.resource( ZLResourceKey("selectionBackground") ).value(), options.colorOption(ZLTextStyle::SELECTION_BACKGROUND));
	 builder.addOption(content.resource( ZLResourceKey("text")).value(), options.regularTextColorOption());
     builder.addOption(content.resource( ZLResourceKey("internalLink")).value(), options.colorOption("internal"));
     builder.addOption(content.resource( ZLResourceKey("externalLink")).value(), options.colorOption("external"));
     //builder.addOption(content.resource(  ZLResourceKey("bookLink")).value(), options.colorOption("book"));
     builder.addOption(content.resource( ZLResourceKey("highlighted")).value(), options.colorOption(ZLTextStyle::HIGHLIGHTED_TEXT));
     //builder.addOption(content.resource(  ZLResourceKey("treeLines")).value(), options.colorOption(ZLTextStyle::TREE_LINES));
     builder.addOption(content.resource(  ZLResourceKey("indicator")).value(), (FBView::commonIndicatorInfo().ColorOption));
     builder.setInitial(BACKGROUND);
     content.addOption( ZLResourceKey("colorFor"), builder.comboEntry());
     content.addOption("", "", builder.colorEntry());
 }

 void PreferencesMenuBuilder::addMarginsOptions(ZLDialogContent& content) {
     FBOptions &options = FBOptions::Instance();
     content.addOption( ZLResourceKey("left"), new ZLSimpleSpinOptionEntry(options.LeftMarginOption, 1) );
     content.addOption( ZLResourceKey("right"), new ZLSimpleSpinOptionEntry(options.RightMarginOption, 1) );
     content.addOption( ZLResourceKey("top"), new ZLSimpleSpinOptionEntry(options.TopMarginOption, 1) );
     content.addOption( ZLResourceKey("bottom"), new ZLSimpleSpinOptionEntry(options.BottomMarginOption, 1) );
 }

 void PreferencesMenuBuilder::addScrollBarOptions(ZLDialogContent& content) {
     FBIndicatorStyle &indicatorInfo = FBView::commonIndicatorInfo();
     static ZLResourceKey typeKey("type");
     IndicatorTypeEntry *indicatorTypeEntry =
             new IndicatorTypeEntry(content.resource(typeKey), indicatorInfo.TypeOption);
     content.addOption(typeKey, indicatorTypeEntry);

     ZLOptionEntry *heightEntry = new ZLSimpleSpinOptionEntry(indicatorInfo.HeightOption, 1);
     content.addOption(ZLResourceKey("footerHeight"),heightEntry);
     indicatorTypeEntry->addDependentEntry(heightEntry);
     //ZLOptionEntry *offsetEntry = new ZLSimpleSpinOptionEntry(indicatorInfo.OffsetOption, 1);
     //content.addOption(ZLResourceKey("offset"), offsetEntry);
     //indicatorTypeEntry->addDependentEntry(offsetEntry);

     ZLOptionEntry *tocMarksEntry =
             new ZLSimpleBooleanOptionEntry(FBReader::Instance().bookTextView().ShowTOCMarksOption);
     content.addOption(ZLResourceKey("tocMarks"), tocMarksEntry);
     indicatorTypeEntry->addDependentEntry(tocMarksEntry);

     StateOptionEntry *showTimeEntry =
             new StateOptionEntry(indicatorInfo.ShowTimeOption);
     content.addOption(ZLResourceKey("showClock"), showTimeEntry);
     indicatorTypeEntry->addDependentEntry(showTimeEntry);

     StateOptionEntry *showTextPositionEntry =
             new StateOptionEntry(indicatorInfo.ShowTextPositionOption);
     content.addOption(ZLResourceKey("showProgress"), showTextPositionEntry);
     indicatorTypeEntry->addDependentEntry(showTextPositionEntry);

     SpecialFontSizeEntry *fontSizeEntry =
             new SpecialFontSizeEntry(indicatorInfo.FontSizeOption, 2, *showTextPositionEntry, *showTimeEntry);
     content.addOption(ZLResourceKey("fontSize"), fontSizeEntry);
     indicatorTypeEntry->addDependentEntry(fontSizeEntry);
     showTextPositionEntry->addDependentEntry(fontSizeEntry);
     showTimeEntry->addDependentEntry(fontSizeEntry);

//     ZLOptionEntry *navigationEntry =
//             new ZLSimpleBooleanOptionEntry(indicatorInfo.IsSensitiveOption);
//     content.addOption(ZLResourceKey("navigation"), navigationEntry);
//     indicatorTypeEntry->addDependentEntry(navigationEntry);

     indicatorTypeEntry->onStringValueSelected(indicatorTypeEntry->initialValue());
     showTextPositionEntry->onStateChanged(showTextPositionEntry->initialState());
     showTimeEntry->onStateChanged(showTimeEntry->initialState());
 }

 void PreferencesMenuBuilder::addDisplayOptions(ZLDialogContent& content) { }

 void PreferencesMenuBuilder::addScrollingOptions(ZLDialogContent& content) {
    FBReader &fbreader = FBReader::Instance();
	content.addOption(ZLResourceKey("keyLinesToKeep"), new ZLSimpleSpinOptionEntry(fbreader.LinesToKeepOption, 1));
//	content.addOption(ZLResourceKey("keyScrollDelay"), new ZLSimpleSpinOptionEntry(fbreader.KeyScrollingDelayOption, 50));
    const bool hasTouchScreen = ZLBooleanOption(ZLCategoryKey::EMPTY, ZLOption::PLATFORM_GROUP, ZLOption::TOUCHSCREEN_PRESENTED, false).value();
    const bool hasVolumeKeys = ZLBooleanOption(ZLCategoryKey::EMPTY, ZLOption::PLATFORM_GROUP, ZLOption::VOLUME_KEYS_PRESENTED, false).value();
    if (hasTouchScreen) {
        static ZLResourceKey zonesKey("tabScrollingZones");
        ScrollingZonesEntry *scrollingZonesEntry =
                new ScrollingZonesEntry(content.resource(zonesKey), fbreader.TapScrollingZonesOption);
        content.addOption(zonesKey, scrollingZonesEntry);
    }
    if (hasVolumeKeys) {
        ZLToggleBooleanOptionEntry *volumeKeysEntry =
                new ZLToggleBooleanOptionEntry(fbreader.EnableTapScrollingByVolumeKeysOption);
        content.addOption(ZLResourceKey("volumeKeys"), volumeKeysEntry);
    }
 }

void PreferencesMenuBuilder::addTapZonesOptions(ZLDialogContent& content) { }
void PreferencesMenuBuilder::addDictionaryOptions(ZLDialogContent& content) { }
void PreferencesMenuBuilder::addImagesOptions(ZLDialogContent& content) { }
void PreferencesMenuBuilder::addCancelMenuOptions(ZLDialogContent& content) { }

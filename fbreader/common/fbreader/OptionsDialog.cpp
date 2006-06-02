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

#include <abstract/ZLDialogManager.h>
#include <abstract/ZLOptionsDialog.h>
#include <abstract/ZLPaintContext.h>

#include "OptionsDialog.h"
#include "FormatOptionsPage.h"
#include "ScrollingOptionsPage.h"
#include "StyleOptionsPage.h"
#include "ColorOptionsPage.h"
#include "KeyBindingsPage.h"

#include "FBReader.h"
#include "CollectionView.h"
#include "RecentBooksView.h"

#include "../collection/BookCollection.h"
#include "../textview/TextView.h"
#include "../textview/TextStyle.h"
#include "../textview/TextStyleOptions.h"
#include "../formats/FormatPlugin.h"

class ShowIndicatorEntry : public ZLSimpleBooleanOptionEntry {

public:
  ShowIndicatorEntry(IndicatorPage &page, const std::string &name, ZLBooleanOption &option);
  ~ShowIndicatorEntry();
  void onValueChange(bool value);

private:
  IndicatorPage &myPage;
};

ShowIndicatorEntry::ShowIndicatorEntry(IndicatorPage &page, const std::string &name, ZLBooleanOption &option) : ZLSimpleBooleanOptionEntry(name, option), myPage(page) {
}

ShowIndicatorEntry::~ShowIndicatorEntry() {
}

void ShowIndicatorEntry::onValueChange(bool value) {
  myPage.HeightEntry->setVisible(value);
  myPage.OffsetEntry->setVisible(value);
  myPage.EnableNavigationEntry->setVisible(value);
}

class RotationTypeEntry : public ZLChoiceOptionEntry {

public:
  RotationTypeEntry(ZLIntegerOption &angleOption);
  const std::string &name() const;
  const std::string &text(int index) const;
  int choiceNumber() const;
  int initialCheckedIndex() const;
  void onAccept(int index);

private:
  ZLIntegerOption &myAngleOption;
};

static const std::string ROTATION_TYPE = "Rotation Type";
static const std::string ROTATION_0 = "Disabled";
static const std::string ROTATION_90 = "90 Degrees Counterclockwise";
static const std::string ROTATION_180 = "180 Degrees";
static const std::string ROTATION_270 = "90 Degrees Clockwise";
static const std::string ROTATION_USE_4_DIRECTIONS = "Cycle Through All 4 Directions";

RotationTypeEntry::RotationTypeEntry(ZLIntegerOption &angleOption) : myAngleOption(angleOption) {
}

const std::string &RotationTypeEntry::name() const {
  return ROTATION_TYPE;
}

const std::string &RotationTypeEntry::text(int index) const {
  switch (index) {
    case 1:
      return ROTATION_90;
    case 2:
      return ROTATION_180;
    case 3:
      return ROTATION_270;
    case 4:
      return ROTATION_USE_4_DIRECTIONS;
    default:
      return ROTATION_0;
  }
}

int RotationTypeEntry::choiceNumber() const {
  return 5;
}

int RotationTypeEntry::initialCheckedIndex() const {
  switch (myAngleOption.value()) {
    default:
      return 0;
    case ZLViewWidget::DEGREES90:
      return 1;
    case ZLViewWidget::DEGREES180:
      return 2;
    case ZLViewWidget::DEGREES270:
      return 3;
    case -1:
      return 4;
  }
}

void RotationTypeEntry::onAccept(int index) {
  int angle = ZLViewWidget::DEGREES0;
  switch (index) {
    case 1:
      angle = ZLViewWidget::DEGREES90;
      break;
    case 2:
      angle = ZLViewWidget::DEGREES180;
      break;
    case 3:
      angle = ZLViewWidget::DEGREES270;
      break;
    case 4:
      angle = -1;
      break;
  }
  myAngleOption.setValue(angle);
}

class DefaultLanguageEntry : public ZLComboOptionEntry {

public:
  const std::string &name() const;
  const std::string &initialValue() const;
  const std::vector<std::string> &values() const;
  void onAccept(const std::string &value);
};

const std::string &DefaultLanguageEntry::name() const {
  static const std::string _name = "Language For Encoding Detection";
  return _name;
}

static std::string LANGUAGE_RUSSIAN = "Russian";
static std::string LANGUAGE_CHINESE = "Chinese";
static std::string LANGUAGE_OTHER = "Other";

const std::string &DefaultLanguageEntry::initialValue() const {
  switch (PluginCollection::instance().DefaultLanguageOption.value()) {
    case EncodingDetector::RUSSIAN:
      return LANGUAGE_RUSSIAN;
    case EncodingDetector::CHINESE:
      return LANGUAGE_CHINESE;
    default:
      return LANGUAGE_OTHER;
  }
}

const std::vector<std::string> &DefaultLanguageEntry::values() const {
  static std::vector<std::string> _values;
  if (_values.empty()) {
    _values.push_back(LANGUAGE_RUSSIAN);
    _values.push_back(LANGUAGE_CHINESE);
    _values.push_back(LANGUAGE_OTHER);
  }
  return _values;
}

void DefaultLanguageEntry::onAccept(const std::string &value) {
  EncodingDetector::Language language = EncodingDetector::OTHER;
  if (value == LANGUAGE_RUSSIAN) {
    language = EncodingDetector::RUSSIAN;
  } else if (value == LANGUAGE_CHINESE) {
    language = EncodingDetector::CHINESE;
  }
  PluginCollection::instance().DefaultLanguageOption.setValue(language);
}

OptionsDialog::OptionsDialog(FBReader &fbreader, ZLPaintContext &context) {
  myDialog = ZLDialogManager::instance().createOptionsDialog("OptionsDialog", "FBReader - Options");

  ZLOptionsDialogTab *generalTab = myDialog->createTab("General");
  generalTab->addOption(new ZLSimpleBooleanOptionEntry("Quit Application On Cancel", fbreader.QuitOnCancelOption));
  generalTab->addOption(new ZLSimpleStringOptionEntry("Book Path", fbreader.myCollectionView->collection().PathOption));
  generalTab->addOption(new ZLSimpleBooleanOptionEntry("Look For Books In Subdirectories", fbreader.myCollectionView->collection().ScanSubdirsOption));
  generalTab->addOption(new ZLSimpleSpinOptionEntry("Recent Books List Size", fbreader.myRecentBooksView->lastBooks().MaxListSizeOption, 1));
  generalTab->addOption(new ZLSimpleBooleanOptionEntry("Keep Position In Contents View", fbreader.StoreContentsPositionOption));
  generalTab->addOption(new ZLSimpleSpinOptionEntry("Delay Between Accepted Key Pressings", fbreader.KeyDelayOption, 50));
  generalTab->addOption(new DefaultLanguageEntry());

  myScrollingPage = new ScrollingOptionsPage(myDialog->createTab("Scrolling"), fbreader);

  ZLOptionsDialogTab *marginTab = myDialog->createTab("Margins");
  marginTab->addOptions(
    new ZLSimpleSpinOptionEntry("Left Margin", TextStyleCollection::instance().baseStyle().LeftMarginOption, 1),
    new ZLSimpleSpinOptionEntry("Right Margin", TextStyleCollection::instance().baseStyle().RightMarginOption, 1)
  );
  marginTab->addOptions(
    new ZLSimpleSpinOptionEntry("Top Margin", TextStyleCollection::instance().baseStyle().TopMarginOption, 1),
    new ZLSimpleSpinOptionEntry("Bottom Margin", TextStyleCollection::instance().baseStyle().BottomMarginOption, 1)
  );

  myFormatPage = new FormatOptionsPage(myDialog->createTab("Format"));
  myStylePage = new StyleOptionsPage(myDialog->createTab("Styles"), context);

  ZLOptionsDialogTab *indicatorTab = myDialog->createTab("Indicator");
  PositionIndicatorStyle &indicatorStyle = TextStyleCollection::instance().indicatorStyle();
  myIndicatorPage.ShowIndicatorEntry = new ShowIndicatorEntry(myIndicatorPage, "Show Position Indicator", indicatorStyle.ShowOption);
  myIndicatorPage.HeightEntry = new ZLSimpleSpinOptionEntry("Indicator Height", indicatorStyle.HeightOption, 1);
  myIndicatorPage.OffsetEntry = new ZLSimpleSpinOptionEntry("Offset From Text", indicatorStyle.OffsetOption, 1);
  myIndicatorPage.EnableNavigationEntry = new ZLSimpleBooleanOptionEntry("Enable Navigation", indicatorStyle.IsSensitiveOption);
  indicatorTab->addOption(myIndicatorPage.ShowIndicatorEntry);
  indicatorTab->addOption(myIndicatorPage.HeightEntry);
  indicatorTab->addOption(myIndicatorPage.OffsetEntry);
  indicatorTab->addOption(myIndicatorPage.EnableNavigationEntry);
  myIndicatorPage.ShowIndicatorEntry->onValueChange(myIndicatorPage.ShowIndicatorEntry->initialState());

  if (fbreader.isRotationSupported()) {
    ZLOptionsDialogTab *rotationTab = myDialog->createTab("Rotation");
    rotationTab->addOption(new RotationTypeEntry(fbreader.RotationAngleOption));
  }

  myColorPage = new ColorOptionsPage(myDialog->createTab("Colors"));
  myKeyBindingsPage = new KeyBindingsPage(fbreader, myDialog->createTab("Keys"));
}

OptionsDialog::~OptionsDialog() {
  delete myScrollingPage;
  delete myFormatPage;
  delete myStylePage;
  delete myColorPage;
  delete myKeyBindingsPage;
  delete myDialog;
}

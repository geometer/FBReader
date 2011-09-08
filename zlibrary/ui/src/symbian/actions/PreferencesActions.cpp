#include <QApplication>
#include <QMessageBox>

#include <ZLDialogManager.h>

#include "../../../../../fbreader/src/optionsDialog/AbstractOptionsDialog.h"

#include "../menu/DrillDownMenu.h"
#include "../menu/PreferencesMenuBuilder.h"

#include "PreferencesActions.h"


ShowPreferencesMenuItemAction::ShowPreferencesMenuItemAction()
	:  myPreferenceResource(ZLResource::resource(PreferencesMenuCode::PREFERENCES) ) {  }

shared_ptr<ZLOptionsDialog> ShowPreferenceMenuAction::createDialog(std::string key) {
   return ZLDialogManager::Instance().createOptionsDialog(ZLResourceKey(key), new OptionsApplyRunnable());
}

void ShowPreferencesMenuItemAction::run() {
   myDialog = new DrillDownMenuDialog(qApp->activeWindow());
   myRootMenu = new DrillDownMenu;
   myDialog->showDrillDownMenu(myRootMenu);

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

   myDialog->run();
   delete myDialog;
}

void ShowPreferencesMenuItemAction::addItem(std::string name) {
   const ZLResource& item = myPreferenceResource[name];
   myRootMenu->addItem( item.value(), new ShowPreferenceMenuAction( item.name() ) );
}

void ShowPreferenceMenuAction::run() {
   shared_ptr<ZLOptionsDialog> dialog;
   dialog = createDialog(mySectionName);
   if ( PreferencesMenuBuilder::addOptionsBySection(mySectionName,dialog->createTab(ZLResourceKey(mySectionName))) == false) {
	   QMessageBox::warning(0,"","no matches for tab name in preferences dialog");
	   return;
   }
   dialog->run();
}


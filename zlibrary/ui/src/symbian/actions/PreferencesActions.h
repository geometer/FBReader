#ifndef __SHOWPREFERENCESACTIONS_H__
#define __SHOWPREFERENCESACTIONS_H__

#include <ZLOptionsDialog.h>

#include "DrillDownMenuActions.h"

class DrillDownMenu;

class ShowPreferencesMenuItemAction : public ZLApplication::Action {
public:
		ShowPreferencesMenuItemAction();
		void run();
private:
		void addItem(std::string name);
private:
		const ZLResource& myPreferenceResource;
		DrillDownMenu* myRootMenu;
		DrillDownMenuDialog* myDialog;
};

class ShowPreferenceMenuAction : public ZLApplication::Action {
public:
		ShowPreferenceMenuAction(std::string sectionName) : mySectionName(sectionName) { }
		void run();
private:
		shared_ptr<ZLOptionsDialog> createDialog(std::string key);
private:
		std::string mySectionName;
};

#endif /* __SHOWPREFERENCESACTIONS_H__ */

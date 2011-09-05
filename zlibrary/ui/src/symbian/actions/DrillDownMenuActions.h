#ifndef __DRILLDOWNMENUACTIONS_H__
#define __DRILLDOWNMENUACTIONS_H__

#include <ZLApplication.h>

#include "../menu/DrillDownMenu.h"

class MenuAction : public ZLApplication::Action {
public:
	MenuAction(DrillDownMenuDialog* dialog);
protected:
    DrillDownMenuDialog* myDialog;
};

class MenuActionCloseWrapper : public MenuAction {
public:
	MenuActionCloseWrapper(DrillDownMenuDialog* dialog, ZLApplication::Action* action);
    void run();
private:
    ZLApplication::Action* myAction;
};

class OpenNodeMenuAction : public MenuAction {
public:
	OpenNodeMenuAction(DrillDownMenuDialog* dialog);
    void run();
    virtual void run_init() = 0;

protected:
    DrillDownMenu* myMenu;
};

inline MenuAction::MenuAction(DrillDownMenuDialog* dialog) : myDialog(dialog) { }
inline MenuActionCloseWrapper::MenuActionCloseWrapper(DrillDownMenuDialog* dialog, ZLApplication::Action* action) : MenuAction(dialog), myAction(action) { }
inline OpenNodeMenuAction::OpenNodeMenuAction(DrillDownMenuDialog* dialog) : MenuAction(dialog), myMenu(0) { }

#endif /* __DRILLDOWNMENUACTIONS_H__ */

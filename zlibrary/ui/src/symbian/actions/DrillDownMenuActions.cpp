#include "DrillDownMenuActions.h"

void OpenNodeMenuAction::run() {
    if (myMenu == 0) {
        myMenu = new DrillDownMenu;
        run_init();
    }
    myDialog->showDrillDownMenu(myMenu);
}

void MenuActionCloseWrapper::run() {
    myAction->checkAndRun();
    myDialog->close();
}

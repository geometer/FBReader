#ifndef __ZLQTAPPLICATIONWINDOW_H__
#define __ZLQTAPPLICATIONWINDOW_H__

#include <map>

#include <QtGui/QMainWindow>
#include <QtGui/QAction>

class QMenu;
class VolumeKeysCapturer;
class QFocusEvent; ////

class ZLPopupData;

class DrillDownMenuDialog;
class DrillDownMenu;
class DrillDownMenuItem;

#include "../../../../core/src/application/ZLApplicationWindow.h"
#include "../../../../core/src/application/ZLMenu.h"

#include "../actions/DrillDownMenuActions.h"


class ZLQtApplicationWindow : public QMainWindow, public ZLApplicationWindow {
	Q_OBJECT

public:
	ZLQtApplicationWindow(ZLApplication *application);
	~ZLQtApplicationWindow();

	void setFocusToMainWidget();

private:
	ZLViewWidget *createViewWidget();
	void addToolbarItem(ZLToolbar::ItemPtr item);
	void init();
	void processAllEvents();
	void close();

	void initMenu();
	void addMenuItem(ZLMenu::ItemPtr item);
	void setMenuItemState(ZLMenu::ItemPtr item, bool visible, bool enabled);

	void grabAllKeys(bool grab);

	void setCaption(const std::string &caption);

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen=true);

	void setToggleButtonState(const ZLToolbar::ToggleButtonItem &button);
	void setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled);

	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event);

private:
		VolumeKeysCapturer* myVolumeKeyCapture;
private:

	friend class ZLQtMenuAction;
	std::map<const ZLMenu::Item*,DrillDownMenuItem*> myMenuItems;
	std::map<const ZLToolbar::MenuButtonItem*,size_t> myPopupIdMap;

private slots:
	void showMenu();

private:
	DrillDownMenuDialog* myMenuDialog;
	DrillDownMenu* myMenu;

};

class ZLQtMenuAction : public MenuAction {
public:
	ZLQtMenuAction(ZLQtApplicationWindow* parent, DrillDownMenuDialog* dialog, ZLMenubar::PlainItem& item);
	void run();
private:
	ZLMenubar::PlainItem& myItem;
	ZLQtApplicationWindow* myParent;
};

class ZLQtRunPopupAction : public QAction {
	Q_OBJECT

public:
	ZLQtRunPopupAction(QObject *parent, shared_ptr<ZLPopupData> data, size_t index);
	~ZLQtRunPopupAction();

private Q_SLOTS:
	void onActivated();

private:
	shared_ptr<ZLPopupData> myData;
	const size_t myIndex;
};

#endif /* __ZLQTAPPLICATIONWINDOW_H__ */

#ifndef __ZLQTAPPLICATIONWINDOW_H__
#define __ZLQTAPPLICATIONWINDOW_H__

#include <map>

#include <QtGui/QMainWindow>
#include <QtGui/QAction>
#include <QtGui/QMenuBar>

class QMenu;
//class QMenuBar;
class VolumeKeysCapturer;
class QFocusEvent; ////

class ZLPopupData;


#include "../../../../core/src/application/ZLApplicationWindow.h"
#include "../../../../core/src/application/ZLMenu.h"


class ZLQtMenuBar : public QMenuBar {
    Q_OBJECT

public:
    explicit ZLQtMenuBar(QWidget *parent = 0): QMenuBar(parent) { }
protected:
    void  focusOutEvent ( QFocusEvent * );
};

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
	void _addMenuItem(ZLMenu::ItemPtr item, QMenu* menuOrMenuBar=0);
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
	ZLQtMenuBar *myMenuBar;

	friend class ZLQtMenuBarAction;
	std::map<const ZLMenu::Item*,QAction*> myMenuActions;
	std::map<const ZLToolbar::MenuButtonItem*,size_t> myPopupIdMap;
};

class ZLQtMenuBarAction : public QAction {
        Q_OBJECT

public:
        ZLQtMenuBarAction(ZLQtApplicationWindow *parent, ZLMenubar::PlainItem &item);

private Q_SLOTS:
        void onActivated();

private:
        ZLMenubar::PlainItem &myItem;
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

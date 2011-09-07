#ifndef __ZLQTAPPLICATIONWINDOW_H__
#define __ZLQTAPPLICATIONWINDOW_H__

#include <map>

#include <QtGui/QMainWindow>
#include <QtGui/QAction>
#include <QtGui/QCursor>
#include <QtGui/QMenuBar>

class QDockWidget;
class QToolBar;
class QMenu;
//class QMenuBar;
class QToolButton;
class QLineEdit;
class VolumeKeysCapturer;
class QFocusEvent; ////

class ZLPopupData;


#include "../../../../core/src/desktop/application/ZLDesktopApplicationWindow.h"
#include "../../../../core/src/application/ZLMenu.h"


class ZLQtMenuBar : public QMenuBar {
    Q_OBJECT

public:
    explicit ZLQtMenuBar(QWidget *parent = 0): QMenuBar(parent) { }
protected:
    void  focusOutEvent ( QFocusEvent * );



};

class ZLQtApplicationWindow : public QMainWindow, public ZLDesktopApplicationWindow {
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

	void setHyperlinkCursor(bool hyperlink);

	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);

	void setToggleButtonState(const ZLToolbar::ToggleButtonItem &button);
	void setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled);

	void closeEvent(QCloseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event);

private:
		VolumeKeysCapturer* myVolumeKeyCapture;
private:
	QToolBar *myWindowToolBar;
	QToolBar *myFullscreenToolBar;
	ZLQtMenuBar *myMenuBar;
	QDockWidget *myDocWidget;
	QToolBar *toolbar(ToolbarType type) { return (type == WINDOW_TOOLBAR) ? myWindowToolBar : myFullscreenToolBar; }
	friend class ZLQtToolBarAction;
	friend class ZLQtMenuBarAction;
	std::map<const ZLToolbar::Item*,QAction*> myActions;
	std::map<const ZLMenu::Item*,QAction*> myMenuActions;
	std::map<const ZLToolbar::MenuButtonItem*,QToolButton*> myMenuButtons;
	std::map<const ZLToolbar::MenuButtonItem*,size_t> myPopupIdMap;

	bool myFullScreen;
	bool myWasMaximized;

	bool myCursorIsHyperlink;
	QCursor myStoredCursor;

private:
	class LineEditParameter : public VisualParameter {

	public:
		LineEditParameter(QToolBar *toolbar, ZLQtApplicationWindow &window, const ZLToolbar::ParameterItem &textFieldItem);
		QAction *action() const;
		void restoreOldValue();

	private:
		std::string internalValue() const;
		void internalSetValue(const std::string &value);
		void setValueList(const std::vector<std::string> &values) {}

	private:
		QLineEdit *myEdit;
		QAction *myAction;
	};

friend class ZLQtLineEdit;
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


class ZLQtToolBarAction : public QAction {
	Q_OBJECT

public:
	ZLQtToolBarAction(ZLQtApplicationWindow *parent, ZLToolbar::AbstractButtonItem &item);

private Q_SLOTS:
	void onActivated();

private:
	ZLToolbar::AbstractButtonItem &myItem;
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

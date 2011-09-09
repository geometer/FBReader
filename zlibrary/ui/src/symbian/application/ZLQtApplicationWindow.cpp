#include <QtGui/QApplication>
#include <QtGui/QPixmap>
#include <QtGui/QImage>
#include <QtGui/QIcon>
#include <QtGui/QToolBar>
#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <QtGui/QToolButton>
#include <QtGui/QLayout>
#include <QtGui/QWheelEvent>
#include <QtGui/QDockWidget>
#include <QtGui/QPushButton>
#include <QtCore/QObjectList>
#include <QtGui/QMessageBox>
#include <QtGui/QFocusEvent>
#include <QAction>

#include <QtGui/QDesktopWidget>

#include <ZLibrary.h>
#include <ZLPopupData.h>

#include "ZLQtApplicationWindow.h"
#include "../dialogs/ZLQtDialogManager.h"
#include "../view/ZLQtViewWidget.h"
#include "../util/ZLQtKeyUtil.h"

#include "../menu/DrillDownMenu.h"

#include "../VolumeKeysCapturer.h"

// Why should I use ifdef's? Isn't it special for Symbian file?
//#ifdef SYMBIAN
#include "../actions/PreferencesActions.h"
#include "../actions/LibraryActions.h"
//#endif

void ZLQtDialogManager::createApplicationWindow(ZLApplication *application) const {
		new ZLQtApplicationWindow(application);
}

ZLQtMenuAction::ZLQtMenuAction(ZLQtApplicationWindow* parent, DrillDownMenuDialog* dialog,  ZLMenubar::PlainItem& item) : myParent(parent), MenuAction(dialog), myItem(item) { }

void ZLQtMenuAction::run() {
	myDialog->close();
	myParent->onMenuItemPress(myItem);
}

void ZLQtApplicationWindow::setToggleButtonState(const ZLToolbar::ToggleButtonItem &) { }

ZLQtApplicationWindow::ZLQtApplicationWindow(ZLApplication *application) :
		ZLApplicationWindow(application) {

		const std::string iconFileName = ZLibrary::ImageDirectory() + ZLibrary::FileNameDelimiter + ZLibrary::ApplicationName() + ".png";
        QPixmap icon(iconFileName.c_str());
        setWindowIcon(icon);

		// FIXME: Find the way to get somewhere this action names
		application->addAction("library", new ShowMenuLibraryAction());
		application->addAction("preferences", new ShowPreferencesMenuItemAction());

		myMenuDialog = new DrillDownMenuDialog(this);
		myMenu = new DrillDownMenu;

		myVolumeKeyCapture = new VolumeKeysCapturer(this);
}



void ZLQtApplicationWindow::init() {
		ZLApplicationWindow::init();
		setGeometry(qApp->desktop()->availableGeometry());

		//TODO add ZLResource here
		const std::string& mainMenu = "Menu";
		myShowMenuAction = new QAction(mainMenu.c_str(),this);
		myShowMenuAction->setSoftKeyRole( QAction::PositiveSoftKey );
		connect(myShowMenuAction, SIGNAL(triggered()), this, SLOT(showMenu()));
		addAction( myShowMenuAction );

		myMenuDialog->showDrillDownMenu(myMenu);

		setFullscreen();
}

void ZLQtApplicationWindow::showMenu() {
	myMenuDialog->setFixedHeight(  qApp->desktop()->availableGeometry().height() * 2 / 3 );
	myMenuDialog->runNoFullScreen();
//	myShowMenuAction->setSoftKeyRole(QAction::PositiveSoftKey);
//	addAction(myShowMenuAction);
	update();
	refresh();
}

void ZLQtApplicationWindow::setFullscreen(bool fullscreen) {
		showFullScreen();
		Qt::WindowFlags flags = windowFlags();
		flags |= Qt::WindowSoftkeysVisibleHint;
		setWindowFlags( flags );
		showFullScreen();
}

bool ZLQtApplicationWindow::isFullscreen() const {
		return true;
}

void ZLQtApplicationWindow::initMenu() {
	ZLApplicationWindow::initMenu();
}

void ZLQtApplicationWindow::addMenuItem(ZLMenu::ItemPtr item) {
	DrillDownMenuItem* menuItem = 0;
	switch (item->type()) {
			case ZLMenu::Item::ITEM:
				{
					ZLMenubar::PlainItem& plainItem = (ZLMenubar::PlainItem&)*item;
					QString text = QString::fromUtf8(plainItem.name().c_str());
					menuItem = new DrillDownMenuItem(text, new ZLQtMenuAction(this,myMenuDialog,plainItem) );
					myMenu->addItem(menuItem);
				}
				break;
			case ZLMenu::Item::SUBMENU:
					//TODO make support for submenu
					break;
			case ZLMenu::Item::SEPARATOR:
					//TODO make support for separator
					break;
	}
	if (menuItem != 0) {
			myMenuItems[&*item] = menuItem;
	}
}

void ZLQtApplicationWindow::setMenuItemState(ZLMenu::ItemPtr item, bool visible, bool enabled) {
	DrillDownMenuItem *menuItem = myMenuItems[&*item];
	if (menuItem != 0) {
		menuItem->setHidden(!visible);
	}
}

ZLQtApplicationWindow::~ZLQtApplicationWindow() {
		// delete myVolumeKeyCapture; // TODO fix app crashing here
}

void ZLQtApplicationWindow::keyPressEvent(QKeyEvent *event) {
        application().doActionByKey(ZLQtKeyUtil::keyName(event));
}


void ZLQtApplicationWindow::wheelEvent(QWheelEvent *event) {
        if (event->orientation() == Qt::Vertical) {
                if (event->delta() > 0) {
                        application().doActionByKey(ZLApplication::MouseScrollUpKey);
                } else {
                        application().doActionByKey(ZLApplication::MouseScrollDownKey);
                }
        }
}

void ZLQtApplicationWindow::closeEvent(QCloseEvent *event) {
        if (application().closeView()) {
                event->accept();
        } else {
                event->ignore();
        }
}

void ZLQtApplicationWindow::addToolbarItem(ZLToolbar::ItemPtr) {
	// do nothing; no toolbars
}

ZLQtRunPopupAction::ZLQtRunPopupAction(QObject *parent, shared_ptr<ZLPopupData> data, size_t index) : QAction(parent), myData(data), myIndex(index) {
        setText(QString::fromUtf8(myData->text(myIndex).c_str()));
        connect(this, SIGNAL(triggered()), this, SLOT(onActivated()));
}

ZLQtRunPopupAction::~ZLQtRunPopupAction() {
}

void ZLQtRunPopupAction::onActivated() {
        myData->run(myIndex);
}

void ZLQtApplicationWindow::setToolbarItemState(ZLToolbar::ItemPtr, bool, bool) { }

void ZLQtApplicationWindow::processAllEvents() {
        qApp->processEvents();
}

ZLViewWidget *ZLQtApplicationWindow::createViewWidget() {
		ZLQtViewWidget *viewWidget = new ZLQtViewWidget(this, &application());
		setCentralWidget(viewWidget->widget());
		////viewWidget->widget()->show();  //// because central widget should not be showed before Window shows;
		return viewWidget;
}

void ZLQtApplicationWindow::close() {
        QMainWindow::close();
}

void ZLQtApplicationWindow::grabAllKeys(bool mode) {  }

void ZLQtApplicationWindow::setCaption(const std::string &caption) {
        QMainWindow::setWindowTitle(QString::fromUtf8(caption.c_str()));
}

void ZLQtApplicationWindow::setFocusToMainWidget() {
        centralWidget()->setFocus();
}

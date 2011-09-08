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

#include <QtGui/QDesktopWidget>

#include <ZLibrary.h>
#include <ZLPopupData.h>

#include "ZLQtApplicationWindow.h"
#include "../dialogs/ZLQtDialogManager.h"
#include "../view/ZLQtViewWidget.h"
#include "../util/ZLQtKeyUtil.h"

#include "../VolumeKeysCapturer.h"

// Why should I use ifdef's? Isn't it special for Symbian file?
//#ifdef SYMBIAN
#include "../actions/PreferencesActions.h"
#include "../actions/LibraryActions.h"
//#endif

void ZLQtDialogManager::createApplicationWindow(ZLApplication *application) const {
		new ZLQtApplicationWindow(application);
}

void  ZLQtMenuBar::focusOutEvent ( QFocusEvent * event) {
}


ZLQtMenuBarAction::ZLQtMenuBarAction(ZLQtApplicationWindow *parent, ZLMenubar::PlainItem &item) : QAction(parent), myItem(item) {
    QString text = QString::fromUtf8(myItem.name().c_str());
    setText(text);
    setToolTip(text);
    connect(this, SIGNAL(triggered()), this, SLOT(onActivated()));
}

void ZLQtMenuBarAction::onActivated() {
        ((ZLQtApplicationWindow*)parent())->onMenuItemPress(myItem);
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

        ////menuBar()->hide();
        myMenuBar = new ZLQtMenuBar(this);
        setMenuBar(myMenuBar);

		myVolumeKeyCapture = new VolumeKeysCapturer(this);
}



void ZLQtApplicationWindow::init() {
		ZLApplicationWindow::init();
        //setGeometry(qApp->desktop()->availableGeometry());
		setFullscreen();
}

void ZLQtApplicationWindow::initMenu() {
	ZLApplicationWindow::initMenu();
    myMenuBar->setContextMenuPolicy(Qt::NoContextMenu);
}

void ZLQtApplicationWindow::addMenuItem(ZLMenu::ItemPtr item) {
	_addMenuItem(item);
}

void ZLQtApplicationWindow::_addMenuItem(ZLMenu::ItemPtr item, QMenu* addToMenu) {
        QWidget* menuOrMenuBar = addToMenu == 0 ? (QWidget*) myMenuBar : (QWidget*)addToMenu;
        QAction *action = 0;
        QMenu* menu = 0;
        switch (item->type()) {
                case ZLMenu::Item::ITEM:
                    action = new ZLQtMenuBarAction(this, (ZLMenubar::PlainItem&)*item);
                    menuOrMenuBar->addAction(action);
                    break;
                case ZLMenu::Item::SUBMENU:
                        menu = new QMenu( QString::fromUtf8(((ZLMenubar::Submenu&)*item).menuName().c_str()), menuOrMenuBar );
                        for (ZLMenu::ItemVector::const_iterator it = ((ZLMenubar::Submenu&)*item).items().begin();
                                                                it != ((ZLMenubar::Submenu&)*item).items().end(); ++it) {
								_addMenuItem(*it, menu);
                        }
                        menuOrMenuBar->addAction(menu->menuAction());
                        break;
                case ZLMenu::Item::SEPARATOR:
                        if (addToMenu != 0) {
                            ((QMenu&)*menuOrMenuBar).addSeparator();
                        } else {
                            ((ZLQtMenuBar&)*menuOrMenuBar).addSeparator();
                        }

                        break;
        }
        if (action != 0) {
                myMenuActions[&*item] = action;
        }
}


void ZLQtApplicationWindow::setMenuItemState(ZLMenu::ItemPtr item, bool visible, bool enabled) {
	QAction *action = myMenuActions[&*item];
	if (action != 0) {
		action->setEnabled(enabled);
		action->setVisible(visible);
	}
}

ZLQtApplicationWindow::~ZLQtApplicationWindow() {

		// delete myVolumeKeyCapture; // TODO fix app crashing here

        for (std::map<const ZLMenu::Item*,QAction*>::iterator it = myMenuActions.begin(); it != myMenuActions.end(); ++it) {
                if (it->second != 0) {
                        delete it->second;
                }
        }

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

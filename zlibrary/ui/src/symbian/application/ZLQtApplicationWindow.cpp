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


ZLQtToolBarAction::ZLQtToolBarAction(ZLQtApplicationWindow *parent, ZLToolbar::AbstractButtonItem &item) : QAction(parent), myItem(item) {
        static std::string imagePrefix = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter;
        QPixmap icon((imagePrefix + myItem.iconName() + ".png").c_str());
        setIcon(QIcon(icon));
        QSize size = icon.size();
        if (item.type() == ZLToolbar::Item::TOGGLE_BUTTON) {
                setCheckable(true);
        }
        QString text = QString::fromUtf8(myItem.tooltip().c_str());
        setText(text);
        setToolTip(text);
        connect(this, SIGNAL(triggered()), this, SLOT(onActivated()));
}

void ZLQtToolBarAction::onActivated() {
        ((ZLQtApplicationWindow*)parent())->onButtonPress(myItem);
}

void ZLQtApplicationWindow::setToggleButtonState(const ZLToolbar::ToggleButtonItem &button) {
        myActions[&button]->setChecked(button.isPressed());
}

ZLQtApplicationWindow::ZLQtApplicationWindow(ZLApplication *application) :
        ZLDesktopApplicationWindow(application),
        myFullscreenToolBar(0),
        myDocWidget(0),
        myFullScreen(false),
        myWasMaximized(false),
        myCursorIsHyperlink(false) {

		const std::string iconFileName = ZLibrary::ImageDirectory() + ZLibrary::FileNameDelimiter + ZLibrary::ApplicationName() + ".png";
        QPixmap icon(iconFileName.c_str());
        setWindowIcon(icon);

        myWindowToolBar = new QToolBar(this);
        myWindowToolBar->setFocusPolicy(Qt::NoFocus);
        myWindowToolBar->setMovable(false);
        //addToolBar(myWindowToolBar);
        myWindowToolBar->setIconSize(QSize(12, 12));

        if (hasFullscreenToolbar()) {
                myFullscreenToolBar = new QToolBar();
                myFullscreenToolBar->setMovable(false);
                myFullscreenToolBar->setIconSize(QSize(32, 32));
                myFullscreenToolBar->hide();
        }

        resize(myWidthOption.value(), myHeightOption.value());
        //move(myXOption.value(), myYOption.value());


        ////menuBar()->hide();
        myMenuBar = new ZLQtMenuBar(this);
        setMenuBar(myMenuBar);

		#ifdef SYMBIAN
			myVolumeKeyCapture = new VolumeKeysCapturer(this);
		#endif
}



void ZLQtApplicationWindow::init() {
        ZLDesktopApplicationWindow::init();

        //setGeometry(qApp->desktop()->availableGeometry());

        //m_bgc = (void*) CEikButtonGroupContainer::Current();

        switch (myWindowStateOption.value()) {
                case NORMAL:
                    show();
                    break;
                case MAXIMIZED:
                    showMaximized();
                    break;
                case FULLSCREEN:
                    setFullscreen(true);
                    break;
        }



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
                        //// to avoid crosses initialization
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
                            //((QMenuBar&)*menuOrMenuBar).addSeparator();
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

        if (isFullscreen()) {
                myWindowStateOption.setValue(FULLSCREEN);
        } else if (isMaximized()) {
                myWindowStateOption.setValue(MAXIMIZED);
        } else {
                myWindowStateOption.setValue(NORMAL);
                QPoint position = pos();
                if (position.x() != -1) {
                        myXOption.setValue(position.x());
                }
                if (position.y() != -1) {
                        myYOption.setValue(position.y());
                }
                myWidthOption.setValue(width());
                myHeightOption.setValue(height());
        }
        for (std::map<const ZLToolbar::Item*,QAction*>::iterator it = myActions.begin(); it != myActions.end(); ++it) {
                if (it->second != 0) {
                        delete it->second;
                }
        }
        for (std::map<const ZLMenu::Item*,QAction*>::iterator it = myMenuActions.begin(); it != myMenuActions.end(); ++it) {
                if (it->second != 0) {
                        delete it->second;
                }
        }

}

void ZLQtApplicationWindow::setFullscreen(bool fullscreen) {
        if (fullscreen == myFullScreen) {
                return;
        }
        myFullScreen = fullscreen;
        if (myFullScreen) {
                myWasMaximized = isMaximized();
                ////myWindowToolBar->hide();

                // to show soft buttons in fullscreen mode:
                showFullScreen();
                Qt::WindowFlags flags = windowFlags();
                flags |= Qt::WindowSoftkeysVisibleHint;
                setWindowFlags( flags );
                showFullScreen();
        } else {
                ////myWindowToolBar->show();
                showNormal();
                if (myWasMaximized) {
                        showMaximized();
                }
//                if (myDocWidget != 0) {
//                        //myFullscreenToolBar->hide();
//                        myDocWidget->hide();
//                }
        }
}

bool ZLQtApplicationWindow::isFullscreen() const {
        return myFullScreen;
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

void ZLQtApplicationWindow::addToolbarItem(ZLToolbar::ItemPtr item) {
        QToolBar *tb = toolbar(type(*item));
        QAction *action = 0;

        switch (item->type()) {
                case ZLToolbar::Item::PLAIN_BUTTON:
                case ZLToolbar::Item::TOGGLE_BUTTON:
                        action = new ZLQtToolBarAction(this, (ZLToolbar::AbstractButtonItem&)*item);
                        tb->addAction(action);
                        break;
                case ZLToolbar::Item::MENU_BUTTON:
                {
                        ZLToolbar::MenuButtonItem &buttonItem = (ZLToolbar::MenuButtonItem&)*item;
                        QToolButton *button = new QToolButton(tb);
                        button->setFocusPolicy(Qt::NoFocus);
                        button->setDefaultAction(new ZLQtToolBarAction(this, buttonItem));
                        button->setMenu(new QMenu(button));
                        button->setPopupMode(QToolButton::MenuButtonPopup);
                        action = tb->addWidget(button);
                        myMenuButtons[&buttonItem] = button;
                        shared_ptr<ZLPopupData> popupData = buttonItem.popupData();
                        myPopupIdMap[&buttonItem] =
                                popupData.isNull() ? (size_t)-1 : (popupData->id() - 1);
                        break;
                }
                case ZLToolbar::Item::TEXT_FIELD:
                case ZLToolbar::Item::SEARCH_FIELD:
                {
                        ZLToolbar::ParameterItem &textFieldItem =
                                (ZLToolbar::ParameterItem&)*item;
                        LineEditParameter *para = new LineEditParameter(tb, *this, textFieldItem);
                        addVisualParameter(textFieldItem.parameterId(), para);
                        action = para->action();
                        break;
                }
                case ZLToolbar::Item::SEPARATOR:
                        action = tb->addSeparator();
                        break;
        }

        if (action != 0) {
                myActions[&*item] = action;
        }
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

void ZLQtApplicationWindow::setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled) {
        QAction *action = myActions[&*item];
        if (action != 0) {
                action->setEnabled(enabled);
                action->setVisible(visible);
        }
        switch (item->type()) {
                default:
                        break;
                case ZLToolbar::Item::MENU_BUTTON:
                {
                        ZLToolbar::MenuButtonItem &buttonItem = (ZLToolbar::MenuButtonItem&)*item;
                        shared_ptr<ZLPopupData> data = buttonItem.popupData();
                        if (!data.isNull() && (data->id() != myPopupIdMap[&buttonItem])) {
                                myPopupIdMap[&buttonItem] = data->id();
                                QToolButton *button = myMenuButtons[&buttonItem];
                                QMenu *menu = button->menu();
                                menu->clear();
                                const size_t count = data->count();
                                for (size_t i = 0; i < count; ++i) {
                                        menu->addAction(new ZLQtRunPopupAction(menu, data, i));
                                }
                        }
                        break;
                }
        }
}

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

void ZLQtApplicationWindow::grabAllKeys(bool mode) {
}

void ZLQtApplicationWindow::setCaption(const std::string &caption) {
        QMainWindow::setWindowTitle(QString::fromUtf8(caption.c_str()));
}

void ZLQtApplicationWindow::setHyperlinkCursor(bool hyperlink) {
        if (hyperlink == myCursorIsHyperlink) {
                return;
        }
        myCursorIsHyperlink = hyperlink;
        if (hyperlink) {
                myStoredCursor = cursor();
                setCursor(Qt::PointingHandCursor);
        } else {
                setCursor(myStoredCursor);
        }
}

void ZLQtApplicationWindow::setFocusToMainWidget() {
        centralWidget()->setFocus();
}

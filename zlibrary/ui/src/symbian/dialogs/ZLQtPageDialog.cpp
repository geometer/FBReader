#include <QtGui/QApplication>
#include <QtGui/QVBoxLayout>
#include <QtGui/QAction>
#include <QtGui/QPushButton>

#include "ZLQtPageDialog.h"
#include "ZLQtDialogContent.h"

#include "../menu/DrillDownMenu.h"

WrapperAction::WrapperAction(shared_ptr<ZLTreeAction> runnable) : myRunnable(runnable) {

}

bool WrapperAction::isVisible() const {
    return myRunnable->makesSense();
}

void WrapperAction::run() {
    //ZLDialogManager::Instance().wait(ZLResourceKey("loadingBook"),*myRunnable);
    myRunnable->run();
}

PageMenu::PageMenu(const ZLTreePageNode& pageNode, QAction* action, QObject* parent): QObject(parent), myPageNode(pageNode), myAction(action) {
    setActionButton();
}

std::vector<shared_ptr<ZLTreeAction> > PageMenu::filterSensibleActions(const std::vector<shared_ptr<ZLTreeAction> > & actions) {
    std::vector<shared_ptr<ZLTreeAction> > senseActions;
    for (size_t i=0; i<actions.size(); ++i) {
        if (actions[i]->makesSense()) {
            senseActions.push_back(actions[i]);
        }
    }
    return senseActions;
}

void PageMenu::setActionButton() {
    //TODO setActionButton should calls when some changes are happened
    if (!myAction) {
        return;
    }
    const std::vector<shared_ptr<ZLTreeAction> > senseActions = filterSensibleActions(myPageNode.actions());
    if (senseActions.size() == 0) {
        myAction->setText(QString());
        return;
    }
    if (senseActions.size() == 1) {
        myAction->setText(QString::fromStdString(myPageNode.actionText(senseActions[0])));
    } else {
        //TODO add ZLResource here
        myAction->setText("Options");
    }
}

void PageMenu::onFinish(int) {
    //TODO remove this slot and signal; DrillDownMenu dialog should close automatically by other way
    emit drillDownMenuClose();
}

void PageMenu::activate() {
    const std::vector<shared_ptr<ZLTreeAction> > senseActions = filterSensibleActions(myPageNode.actions());
    if (senseActions.size() == 0) {
        return;
    }
    if (senseActions.size() == 1) {
        senseActions.at(0)->run();
        return;
    }
    DrillDownMenuDialog dialog;
    connect(this, SIGNAL(drillDownMenuClose()), &dialog, SLOT(close()));
    DrillDownMenu* menu = new DrillDownMenu;
    for (size_t i=0; i < senseActions.size(); ++i) {
        menu->addItem(myPageNode.actionText(senseActions[i]), new WrapperAction(senseActions[i]));
    }
    dialog.showDrillDownMenu(menu);
    dialog.runNoFullScreen();
}

ZLQtPageDialog::ZLQtPageDialog(const ZLTreePageNode& pageNode, QWidget* parent) : QDialog(parent), myPageNode(pageNode), myContent(pageNode.content()) {

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(static_cast<ZLQtDialogContent&>(*myContent).widget());

    const ZLResource& back = ZLResource::resource("dialog")["button"]["back"];
    QAction* action = new QAction(QString::fromStdString(back.value()),this);
#ifdef __SYMBIAN__
    action->setSoftKeyRole( QAction::NegativeSoftKey );
#endif
    connect(action, SIGNAL(triggered()), this, SLOT(accept()));
    addAction( action );
#ifndef __SYMBIAN__
    QPushButton* button = new QPushButton(QString::fromStdString(back.value()));
    connect(button, SIGNAL(clicked()), this, SLOT(accept()));
    layout->addWidget(button);
#endif

    QAction* pageMenuAction = new QAction(this);
#ifdef __SYMBIAN__
    pageMenuAction->setSoftKeyRole( QAction::PositiveSoftKey );
#endif
    myPageMenu = new PageMenu(pageNode, pageMenuAction, this);
    addAction(pageMenuAction);
    connect(pageMenuAction, SIGNAL(triggered()), myPageMenu, SLOT(activate()));
    connect(this,SIGNAL(finished(int)), myPageMenu, SLOT(onFinish(int)));
#ifndef __SYMBIAN__
        QPushButton* button = new QPushButton(pageMenuAction->text());
        connect(button, SIGNAL(clicked()), myPageMenu, SLOT(activate()));
        layout->addWidget(button);
#endif

    this->setLayout(layout);
}

ZLQtPageDialog::~ZLQtPageDialog() {
    //TODO reimplement the way to use inner widget in ZLQtDialogContent
    //(if not set parent to 0, it will crash, cause inner widget will be deleted)
    //static_cast<ZLQtDialogContent&>(*myContent).widget()->setParent(0);
}

bool ZLQtPageDialog::run() {
#ifdef __SYMBIAN__
    setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);
#endif
    setWindowState(Qt::WindowFullScreen);
    return exec() == QDialog::Accepted;
}

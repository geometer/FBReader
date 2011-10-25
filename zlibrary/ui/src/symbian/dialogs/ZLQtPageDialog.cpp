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

PageMenu::PageMenu(const ZLTreePageNode& pageNode): myPageNode(pageNode), myWidget(0) {
    myAction = new QAction(this);
#ifdef __SYMBIAN__
    myAction->setSoftKeyRole( QAction::PositiveSoftKey );
#endif
    connect(myAction, SIGNAL(triggered()), this, SLOT(activate()));
}

void PageMenu::setWidget(QWidget* widget) {
    myWidget = widget;
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
    if (!myWidget) {
        return;
    }
    const std::vector<shared_ptr<ZLTreeAction> > senseActions = filterSensibleActions(myPageNode.actions());
    if (senseActions.size() == 0) {
        myWidget->removeAction(myAction);
        return;
    }
    if (senseActions.size() == 1) {
        myAction->setText(QString::fromStdString(myPageNode.actionText(senseActions[0])));
    } else {
        //TODO add ZLResource here
        myAction->setText("Menu");
    }
    myWidget->addAction(myAction);
#ifndef __SYMBIAN__
        QPushButton* button = new QPushButton(myAction->text());
        connect(button, SIGNAL(clicked()), this, SLOT(activate()));
        myWidget->layout()->addWidget(button);
#endif
}

void PageMenu::finished(int) {
    emit drillDownMenuClose();
}

void PageMenu::activate() {
    const std::vector<shared_ptr<ZLTreeAction> > senseActions = filterSensibleActions(myPageNode.actions());
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

ZLQtPageDialog::ZLQtPageDialog(const ZLTreePageNode& pageNode, QWidget* parent) : QDialog(parent), myPageNode(pageNode), myContent(pageNode.content()), myPageMenu(pageNode) {
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

    this->setLayout(layout);

    connect(this,SIGNAL(finished(int)), &myPageMenu, SLOT(finished(int)));
}

bool ZLQtPageDialog::run() {
    myPageMenu.setWidget(this);
#ifdef __SYMBIAN__
    setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);
#endif
    setWindowState(Qt::WindowFullScreen);
    return exec() == QDialog::Accepted;
}

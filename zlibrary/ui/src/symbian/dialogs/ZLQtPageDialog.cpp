#include <QtGui/QApplication>
#include <QtGui/QVBoxLayout>
#include <QtGui/QAction>
#include <QtGui/QPushButton>

#include "ZLQtPageDialog.h"
#include "ZLQtDialogContent.h"

ZLQtPageDialog::ZLQtPageDialog(shared_ptr<ZLDialogContent> content) : QDialog(qApp->activeWindow()), myContent(content) {
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
}

bool ZLQtPageDialog::run() {
#ifdef __SYMBIAN__
    setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);
#endif
    setWindowState(Qt::WindowFullScreen);
    return exec() == QDialog::Accepted;
}

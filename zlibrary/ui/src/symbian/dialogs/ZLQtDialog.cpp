#include <QtGui/QApplication>
#include <QtGui/QPushButton>
#include <QtGui/QAction>
#include <QtGui/QVBoxLayout>

#include <ZLDialogManager.h>

#include "ZLQtDialog.h"
#include "ZLQtDialogContent.h"
#include "ZLQtUtil.h"

ZLQtDialog::ZLQtDialog(const ZLResource &resource) : QDialog(qApp->activeWindow()) {
	setModal(true);
	setWindowTitle(::qtString(resource[ZLDialogManager::DIALOG_TITLE].value()));

	QVBoxLayout *layout = new QVBoxLayout(this);
        myTab = new ZLQtDialogContent(resource);
        layout->addWidget(((ZLQtDialogContent*)myTab)->widget());

}

ZLQtDialog::~ZLQtDialog() {
}

void ZLQtDialog::addButton(const ZLResourceKey &key, bool accept) {
	QAction* button = new QAction( ::qtButtonName(key) ,this);

	addAction( button );
#ifdef __SYMBIAN__

	button->setSoftKeyRole( accept ?  QAction::PositiveSoftKey : QAction::NegativeSoftKey );
#endif
	connect(button, SIGNAL(triggered()), this, accept ? SLOT(accept()) : SLOT(reject()));

#ifndef 	__SYMBIAN__
	QPushButton* realButton = new QPushButton(::qtButtonName(key), this );
	layout()->addWidget(realButton);
        connect(realButton, SIGNAL(clicked()), this, accept ? SLOT(accept()) : SLOT(reject()));
#endif
}

bool ZLQtDialog::run() {
	((ZLQtDialogContent*)myTab)->close();
	return exec() == QDialog::Accepted;
}

bool ZLQtDialog::runFullscreen() {
    #ifdef __SYMBIAN__
        setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);
        setWindowState(Qt::WindowFullScreen);
    #else
        setFixedSize(400,600);
    #endif

    return run();
}

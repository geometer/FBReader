#include <unistd.h>


#include <QtGui/QVBoxLayout>
#include <QtGui/QDesktopWidget>
#include <QtGui/QProgressDialog>
#include <QtCore/QDebug>

#include <ZLTimeManager.h>

#include "ZLQtProgressDialog.h"
#include "ZLQtTreeDialog.h" // for TreeActionListener
#include "ZLQtUtil.h"
#include "ZLQtDialogManager.h"

ZLQtProgressDialog::ZLQtProgressDialog(const ZLResourceKey &key) : ZLProgressDialog(key), myDialog(0) {

}

static const double COEF_PROGRESS_BAR_WIDTH = 0.75;

void ZLQtProgressDialog::run(ZLRunnable &runnable) {
		myDialog = new ZLQtWaitDialog(messageText());

		ThreadRunnable threadRunnable(&runnable);
		QObject::connect(&threadRunnable, SIGNAL(finished()), myDialog, SLOT(close()));
		threadRunnable.start();

		myDialog->exec();

		delete myDialog;
		myDialog = 0;
}

void ZLQtProgressDialog::run(TreeActionListener* listener) {
                if (!listener) {
                    return;
                }
                if (listener->isFinished()) {
                    qDebug() << Q_FUNC_INFO << "is finished yet!";
                    return;
                }
                ZLQtDialogManager& dialogManager = static_cast<ZLQtDialogManager&>(ZLQtDialogManager::Instance());
                myDialog = new ZLQtWaitDialog(messageText(),dialogManager.getParentWidget());

                QObject::connect(listener, SIGNAL(finishedHappened(std::string)), myDialog, SLOT(close()));
                myDialog->exec();

                delete myDialog;
                myDialog = 0;
}

void ZLQtProgressDialog::setMessage(const std::string &message) {
		if (myDialog == 0) {
				return;
		}

		myDialog->myLabel->setText(::qtString(message));

		myDialog->myLayout->invalidate();
		myDialog->repaint();
		qApp->processEvents();
}




ZLQtWaitDialog::ZLQtWaitDialog(const std::string &message, QWidget* parent) : QDialog(parent) {

	   // for what reasons we use processEvents here?
	   // qApp->processEvents();

		myLayout = new QVBoxLayout;

		myLabel = new QLabel(::qtString(message));
		myLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		myLabel->setWordWrap(true);

		myProgressBar = new QProgressBar;
		myProgressBar->setRange(0,0);
                myProgressBar->setFixedWidth(qApp->desktop()->geometry().width()*COEF_PROGRESS_BAR_WIDTH);

		myLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		myLayout->addWidget(myLabel);
		myLayout->addWidget(myProgressBar);

		this->setLayout(myLayout);

		//setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);
		setWindowState(Qt::WindowFullScreen);

//        qApp->processEvents();
//        usleep(5000);
//        qApp->processEvents();


}

 void ZLQtWaitDialog::paintEvent(QPaintEvent *event) {
        //myLayout->invalidate();
	QDialog::paintEvent(event);
 }

 void ZLQtWaitDialog::resizeEvent(QResizeEvent *event) {
    myProgressBar->setFixedWidth(event->size().width()*COEF_PROGRESS_BAR_WIDTH);
 }

ZLQtWaitDialog::~ZLQtWaitDialog() {
}

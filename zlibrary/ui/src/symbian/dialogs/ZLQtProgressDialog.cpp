#include <unistd.h>


#include <QVBoxLayout>
#include <QDesktopWidget>
#include <ZLTimeManager.h>

#include <QProgressDialog>

#include "ZLQtProgressDialog.h"
#include "ZLQtUtil.h"
#include <QDebug>

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

void ZLQtProgressDialog::setMessage(const std::string &message) {
		if (myDialog == 0) {
				return;
		}

		myDialog->myLabel->setText(::qtString(message));

		myDialog->myLayout->invalidate();
		myDialog->repaint();
		qApp->processEvents();
}




ZLQtWaitDialog::ZLQtWaitDialog(const std::string &message) : QDialog(0) {

	   // for what reasons we use processEvents here?
	   // qApp->processEvents();

		myLayout = new QVBoxLayout;

		myLabel = new QLabel(::qtString(message));
		myLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		myLabel->setWordWrap(true);

		myProgressBar = new QProgressBar;
		myProgressBar->setRange(0,0);
		myProgressBar->setFixedWidth( qApp->desktop()->availableGeometry().width()*COEF_PROGRESS_BAR_WIDTH );

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
	myLayout->invalidate();
	QDialog::paintEvent(event);
 }

ZLQtWaitDialog::~ZLQtWaitDialog() {
}

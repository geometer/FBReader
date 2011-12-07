#include <unistd.h>

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QDesktopWidget>
#include <QtGui/QProgressDialog>
#include <QtGui/QPainter>

#include <QtCore/QDebug>

#include <ZLibrary.h>
#include <ZLTimeManager.h>

#include "ZLQtProgressDialog.h"
#include "ZLQtTreeDialog.h" // for TreeActionListener
#include "ZLQtUtil.h"
#include "ZLQtDialogManager.h"

LoadingIcon::LoadingIcon(QWidget* parent) : QLabel(parent), myAngle(0) {
    const int ICON_WIDTH = 60;
    const int ICON_HEIGHT = ICON_WIDTH;
    QString iconFile = QString::fromStdString(ZLibrary::ApplicationImageDirectory()) +
                       QString::fromStdString(ZLibrary::FileNameDelimiter) +
                       "loading_icon.svg";
    qDebug() << "LoadingIcon" << iconFile;
    myTimer = new QTimer(this);
    myPixmap = QPixmap(iconFile);
    myPixmap = myPixmap.scaled(QSize(ICON_WIDTH, ICON_HEIGHT), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    this->setFixedSize(ICON_WIDTH, ICON_HEIGHT);
    connect(myTimer,SIGNAL(timeout()), this, SLOT(rotate()));
    this->hide();

}

void LoadingIcon::moveToPosition(QSize size) {
    move( (size.width() - myPixmap.width()) / 2,
          (size.height() - myPixmap.height()) / 2
        );
}

void LoadingIcon::start() {
    this->show();
    const int REFRESH_TIME = 100;
    myTimer->start(REFRESH_TIME);
}

void LoadingIcon::finish() {
    qDebug() << Q_FUNC_INFO;
    myTimer->stop();
    this->hide();
}

void LoadingIcon::rotate() {
    qDebug() << this->size();
    const int ANGLE_SPEED = 360/10;
    myAngle += ANGLE_SPEED;
    if (myAngle >= 360) {
        myAngle -= 360;
    }
    QPixmap tmpPixmap(myPixmap.size());
    tmpPixmap.fill(Qt::transparent);
    QPainter painter(&tmpPixmap);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.translate(myPixmap.width()/2,myPixmap.height()/2);
    painter.rotate(qreal(myAngle));
    painter.translate(-myPixmap.width()/2,-myPixmap.height()/2);
    painter.drawPixmap(0,0,myPixmap);
    painter.end();
    this->setPixmap(tmpPixmap);
    QWidget::raise();
}

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

                QHBoxLayout* layout = new QHBoxLayout;

                myLabel = new QLabel(oneWordWrapped(::qtString(message)));
                myLabel->setFont(MenuItemParameters::getWaitMessageFont());
                //myLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		myLabel->setWordWrap(true);

                myLoadingIcon = new LoadingIcon;
                myLoadingIcon->start();

//		myProgressBar = new QProgressBar;
//		myProgressBar->setRange(0,0);
//                myProgressBar->setFixedWidth(qApp->desktop()->geometry().width()*COEF_PROGRESS_BAR_WIDTH);

                layout->setAlignment(Qt::AlignHCenter);
                layout->addWidget(myLoadingIcon);
                layout->addWidget(myLabel);
                //myLayout->addWidget(myProgressBar);
                layout->setSpacing(10);

                myLayout->addLayout(layout);
                myLayout->addSpacing(myLoadingIcon->size().height()); // to be on bottom of center line

                myLayout->setAlignment(Qt::AlignVCenter);

		this->setLayout(myLayout);

		//setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);

#ifdef __SYMBIAN__
        //setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);
        setWindowState(Qt::WindowFullScreen);
#else
        setFixedSize(360,300);
#endif

//        qApp->processEvents();
//        usleep(5000);
//        qApp->processEvents();


}

 void ZLQtWaitDialog::paintEvent(QPaintEvent *event) {
        //myLayout->invalidate();
	QDialog::paintEvent(event);
 }

 void ZLQtWaitDialog::resizeEvent(QResizeEvent *event) {
    //myProgressBar->setFixedWidth(event->size().width()*COEF_PROGRESS_BAR_WIDTH);
    QDialog::resizeEvent(event);
 }

ZLQtWaitDialog::~ZLQtWaitDialog() {
}

QString ZLQtWaitDialog::oneWordWrapped(QString string) {
    const static QString SPACE = " ";
    const static QString WRAP = "\n";
    QString result = string.trimmed();

    int index = 0;
    while ((index = result.indexOf(SPACE + SPACE)) != -1) {
        result.replace(SPACE + SPACE, SPACE);
    }

    result.replace(SPACE, WRAP);
    return result;
}

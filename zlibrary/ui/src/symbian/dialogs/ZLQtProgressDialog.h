#ifndef __ZLQTPROGRESSDIALOG_H__
#define __ZLQTPROGRESSDIALOG_H__

#include <QtGui/QApplication>
#include <QtGui/QDialog>
#include <QtGui/QWidget>

#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QProgressBar>
#include <QtGui/QPaintEvent>
#include <QtCore/QThread>
#include <QtCore/QTimer>


#include <ZLProgressDialog.h>
#include <ZLRunnable.h>

#include "ZLQtTreeDialog.h"

class LoadingIcon : public QLabel {
    Q_OBJECT
public:
    LoadingIcon(QWidget* parent=0);
private slots:
    void rotate();
private:
    int myAngle;
    QPixmap myPixmap;
    QTimer* myTimer;
};

class ZLQtWaitDialog : public QDialog {

public:
                ZLQtWaitDialog(const std::string &message, QWidget* parent=0);
		~ZLQtWaitDialog();
		void paintEvent(QPaintEvent *event);
                void resizeEvent(QResizeEvent *event);

private:
		QLayout* myLayout;
		QLabel* myLabel;
                LoadingIcon* myLoadingIcon;
                //QProgressBar* myProgressBar;

friend class ZLQtProgressDialog;
};


class ZLQtProgressDialog : public ZLProgressDialog {

public:
		ZLQtProgressDialog(const ZLResourceKey &key);

                void run(ZLRunnable &runnable);
                void run(TreeActionListener* listener); // for wait dialog while loading book

		void setMessage(const std::string &message);

private:
		ZLQtWaitDialog* myDialog;

};


class ThreadRunnable : public QThread  {
 public:
		ThreadRunnable(ZLRunnable* runnable): QThread(0), myRunnable(runnable) { }
		void run() {
			myRunnable->run();
		}
private:
	ZLRunnable* myRunnable;

};



#endif // __ZLQTPROGRESSDIALOG_H__

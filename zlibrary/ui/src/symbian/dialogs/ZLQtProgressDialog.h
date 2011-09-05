#ifndef __ZLQTPROGRESSDIALOG_H__
#define __ZLQTPROGRESSDIALOG_H__

#include <QApplication>
#include <QDialog>
#include <QWidget>

#include <QLabel>
#include <QProgressBar>
#include <QtGui/QPaintEvent>
#include <QThread>


#include <ZLProgressDialog.h>
#include <ZLRunnable.h>


class ZLQtWaitDialog : public QDialog {

public:
		ZLQtWaitDialog(const std::string &message);
		~ZLQtWaitDialog();
		void paintEvent(QPaintEvent *event);

private:
		QLayout* myLayout;
		QLabel* myLabel;
		QProgressBar* myProgressBar;

friend class ZLQtProgressDialog;
};


class ZLQtProgressDialog : public ZLProgressDialog {

public:
		ZLQtProgressDialog(const ZLResourceKey &key);

private:
		void run(ZLRunnable &runnable);
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

#ifndef __ZLQTOPTIONSDIALOG_H__
#define __ZLQTOPTIONSDIALOG_H__


#include <QtGui/QWidget>
#include <QtGui/QDialog>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QScrollArea>
#include <QtGui/QPaintEvent>
#include <QtGui/QStackedWidget>
#include <QtGui/QListWidget>

#include "ZLQtDialogContent.h"

#include <string>

#include <ZLOptionsDialog.h>

//TODO rename it
class MyTabWidget : public QWidget {
	Q_OBJECT;
public:
	MyTabWidget(QWidget* parent=0);
	void addTab(QWidget *widget, const QString &label);
	int currentIndex() const;
	//TODO make slots
	void setCurrentWidget(QWidget *widget);
	void setCurrentIndex(int index);
public slots:
	//TODO rename
	void clicked(const QModelIndex &index);
private:
	QStackedWidget* myStackedWidget;
	QScrollArea* myScrollArea;
	QListWidget* myMenuWidget;
};

class ZLQtOptionsDialog : public QDialog, public ZLOptionsDialog {
	Q_OBJECT

public:
	ZLQtOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction);
	ZLDialogContent &createTab(const ZLResourceKey &key);	

public:
	bool run();

protected:
	const std::string &selectedTabKey() const;
	void selectTab(const ZLResourceKey &key);
	bool runInternal();

	void setSize(int width, int height) { QDialog::resize(width, height); }
	int width() const { return QDialog::width(); }
	int height() const { return QDialog::height(); }

protected:
		void setFullScreenWithSoftButtons();

private Q_SLOTS:
	void apply();

private:
	//TODO rename it
	MyTabWidget *myTabWidget;
	//ZLQtDialogContent* myContent;
	//QWidget* myWidget;
	//ZLResourceKey myKey;
	//TODO may be place it to myTabWidget
	//QScrollArea* myScrollArea;

};
#endif /* __ZLQTOPTIONSDIALOG_H__ */

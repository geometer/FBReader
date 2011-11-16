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

class TabMenuWidget : public QWidget {
	Q_OBJECT;

public:
	TabMenuWidget(QWidget* parent=0);
	void addItem(QWidget *widget, const QString &label);

public:
	enum ShowStatus {
		MENU,
		TAB,
	};
	ShowStatus getStatus() const;
	void setStatus(ShowStatus status);

public slots:
	void menuItemClicked(const QModelIndex &index);

private:
	QStackedWidget* myStackedWidget;
	QListWidget* myMenuWidget;
};

class ZLQtOptionsDialog : public QDialog, public ZLOptionsDialog {
	Q_OBJECT

public:
        ZLQtOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction, QWidget* parent);
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
	void back();

private:
	TabMenuWidget *myTabMenuWidget;
	std::string myEmptyString;

};
#endif /* __ZLQTOPTIONSDIALOG_H__ */

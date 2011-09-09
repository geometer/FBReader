#ifndef __ZLQTOPTIONSDIALOG_H__
#define __ZLQTOPTIONSDIALOG_H__


#include <QtGui/QWidget>
#include <QtGui/QDialog>
#include <QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QScrollArea>
#include <QtGui/QPaintEvent>

#include "ZLQtDialogContent.h"

#include <string>

#include <ZLOptionsDialog.h>

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
	ZLQtDialogContent* myContent;
	QWidget* myWidget;
	ZLResourceKey myKey;
	QScrollArea* myScrollArea;

};
#endif /* __ZLQTOPTIONSDIALOG_H__ */

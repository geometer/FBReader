#ifndef __ZLQTDIALOG_H__
#define __ZLQTDIALOG_H__

#include <QtGui/QDialog>
#include <QtGui/QLayout>

#include <ZLDialog.h>

class ZLQtDialog : public QDialog, public ZLDialog {

public:
	ZLQtDialog(const ZLResource &resource);
	~ZLQtDialog();

	void addButton(const ZLResourceKey &key, bool accept);
	bool run();
        bool runFullscreen();

private:
};

#endif /* __ZLQTDIALOG_H__ */

#ifndef __ZLQTDIALOGCONTENT_H__
#define __ZLQTDIALOGCONTENT_H__

#include "../../../../core/src/desktop/dialogs/ZLDesktopOptionsDialog.h"

#include <QtCore/QString>

class QWidget;
class QVBoxLayout;

class ZLQtDialogContent : public ZLDialogContent {

public:
	ZLQtDialogContent(QWidget *parent, const ZLResource &resource);
	~ZLQtDialogContent();

	void addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option);
	void addOptions(const std::string &name0, const std::string &tooltip0, ZLOptionEntry *option0,
									const std::string &name1, const std::string &tooltip1, ZLOptionEntry *option1);

	void addItem(QWidget *widget);

	void close();

	QWidget *widget();

private:
	void createViewByEntry(const std::string &name, const std::string &tooltip, ZLOptionEntry *option);

private:
	QWidget *myWidget;
	QVBoxLayout *myLayout;
	bool isFirstWidget;
};

#endif /* __ZLQTDIALOGCONTENT_H__ */

#ifndef __ZLQTDIALOGCONTENT_H__
#define __ZLQTDIALOGCONTENT_H__

#include <ZLDialogContent.h>

#include <QtCore/QString>
#include <QtGui/QScrollArea>

class QWidget;
class QVBoxLayout;

class ZLQtDialogContent : public QObject, public ZLDialogContent {

public:
        ZLQtDialogContent(const ZLResource &resource);
	~ZLQtDialogContent();

	void addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option);
	void addOptions(const std::string &name0, const std::string &tooltip0, ZLOptionEntry *option0,
									const std::string &name1, const std::string &tooltip1, ZLOptionEntry *option1);

	void addItem(QWidget *widget);
        void addItem(int index, QWidget *widget);

	void close();

	QWidget *widget();

private:
	void createViewByEntry(const std::string &name, const std::string &tooltip, ZLOptionEntry *option);

private:
        bool eventFilter(QObject *obj, QEvent *event);

private:
        QScrollArea* myScrollArea;
	QWidget *myWidget;
	QVBoxLayout *myLayout;

        //isFirstWidget were used to set focus on first widget, this is necessary only for
        //phones without touch screen
        //bool isFirstWidget;
};

#endif /* __ZLQTDIALOGCONTENT_H__ */

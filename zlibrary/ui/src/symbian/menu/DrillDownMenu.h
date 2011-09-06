#ifndef __DRILLDOWNMENU_H__
#define __DRILLDOWNMENU_H__

#include <string>

#include <QtGui/QApplication>
#include <QVBoxLayout>
#include <QAction>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QList>
#include <QLabel>
#include <QtGui/QPaintEvent>

#include <ZLApplication.h>

class DrillDownMenuItem : public QListWidgetItem {
public:
    explicit DrillDownMenuItem(const QString &text, ZLApplication::Action* action, QListWidget *view = 0, int type = Type);
    explicit DrillDownMenuItem(const QIcon &icon, const QString &text, ZLApplication::Action* action, QListWidget *view = 0, int type = Type);
    void run();
private:
	shared_ptr<ZLApplication::Action> myAction;
};

class DrillDownMenu: public QListWidget {
    Q_OBJECT
public:
    explicit DrillDownMenu(QWidget *parent = 0);
    void addItem(const std::string &text, ZLApplication::Action* action=0);
    void addItem(const QIcon& icon, const std::string &text, ZLApplication::Action* action=0);
public:
	void setMessage(const std::string& message);
	std::string getMessage() const;
private slots:
    void run(QListWidgetItem* item);
private:
	QString myMessage;
};

class DrillDownMenuDialog : public QDialog {
    Q_OBJECT
public:
    DrillDownMenuDialog(QWidget* parent=0);
    void showDrillDownMenu(DrillDownMenu* menu);
	void showMessage(std::string message);
public:
	bool run();
public:
	void paintEvent(QPaintEvent *event);
public slots:
    void back();
private: // exec() should not be executed directly, use run()
	int exec();
private:
    void setCurrentMenu(DrillDownMenu* menu);
private:
    QStackedWidget* myStackedWidget;
	QLabel* myLabel;
    QList<DrillDownMenu*> myHistory;
};

#endif /* __DRILLDOWNMENU_H__ */

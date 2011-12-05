#ifndef __DRILLDOWNMENU_H__
#define __DRILLDOWNMENU_H__

#include <string>

#include <QtCore/QList>
#include <QtGui/QApplication>
#include <QtGui/QVBoxLayout>
#include <QtGui/QAction>
#include <QtGui/QPushButton>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMessageBox>
#include <QtGui/QDialog>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QLabel>
#include <QtGui/QPaintEvent>

#include <ZLApplication.h>

class NiceSizeListWidgetItem : public QListWidgetItem {
public:
    explicit NiceSizeListWidgetItem(const QString &text, QListWidget *view = 0, int type = Type);
    explicit NiceSizeListWidgetItem(const QIcon &icon, const QString &text,QListWidget *view = 0, int type = Type);
    QVariant data (int role) const;
};

namespace MenuItemParameters {
    //TODO replace MenuItemsParameters implementation to separate file
    int getMenuDialogHeight();
    int getTapBottomZoneSize();
    QSize getItemSize();
    QSize getSmallItemSize();
    QSize getImageSize();
    QSize getImageZoneSize();
    QFont getFont();
    QFont getSubtitleFont();
    QSize getMaximumBookCoverSize(QSize window);
    int MMtoPixels(int mm);
}

class DrillDownMenuItem : public NiceSizeListWidgetItem {
public:
    explicit DrillDownMenuItem(const QString &text, ZLApplication::Action* action, QListWidget *view = 0, int type = Type);
    explicit DrillDownMenuItem(const QIcon &icon, const QString &text, ZLApplication::Action* action, QListWidget *view = 0, int type = Type);
    //TODO support for isVisible method of Action
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
    void addItem(DrillDownMenuItem* item);
public:
	void setMessage(const std::string& message);
	std::string getMessage() const;
signals:
    void itemRunned(QListWidgetItem* item);
private slots:
    void run(QListWidgetItem* item);
private:
	QString myMessage;
};

class DrillDownMenuDialog : public QDialog {
    Q_OBJECT
public:
    DrillDownMenuDialog(bool closeAfterRun, QWidget* parent=0);
    //TODO rename next method (because show doesn't mean show)
    void showDrillDownMenu(DrillDownMenu* menu); //actually, it doesn't show menu, it set it as first
    void showMessage(std::string message);
public:
	bool run();
	bool runNoFullScreen();
public:
	void paintEvent(QPaintEvent *event);
public slots:
    void back();
    void onItemRunned(QListWidgetItem* item);
signals:
    void itemRunBefore(QListWidgetItem* item);
    void itemRunAfter(QListWidgetItem* item);
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

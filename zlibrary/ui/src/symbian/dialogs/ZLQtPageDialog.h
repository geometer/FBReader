#ifndef __ZLQTPAGEDIALOG_H__
#define __ZLQTPAGEDIALOG_H__

#include <QtGui/QDialog>
#include <QtGui/QAction>

#include <shared_ptr.h>
#include <ZLDialogContent.h>
#include <ZLApplication.h>
#include <ZLTreePageNode.h>

class PageMenu : public QObject {
    Q_OBJECT



public:
    PageMenu(const ZLTreePageNode& pageNode);
    void setWidget(QWidget* widget); //widget where action will be placed

public:
    static std::vector<shared_ptr<ZLTreeAction> > filterSensibleActions(const std::vector<shared_ptr<ZLTreeAction> > & actions);

signals:
    void drillDownMenuClose();

public slots:
    void activate();
    void finished(int);

private:
    void setActionButton();

private:
    QAction* myAction;
    const ZLTreePageNode& myPageNode;
    QWidget* myWidget;
};


class ZLQtPageDialog : public QDialog {
    Q_OBJECT

public:
    ZLQtPageDialog(const ZLTreePageNode& pageNode, QWidget* parent = 0);

public:
    bool run();

private:
    shared_ptr<ZLDialogContent> myContent;
    const ZLTreePageNode& myPageNode;
    PageMenu myPageMenu;
};

class WrapperAction : public ZLApplication::Action {
public:
    WrapperAction(shared_ptr<ZLTreeAction> runnable);
    bool isVisible() const;
    void run();

private:
    shared_ptr<ZLTreeAction> myRunnable;
};

#endif // __ZLQTPAGEDIALOG_H__

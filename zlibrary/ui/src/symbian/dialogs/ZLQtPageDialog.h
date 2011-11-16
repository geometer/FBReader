#ifndef __ZLQTPAGEDIALOG_H__
#define __ZLQTPAGEDIALOG_H__

#include <QtGui/QDialog>
#include <QtGui/QAction>

#include <shared_ptr.h>
#include <ZLDialogContent.h>
#include <ZLApplication.h>
#include <ZLTreePageNode.h>

#include "ZLQtTreeDialog.h"

class ZLQtPageDialog : public QDialog {
    Q_OBJECT

public:
    ZLQtPageDialog(const ZLTreePageNode& pageNode, QWidget* parent = 0);
    ~ZLQtPageDialog();

public:
    bool run();

private:
    shared_ptr<ZLDialogContent> myContent;
    const ZLTreePageNode& myPageNode;
    TreeNodeActionsMenu* myPageMenu;
};

#endif // __ZLQTPAGEDIALOG_H__

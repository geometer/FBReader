#ifndef __ZLQTPAGEDIALOG_H__
#define __ZLQTPAGEDIALOG_H__

#include <QtGui/QDialog>
#include <QtGui/QAction>

#include <shared_ptr.h>
#include <ZLDialogContent.h>
#include <ZLApplication.h>
#include <ZLTreePageNode.h>

#include "ZLQtTreeDialog.h"
#include "ZLQtOptionView.h"

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

class NodePictureWidget : public PictureWidget {
    Q_OBJECT
public:
    NodePictureWidget(const ZLTreePageNode& pageNode, QWidget* parent = 0);
public slots:
    void refresh();
private:
    ImageProvider* myImageProvider;
    const ZLTreePageNode& myPageNode;
};

#endif // __ZLQTPAGEDIALOG_H__

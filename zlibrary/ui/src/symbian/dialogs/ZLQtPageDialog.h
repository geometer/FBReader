#ifndef __ZLQTPAGEDIALOG_H__
#define __ZLQTPAGEDIALOG_H__

#include <QtGui/QDialog>

#include <shared_ptr.h>
#include <ZLDialogContent.h>


class ZLQtPageDialog : public QDialog {
    Q_OBJECT

public:
    ZLQtPageDialog(shared_ptr<ZLDialogContent> content);

public:
    bool run();

private:
    shared_ptr<ZLDialogContent> myContent;
};

#endif // __ZLQTPAGEDIALOG_H__

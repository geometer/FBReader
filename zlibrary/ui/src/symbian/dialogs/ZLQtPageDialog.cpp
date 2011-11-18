#include <QtGui/QApplication>
#include <QtGui/QVBoxLayout>
#include <QtGui/QAction>
#include <QtGui/QPushButton>
#include <QtCore/QDebug>

#include <ZLDialogManager.h>

#include "ZLQtPageDialog.h"
#include "ZLQtDialogContent.h"

#include "../menu/DrillDownMenu.h"
#include "../menu/ActionButtons.h"

#include "ZLQtTreeDialog.h"

ZLQtPageDialog::ZLQtPageDialog(const ZLTreePageNode& pageNode, QWidget* parent) : QDialog(parent), myContent(pageNode.content()), myPageNode(pageNode) {

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(static_cast<ZLQtDialogContent&>(*myContent).widget());

    const std::string& back = ZLDialogManager::Instance().buttonName(ZLResourceKey("back"));
    QAction* action = new QAction(QString::fromStdString(back),this);
#ifdef __SYMBIAN__
    action->setSoftKeyRole( QAction::NegativeSoftKey );
#endif
    connect(action, SIGNAL(triggered()), this, SLOT(accept()));
    addAction( action );
#ifndef __SYMBIAN__
    QPushButton* backButton = new ButtonAction(action);
    layout->addWidget(backButton);
#endif

    QAction* pageMenuAction = new QAction(this);
#ifdef __SYMBIAN__
    pageMenuAction->setSoftKeyRole( QAction::PositiveSoftKey );
#endif
    myPageMenu = new TreeNodeActionsMenu(pageMenuAction, this);
    myPageMenu->setTreeNode(&pageNode);
    addAction(pageMenuAction);
    connect(pageMenuAction, SIGNAL(triggered()), myPageMenu, SLOT(activate()));
    connect(this,SIGNAL(finished(int)), myPageMenu, SLOT(onFinish(int)));
#ifndef __SYMBIAN__
        QPushButton* menuButton = new ButtonAction(pageMenuAction);
        layout->addWidget(menuButton);
#endif

    this->setLayout(layout);
}

ZLQtPageDialog::~ZLQtPageDialog() {
    //TODO reimplement the way to use inner widget in ZLQtDialogContent
    //(if not set parent to 0, it will crash, cause inner widget will be deleted)
    //static_cast<ZLQtDialogContent&>(*myContent).widget()->setParent(0);
}

bool ZLQtPageDialog::run() {
#ifdef __SYMBIAN__
    setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);
    setWindowState(Qt::WindowFullScreen);
#else
    setFixedSize(400, 600);
#endif
    NodePictureWidget* picture = new NodePictureWidget(myPageNode);
    ZLQtDialogContent* content = static_cast<ZLQtDialogContent*>(&*myContent);
    content->addItem(0, picture);
    content->close();
    return exec() == QDialog::Accepted;
}

NodePictureWidget::NodePictureWidget(const ZLTreePageNode& pageNode, QWidget* parent) : PictureWidget(QPixmap(), parent), myPageNode(pageNode) {
    //TODO implement a way for caching cover, if it has been loaded yet via network manager
    myImageProvider = new ImageProvider(ImageProvider::FULL, this);
    connect(myImageProvider, SIGNAL(cacheUpdated()), this, SLOT(refresh()));
    myPicture = myImageProvider->getImageForNode(&myPageNode);
}

void NodePictureWidget::refresh() {
    //qDebug() << Q_FUNC_INFO;
    if (ImageProvider::generateUrl(&myPageNode).scheme() == QLatin1String(ZLTreeTitledNode::LOCALFILE_SCHEME.c_str())) {
        return;
    }
    myPicture = myImageProvider->getImageForNode(&myPageNode);
    //qDebug() << Q_FUNC_INFO << myPicture.size();
    updateGeometry();
}

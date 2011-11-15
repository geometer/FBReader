#include <QtCore/QDebug>

#include "../dialogs/ScrollerManager.h"

#include "../dialogs/ZLQtUtil.h"

#include "DrillDownMenu.h"

DrillDownMenuDialog::DrillDownMenuDialog(QWidget* parent) : QDialog(parent) {
	QVBoxLayout* layout = new QVBoxLayout(this);
	myStackedWidget = new QStackedWidget;
	myLabel = new QLabel;
	myLabel->setWordWrap(true);

	layout->addWidget(myLabel);
        layout->addWidget(myStackedWidget);

	const ZLResource& back = ZLResource::resource("dialog")["button"]["back"];
	QAction* action = new QAction(QString::fromStdString(back.value()),this);
#ifdef __SYMBIAN__
        action->setSoftKeyRole( QAction::NegativeSoftKey );
#endif
        connect(action, SIGNAL(triggered()), this, SLOT(back()));
        addAction( action );

#ifndef __SYMBIAN__
        QPushButton* button = new QPushButton(QString::fromStdString(back.value()));
        connect(button, SIGNAL(clicked()), this, SLOT(back()));
        layout->addWidget(button);
#endif
}

void DrillDownMenuDialog::paintEvent(QPaintEvent *event) {
    QDialog::paintEvent(event);
}

bool DrillDownMenuDialog::run() {
#ifdef __SYMBIAN__
	setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);
#endif
	setWindowState(Qt::WindowFullScreen);
	return exec() == QDialog::Accepted;
}

bool DrillDownMenuDialog::runNoFullScreen() {
	return exec() == QDialog::Accepted;
}

int DrillDownMenuDialog::exec() {
	return QDialog::exec();
}

void DrillDownMenuDialog::showMessage(std::string message) {
	myLabel->setText(::qtString(message));
	myLabel->setVisible(message.empty() == false);
}

void DrillDownMenuDialog::showDrillDownMenu(DrillDownMenu* menu) {
    if (myStackedWidget->indexOf(menu) == -1) {
        myStackedWidget->addWidget(menu);
    }
    setCurrentMenu(menu);
    myHistory.append(menu);
}

 void DrillDownMenuDialog::setCurrentMenu(DrillDownMenu* menu) {
    ScrollerManager::setScroll(menu);
    myStackedWidget->setCurrentWidget(menu);
#ifdef __SYMBIAN__
    menu->setEditFocus(true); // for phones with keyboard: need to activate for single-click
#endif
	showMessage(menu->getMessage());
    myStackedWidget->update();
 }

void DrillDownMenuDialog::back() {
    if (myHistory.isEmpty()) {
        close();
        return;
    }
    myHistory.pop_back();
    if (myHistory.isEmpty()) {
		reject();
        return;
    }
    setCurrentMenu(myHistory.last());
}

void DrillDownMenu::addItem(const std::string &text, ZLApplication::Action* action) {
    DrillDownMenuItem* item = new DrillDownMenuItem( ::qtString(text), action, this);
    QListWidget::addItem( item );
    //this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);
}

void DrillDownMenu::addItem(const QIcon& icon, const std::string &text, ZLApplication::Action* action) {
    DrillDownMenuItem* item = new DrillDownMenuItem(icon, ::qtString(text), action, this);
    QListWidget::addItem( item );
//    this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);
}

void DrillDownMenu::addItem(DrillDownMenuItem* item) {
	QListWidget::addItem( item );
}

DrillDownMenu::DrillDownMenu(QWidget *parent) : QListWidget(parent) {
    connect(this, SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(run(QListWidgetItem*)), Qt::QueuedConnection);
	//connect(this, SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(run(QListWidgetItem*)));
    // don't close DrillDownMenu after options dialog was closed:
    //connect(this, SIGNAL(itemActivated(QListWidgetItem*)),parent,SLOT(close()));

}

void DrillDownMenu::run(QListWidgetItem* item) {
    ((DrillDownMenuItem*)item)->run();
}

void DrillDownMenu::setMessage(const std::string& message) {
	myMessage = ::qtString(message);
}

std::string DrillDownMenu::getMessage() const {
	return ::stdString(myMessage);
}

DrillDownMenuItem::DrillDownMenuItem(const QIcon &icon, const QString &text, ZLApplication::Action* action, QListWidget *view, int type) :
    NiceSizeListWidgetItem(icon,text,view,type), myAction(action) {
}

DrillDownMenuItem::DrillDownMenuItem(const QString &text, ZLApplication::Action* action, QListWidget *view, int type) :
    NiceSizeListWidgetItem(text,view,type), myAction(action) {
}

void DrillDownMenuItem::run() {
    if (myAction == 0) {
        QMessageBox::information(0,"","Sorry!\nIt's not implemented yet", QMessageBox::Ok);
        return;
    }
    myAction->checkAndRun();
}

NiceSizeListWidgetItem::NiceSizeListWidgetItem(const QString &text, QListWidget *view , int type)
    : QListWidgetItem(text,view,type) { }

NiceSizeListWidgetItem::NiceSizeListWidgetItem(const QIcon &icon, const QString &text,QListWidget *view, int type)
    : QListWidgetItem(icon,text,view,type) {}


QVariant NiceSizeListWidgetItem::data (int role) const {
    if (role == Qt::SizeHintRole){
        //qDebug() << Q_FUNC_INFO << role << index;
        return MenuItemParameters::getItemSize();
    } else if (role == Qt::FontRole) {
        return MenuItemParameters::getFont();
    }
    return QListWidgetItem::data(role);
}

int MenuItemParameters::getMenuDialogHeight() {
    return qApp->desktop()->availableGeometry().height()*2/3;
}

int MenuItemParameters::getTapBottomZoneSize() {
    static const int TAP_BOTTOM_SIZE_MM = 10;
    return MMtoPixels(TAP_BOTTOM_SIZE_MM);
}

int MenuItemParameters::MMtoPixels(int mm) {
    const int coef = qApp->desktop()->height() / qApp->desktop()->heightMM();
    //qDebug() << "desktop height" << qApp->desktop()->height() << "in mm:" << qApp->desktop()->heightMM();
    return coef*mm;
}

QSize MenuItemParameters::getItemSize() {
#ifdef 	__SYMBIAN__
    static const int SIZE_FOR_FINGERS_MM = 12; // in millimetres
#else
    static const int SIZE_FOR_FINGERS_MM = 25; // in millimetres
#endif
    static const int MAX_PART_OF_SCREEN = 5;  // if 1/5 of screen if less than SIZE_FOR_FINGERS_MM,
                                           // set size as 1/5 of screen
    QRect rect = qApp->desktop()->availableGeometry();
    int height = std::min(rect.height()/MAX_PART_OF_SCREEN, MMtoPixels(SIZE_FOR_FINGERS_MM));
    return QSize(rect.width(), height);
}

QSize MenuItemParameters::getImageSize() {
    static const qreal COEF_W = 0.7;
    static const qreal COEF_H = 0.5;
    int height = getImageZoneSize().height();
    int width = getImageZoneSize().width();
    width  *= COEF_W;
    height *= COEF_H;
    return QSize(width,height);
}

QSize MenuItemParameters::getImageZoneSize() {
    static const qreal COEF_W = 0.75;
    int height = getItemSize().height();
    int width = height;
    width *= COEF_W;
    return QSize(width,height);
}

QFont MenuItemParameters::getFont() {
    static const qreal COEF = 1.15;
    QFont font = qApp->desktop()->font();
    font.setPointSizeF( qreal(font.pointSizeF()*COEF) );
    font.setBold(true);
    return font;
}

QFont MenuItemParameters::getSubtitleFont() {
    static const qreal COEF = 0.85;
    QFont font = qApp->desktop()->font();
    font.setPointSizeF( qreal(font.pointSizeF()*COEF) );
    font.setBold(false);
    return font;
}

QSize MenuItemParameters::getMaximumBookCoverSize(QSize window) {
    static const qreal COEF = 0.5;
    QSize size = window;
    return QSize(size.width(), size.height()*COEF);
}

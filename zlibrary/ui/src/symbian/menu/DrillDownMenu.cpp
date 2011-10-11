#include <QtCore/QDebug>

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

QSize DrillDownMenu::generateSizeHint() const{
    QRect rect = qApp->desktop()->availableGeometry();
    int height = std::min(rect.height()/5, 40);
    return QSize(rect.width(), height);
}

void DrillDownMenu::addItem(const std::string &text, ZLApplication::Action* action) {
    DrillDownMenuItem* item = new DrillDownMenuItem( ::qtString(text), action, this);
    QListWidget::addItem( item );
    //item->setSizeHint(generateSizeHint());
    //this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);
}

void DrillDownMenu::addItem(const QIcon& icon, const std::string &text, ZLApplication::Action* action) {
    DrillDownMenuItem* item = new DrillDownMenuItem(icon, ::qtString(text), action, this);
    QListWidget::addItem( item );
 //   item->setSizeHint(generateSizeHint());
//    this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum);
}

void DrillDownMenu::addItem(DrillDownMenuItem* item) {
	QListWidget::addItem( item );
}

DrillDownMenu::DrillDownMenu(QWidget *parent) : QListWidget(parent) {
    connect(this, SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(run(QListWidgetItem*)));
	//connect(this, SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(run(QListWidgetItem*)));
    // don't close DrillDownMenu after options dialog was closed:
    //connect(this, SIGNAL(itemActivated(QListWidgetItem*)),parent,SLOT(close()));
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
        return MenuItemParameters::getSize();
    } else if (role == Qt::FontRole) {
        return MenuItemParameters::getFont();
    }
    return QListWidgetItem::data(role);
}

QSize MenuItemParameters::getSize() {
    static const int SIZE_FOR_FINGERS_MM = 8; // in millimetres
    static const int MAX_PART_OF_SCREEN = 5;  // if 1/5 of screen if less than sizeForFingersMM,
                                           // set size as 1/5 of screen
    QRect rect = qApp->desktop()->availableGeometry();
    int coef = qApp->desktop()->height() / qApp->desktop()->heightMM();;
    int height = std::min(rect.height()/MAX_PART_OF_SCREEN, coef*SIZE_FOR_FINGERS_MM);
    return QSize(rect.width(), height);
}

QFont MenuItemParameters::getFont() {
    static const int FONT_SIZE_IN_POINTS = 8;
    QFont font = qApp->desktop()->font();
    //qDebug() << "font default size" <<  font.pointSize();
    font.setPointSize(FONT_SIZE_IN_POINTS);
    //font.setStyleStrategy(QFont::PreferDevice);
    font.setBold(true);
    return font;
}

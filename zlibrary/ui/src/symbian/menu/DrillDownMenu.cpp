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
    action->setSoftKeyRole( QAction::NegativeSoftKey );
    connect(action, SIGNAL(triggered()), this, SLOT(back()));
    addAction( action );
}

void DrillDownMenuDialog::paintEvent(QPaintEvent *event) {
    QDialog::paintEvent(event);
}

bool DrillDownMenuDialog::run() {
	setWindowFlags(windowFlags() | Qt::WindowSoftkeysVisibleHint);
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

void DrillDownMenu::addItem(const std::string &text, ZLApplication::Action* action) {
    DrillDownMenuItem* item = new DrillDownMenuItem( ::qtString(text), action, this);
    QListWidget::addItem( item );

}

void DrillDownMenu::addItem(const QIcon& icon, const std::string &text, ZLApplication::Action* action) {
    DrillDownMenuItem* item = new DrillDownMenuItem(icon, ::qtString(text), action, this);
    QListWidget::addItem( item );
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
    QListWidgetItem(icon,text,view,type), myAction(action) {
    QFont font(this->font());
    font.setBold(true);
	setFont(font);
}

DrillDownMenuItem::DrillDownMenuItem(const QString &text, ZLApplication::Action* action, QListWidget *view, int type) :
    QListWidgetItem(text,view,type), myAction(action) {
    QFont font(this->font());
    font.setBold(true);
	setFont(font);
}

void DrillDownMenuItem::run() {
    if (myAction == 0) {
        QMessageBox::information(0,"","Sorry!\nIt's not implemented yet", QMessageBox::Ok);
        return;
    }
    myAction->checkAndRun();
}

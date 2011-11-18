#include "ActionButtons.h"

ButtonAction::ButtonAction(QWidget *parent) : QPushButton(parent), myAction(0) {

}

ButtonAction::ButtonAction(const QString &text, QWidget *parent) : QPushButton(text,parent), myAction(0) {

}

ButtonAction::ButtonAction(QAction* action, QWidget* parent): QPushButton(parent), myAction(action) {
    this->setText(myAction->text());
    connect(action, SIGNAL(changed()), this, SLOT(onActionChange()));
    connect(this, SIGNAL(clicked()), action, SLOT(trigger()));
}

void ButtonAction::onActionChange() {
    this->setText(myAction->text());
}

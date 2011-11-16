#ifndef __ACTIONBUTTONS_H__
#define __ACTIONBUTTONS_H__

#include <QtGui/QAction>
#include <QtGui/QPushButton>

class ButtonAction : public QPushButton {
    Q_OBJECT
public:
    explicit ButtonAction(QWidget *parent=0);
    explicit ButtonAction(const QString &text, QWidget *parent=0);
    explicit ButtonAction(QAction* action, QWidget* parent=0);
public slots:
    void onActionChange();
private:
    QAction* myAction;
};

#endif /* __ACTIONBUTTONS_H__ */

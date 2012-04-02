/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __ZLQTOPTIONSDIALOG_H__
#define __ZLQTOPTIONSDIALOG_H__

#include <QtGui/QWidget>
//#include <QtGui/QTabWidget>

#include <QtGui/QDialog>

#include "../../../../core/src/desktop/dialogs/ZLDesktopOptionsDialog.h"


//NEW TAB CLASS BEGIN==================================================

#include <QtGui/QWidget>
#include <QtGui/QListWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QStringListModel>
#include <QtGui/QStackedWidget>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtCore/QDebug>
#include <QtCore/QString>

class NewTabWidget : public QWidget
{
    Q_OBJECT
public:
    void setCurrentWidget(QWidget *widget);
    int currentIndex() const;
    int addTab(QWidget *widget, const QString &tabLabel);
    void addStringToList(QString newString);
    explicit NewTabWidget(QWidget *parent = 0);
private:
    QListWidget    *myListWidget;
    QStackedWidget *myStackedWidget;
signals:

public slots:
    void listViewItemChanged(QListWidgetItem* current, QListWidgetItem* prev);

};

//NEW TAB CLASS END====================================================






class ZLQtOptionsDialog : public QDialog, public ZLDesktopOptionsDialog {
	Q_OBJECT

public:
	ZLQtOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction, bool showApplyButton);
	ZLDialogContent &createTab(const ZLResourceKey &key);

protected:
	const std::string &selectedTabKey() const;
	void selectTab(const ZLResourceKey &key);
	bool runInternal();

	void setSize(int width, int height) { QDialog::resize(width, height); }
	int width() const { return QDialog::width(); }
	int height() const { return QDialog::height(); }

private Q_SLOTS:
	void apply();

private:
    NewTabWidget *myTabWidget;
};

#endif /* __ZLQTOPTIONSDIALOG_H__ */

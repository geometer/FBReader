/*
 * Copyright (C) 2004-2011 Geometer Plus <contact@geometerplus.com>
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
#include <QtGui/QTabWidget>
#include <QtGui/QDialog>

#include "../../../../core/src/dialogs/ZLOptionsDialog.h"

class ZLQmlOptionsDialog : public QObject, public ZLOptionsDialog {
	Q_OBJECT
	Q_PROPERTY(QObjectList sections READ sections NOTIFY sectionsChanged)
	Q_PROPERTY(QString okButtonText READ applyButtonText CONSTANT)
	Q_PROPERTY(QString applyButtonText READ applyButtonText CONSTANT)
	Q_PROPERTY(QString cancelButtonText READ cancelButtonText CONSTANT)
public:
	ZLQmlOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction, bool showApplyButton);
	ZLDialogContent &createTab(const ZLResourceKey &key);

protected:
	QObjectList sections() const;
	const std::string &selectedTabKey() const;
	void selectTab(const ZLResourceKey &key);
	bool runInternal();
	
	QString okButtonText() const;
	QString applyButtonText() const;
	QString cancelButtonText() const;
	
	Q_INVOKABLE void accept();
	Q_INVOKABLE void reject();
	
Q_SIGNALS:
	void sectionsChanged(const QObjectList &sections);
	void finished();

private:
	QObjectList mySections;
	QString myOkButtonText;
	QString myApplyButtonText;
	QString myCancelButtonText;
	std::string myEmptyString;
	bool myShowApplyButton;
	bool myResult;
};

#endif /* __ZLQTOPTIONSDIALOG_H__ */

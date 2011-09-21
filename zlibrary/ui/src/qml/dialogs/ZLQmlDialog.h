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

#ifndef __ZLQTDIALOG_H__
#define __ZLQTDIALOG_H__

#include <QtCore/QObject>
#include <QtCore/QStringList>

#include <ZLDialog.h>

class ZLQmlDialog : public QObject, public ZLDialog {
	Q_OBJECT
	Q_PROPERTY(QString title READ title CONSTANT)
	Q_PROPERTY(QStringList buttonNames READ buttonNames NOTIFY buttonNamesChanged)
	Q_PROPERTY(QStringList acceptButtons READ acceptButtons NOTIFY acceptButtonsChanged)
	Q_PROPERTY(QObject *content READ content CONSTANT)
public:
	ZLQmlDialog(const ZLResource &resource);
	~ZLQmlDialog();
	
	QString title() const;
	QStringList buttonNames() const;
	QStringList acceptButtons() const;
	QObject *content() const;
	
	void addButton(const ZLResourceKey &key, bool accept);
	bool run();

	Q_INVOKABLE void accept();
	Q_INVOKABLE void reject();
	
Q_SIGNALS:
	void finished();
	void buttonNamesChanged(const QStringList &buttonNames);
	void acceptButtonsChanged(const QStringList &acceptButtons);

private:
	QString myTitle;
	QStringList myButtonNames;
	QStringList myAcceptButtons;
	bool myResult;
};

#endif /* __ZLQTDIALOG_H__ */

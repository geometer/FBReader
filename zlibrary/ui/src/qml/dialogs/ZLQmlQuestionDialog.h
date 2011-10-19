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

#ifndef ZLQMLQUESTIONDIALOG_H
#define ZLQMLQUESTIONDIALOG_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

class ZLQmlQuestionDialog : public QObject
{
    Q_OBJECT
	Q_PROPERTY(QString title READ title CONSTANT)
	Q_PROPERTY(QString text READ text CONSTANT)
	Q_PROPERTY(QStringList buttons READ buttons CONSTANT)
public:
    explicit ZLQmlQuestionDialog(const QString &title, const QString &text,
		                         const QString &button0Text, const QString &button1Text, const QString &button2Text);
	
	QString title() const;
	QString text() const;
	QStringList buttons() const;
	Q_INVOKABLE void press(int index);
	int run();

signals:
	void finished();
	
private:
	QString myTitle;
	QString myText;
	QStringList myButtons;
	int myResult;
};

#endif // ZLQMLQUESTIONDIALOG_H

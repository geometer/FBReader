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

#ifndef __ZLQTDIALOGCONTENT_H__
#define __ZLQTDIALOGCONTENT_H__

#include "../../../../core/src/desktop/dialogs/ZLDesktopOptionsDialog.h"
#include <QObject>

class QWidget;
class QGridLayout;

class ZLQmlDialogContent : public QObject, public ZLDialogContent {
	Q_OBJECT
	Q_PROPERTY(QObjectList items READ items NOTIFY itemsChanged)
	Q_PROPERTY(QString title READ title CONSTANT)
public:
	ZLQmlDialogContent(const ZLResource &resource);
	~ZLQmlDialogContent();

	void addOption(const std::string &name, const std::string &tooltip, ZLOptionEntry *option);
	void addOptions(const std::string &name0, const std::string &tooltip0, ZLOptionEntry *option0,
									const std::string &name1, const std::string &tooltip1, ZLOptionEntry *option1);

	QObjectList items() const;
	QString title() const;

private:
	void createViewByEntry(const std::string &name, const std::string &tooltip, ZLOptionEntry *option, int fromColumn, int toColumn);

Q_SIGNALS:
	void itemsChanged(const QObjectList &items);
	
private:
	QObjectList myItems;
	QString myTitle;
	int myRowCounter;
};

#endif /* __ZLQTDIALOGCONTENT_H__ */

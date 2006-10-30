/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#ifndef __QCOMMONDIALOG_H__
#define __QCOMMONDIALOG_H__

#include <qdialog.h>
#include <qlayout.h>
#include <qbuttongroup.h>

#include <ZLDialog.h>

class QCommonDialog : public QDialog, public ZLDialog {
	Q_OBJECT

public:
	QCommonDialog(const std::string &name);
	~QCommonDialog();

	void addButton(const std::string &text);
	bool run();

private:
	QGridLayout *myButtonLayout;
	QButtonGroup *myButtonGroup;
};

#endif /* __QCOMMONDIALOG_H__ */

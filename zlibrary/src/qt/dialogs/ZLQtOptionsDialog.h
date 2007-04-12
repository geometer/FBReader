/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLQTOPTIONSDIALOG_H__
#define __ZLQTOPTIONSDIALOG_H__

#include <vector>

#include <qwidget.h>
#include <qtabdialog.h>

#include "../../desktop/dialogs/ZLDesktopOptionsDialog.h"

class ZLQtOptionsDialog : public QTabDialog, public ZLDesktopOptionsDialog {
	Q_OBJECT

public:
	ZLQtOptionsDialog(const std::string &id, const std::string &caption, shared_ptr<ZLRunnable> applyAction, bool showApplyButton);
	ZLDialogContent &createTab(const std::string &name);

protected:
	const std::string &selectedTabName() const;
	void selectTab(const std::string &name);
	bool run();

	void setSize(int width, int height) { QTabDialog::resize(width, height); }
	int width() const { return QTabDialog::width(); }
	int height() const { return QTabDialog::height(); }

private slots:
	void resizeEvent(QResizeEvent *);
	void apply();

signals:
	void resized(const QSize &size);
};

#endif /* __ZLQTOPTIONSDIALOG_H__ */

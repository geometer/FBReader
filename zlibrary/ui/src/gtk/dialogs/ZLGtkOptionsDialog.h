/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLGTKOPTIONSDIALOG_H__
#define __ZLGTKOPTIONSDIALOG_H__

#include <vector>

#include <gtk/gtkdialog.h>
#include <gtk/gtknotebook.h>

#include "../../../../core/src/desktop/dialogs/ZLDesktopOptionsDialog.h"

class ZLGtkOptionsDialog : public ZLDesktopOptionsDialog {

public:
	ZLGtkOptionsDialog(const ZLResource &resource, shared_ptr<ZLRunnable> applyAction, bool showApplyButton);
	~ZLGtkOptionsDialog();
	ZLDialogContent &createTab(const ZLResourceKey &key);

protected:
	const std::string &selectedTabKey() const;
	void selectTab(const ZLResourceKey &key);
	bool runInternal();

	void setSize(int width, int height);
	int width() const;
	int height() const;

private:
	GtkDialog *myDialog;
	GtkNotebook *myNotebook;
};

#endif /* __ZLGTKOPTIONSDIALOG_H__ */

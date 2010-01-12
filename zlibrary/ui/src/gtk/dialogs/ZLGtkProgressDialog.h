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

#ifndef __ZLGTKPROGRESSDIALOG_H__
#define __ZLGTKPROGRESSDIALOG_H__

#include <string>

#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>

#include <ZLProgressDialog.h>

class ZLGtkProgressDialog : public ZLProgressDialog {

public:
	ZLGtkProgressDialog(GtkWindow *window, const ZLResourceKey &key);

private:
	void run(ZLRunnable &runnable);
	void setMessage(const std::string &message);

private:
	GtkWindow *myParent;
	GtkWidget *myLabel;
};

#endif /* __ZLGTKPROGRESSDIALOG_H__ */

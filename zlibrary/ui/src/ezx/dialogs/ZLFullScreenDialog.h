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

#ifndef __ZLFULLSCREENDIALOG_H__
#define __ZLFULLSCREENDIALOG_H__

#include <string>

#include <zmainwidget.h>

class ZLFullScreenDialog : public ZMainWidget {
	Q_OBJECT

public:
	enum DialogCode { Rejected, Accepted };

protected:
	ZLFullScreenDialog(const std::string &caption);
	virtual ~ZLFullScreenDialog();

	void setResultCode(DialogCode code) { myResultCode = code; }	
	void resizeEvent(QResizeEvent *event);
	void keyPressEvent(QKeyEvent *event);

public:
	DialogCode exec();
	DialogCode result() const { return myResultCode; }

protected slots:
	virtual void accept();
	virtual void reject();

private:
	bool myInLoop;
	DialogCode myResultCode;
};

#endif /* __ZLFULLSCREENDIALOG_H__ */

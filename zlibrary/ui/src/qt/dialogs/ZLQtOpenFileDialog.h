/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLQTOPENFILEDIALOG_H__
#define __ZLQTOPENFILEDIALOG_H__

#include "../../../../core/src/desktop/dialogs/ZLDesktopOpenFileDialog.h"

class QFileDialog;

class ZLQtOpenFileDialog : public ZLDesktopOpenFileDialog {

public:
	ZLQtOpenFileDialog(const std::string &title, const std::string &directoryPath, const std::string &filePath, const Filter &filter);
	~ZLQtOpenFileDialog();

	bool runInternal();
	std::string filePath() const;
	std::string directoryPath() const;

	void setPosition(int x, int y);
	void setSize(int width, int height);
	int x() const;
	int y() const;
	int width() const;
	int height() const;

private:
	QFileDialog *myDialog;
};

#endif /* __ZLQTOPENFILEDIALOG_H__ */

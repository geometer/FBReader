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

#ifndef __ZLCOCOAOPENFILEDIALOG_H__
#define __ZLCOCOAOPENFILEDIALOG_H__

#include <ZLOpenFileDialog.h>

class ZLCocoaOpenFileDialog : public ZLOpenFileDialog {

public:
	ZLCocoaOpenFileDialog(const std::string &directoryPath, const Filter &filter);

private:
	bool run();
	std::string filePath() const;
	std::string directoryPath() const;

private:
	std::string myDirectoryPath;
	const Filter &myFilter;
};

#endif /* __ZLCOCOAOPENFILEDIALOG_H__ */

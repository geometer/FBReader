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

#include "../../../../core/src/dialogs/ZLOpenFileDialog.h"
#include <QtCore/QObject>

class QFileDialog;

class ZLQmlOpenFileDialog : public QObject, public ZLOpenFileDialog {
	Q_OBJECT
	Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
	Q_PROPERTY(QString directoryPath READ dirPath WRITE setDirPath NOTIFY dirPathChanged)
public:
	ZLQmlOpenFileDialog(const std::string &title, const std::string &directoryPath, const std::string &filePath, const Filter &filter);
	~ZLQmlOpenFileDialog();
	
	bool run();
	std::string filePath() const;
	std::string directoryPath() const;
	QString fileName() const;
	void setFileName(const QString &fileName);
	QString dirPath() const;
	void setDirPath(const QString &dirPath);
	
public Q_SLOTS:
	void finish();
	bool check(const QString &filePath);
	
Q_SIGNALS:
	void finished();
	void fileNameChanged(const QString &fileName);
	void dirPathChanged(const QString &dirPath);

private:
	const Filter &myFilter;
	QString myFilePath;
	QString myDirPath;
};

#endif /* __ZLQTOPENFILEDIALOG_H__ */

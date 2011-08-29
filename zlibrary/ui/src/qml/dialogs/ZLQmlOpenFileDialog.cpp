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

#include <QtGui/QFileDialog>
#include <QtCore/QEventLoop>

#include "ZLQmlOpenFileDialog.h"
#include <ZLFile.h>

ZLQmlOpenFileDialog::ZLQmlOpenFileDialog(const std::string &title, const std::string &directoryPath,
                                         const std::string &filePath, const Filter &filter) : myFilter(filter) {
	Q_UNUSED(title);
	myFilePath = QString::fromUtf8(filePath.c_str());
	myDirPath = QString::fromUtf8(directoryPath.c_str());
	qDebug("%s", Q_FUNC_INFO);
}

ZLQmlOpenFileDialog::~ZLQmlOpenFileDialog() {
	qDebug("%s", Q_FUNC_INFO);
}

bool ZLQmlOpenFileDialog::run() {
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
    QEventLoop eventLoop;
	connect(this, SIGNAL(finished()), &eventLoop, SLOT(quit()), Qt::QueuedConnection);
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
    eventLoop.exec(QEventLoop::AllEvents);
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	return !myFilePath.isEmpty();
}

std::string ZLQmlOpenFileDialog::filePath() const {
	return myFilePath.toUtf8().constData();
}

std::string ZLQmlOpenFileDialog::directoryPath() const {
	return myDirPath.toUtf8().constData();
}

QString ZLQmlOpenFileDialog::fileName() const {
	return myFilePath;
}

void ZLQmlOpenFileDialog::setFileName(const QString &fileName) {
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	myFilePath = fileName;
	emit fileNameChanged(myFilePath);
}

QString ZLQmlOpenFileDialog::dirPath() const {
	return myDirPath;
}

void ZLQmlOpenFileDialog::setDirPath(const QString &dirPath) {
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	myDirPath = dirPath;
	emit dirPathChanged(myDirPath);
}

void ZLQmlOpenFileDialog::finish() {
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	emit finished();
}

bool ZLQmlOpenFileDialog::check(const QString &filePath) {
	std::string fileName = filePath.toUtf8().constData();
	ZLFile file(fileName);
	return myFilter.accepts(file);
}

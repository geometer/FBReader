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

#include <QtGui/QFileDialog>
#include <QtCore/QEventLoop>
#include <QtCore/QCoreApplication>

#include "ZLQmlOpenFileDialog.h"
#include <ZLFile.h>

ZLQmlOpenFileDialog::ZLQmlOpenFileDialog(const std::string &title, const std::string &directoryPath,
                                         const std::string &filePath, const Filter &filter) : myFilter(filter) {
	Q_UNUSED(title);
	myFilePath = QString::fromUtf8(filePath.c_str());
	myDirPath = QString::fromUtf8(directoryPath.c_str());
}

ZLQmlOpenFileDialog::~ZLQmlOpenFileDialog() {
}

bool ZLQmlOpenFileDialog::run() {
    QEventLoop eventLoop;
	connect(this, SIGNAL(finished()), &eventLoop, SLOT(quit()), Qt::QueuedConnection);
    eventLoop.exec(QEventLoop::AllEvents);
	qApp->sendPostedEvents(0, QEvent::DeferredDelete);
	return !myFilePath.isEmpty();
}

std::string ZLQmlOpenFileDialog::filePath() const {
	return myFilePath.toStdString();
}

std::string ZLQmlOpenFileDialog::directoryPath() const {
	return myDirPath.toStdString();
}

QString ZLQmlOpenFileDialog::fileName() const {
	return myFilePath;
}

void ZLQmlOpenFileDialog::setFileName(const QString &fileName) {
	myFilePath = fileName;
	emit fileNameChanged(myFilePath);
}

QString ZLQmlOpenFileDialog::dirPath() const {
	return myDirPath;
}

void ZLQmlOpenFileDialog::setDirPath(const QString &dirPath) {
	myDirPath = dirPath;
	emit dirPathChanged(myDirPath);
}

void ZLQmlOpenFileDialog::finish() {
	emit finished();
}

bool ZLQmlOpenFileDialog::check(const QString &filePath) {
	std::string fileName = filePath.toStdString();
	ZLFile file(fileName);
	return myFilter.accepts(file);
}

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

#include <QtCore/QString>
#include <QtCore/QDir>

#include "ZLQtFSManager.h"

ZLQtFSManager::ZLQtFSManager() {
	connect(&myWatcher, SIGNAL(directoryChanged(QString)), SLOT(onPathChanged(QString)));
	connect(&myWatcher, SIGNAL(fileChanged(QString)), SLOT(onPathChanged(QString)));
}

ZLQtFSManager::~ZLQtFSManager() {
}

static QString fixPath(const QString &path) {
	if (path.startsWith('~'))
		return QDir::homePath().append(path.midRef(1));
	else if (path.isEmpty())
		return QDir::homePath();
	else
		return path;
}

void ZLQtFSManager::addWatcher(const std::string &path, shared_ptr<ZLFSWatcher> watcher) {
	QString qPath = fixPath(QString::fromStdString(path));
	myWatchers.insert(qPath, watcher);
	myWatcher.addPath(qPath);
}

void ZLQtFSManager::removeWatcher(const std::string &path, shared_ptr<ZLFSWatcher> watcher) {
	QString qPath = fixPath(QString::fromStdString(path));
	myWatchers.remove(qPath, watcher);
	if (!myWatchers.contains(qPath))
		myWatcher.removePath(qPath);
}

void ZLQtFSManager::onPathChanged(const QString &path) {
	std::string stlPath = path.toStdString();
	foreach (shared_ptr<ZLFSWatcher> watcher, myWatchers.values(path))
		watcher->onPathChanged(stlPath);
}

std::string ZLQtFSManager::convertFilenameToUtf8(const std::string &name) const {
	if (name.empty()) {
		return name;
	}

	QString qString = QString::fromLocal8Bit(name.c_str());
	return (qString == QString::null) ? "" : (const char*)qString.toUtf8();
}

std::string ZLQtFSManager::mimeType(const std::string &path) const {
	// TODO: implement
	return std::string();
}

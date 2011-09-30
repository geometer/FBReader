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

#include <QtCore/QDir>
#include <QtCore/QDebug>

#include "ZLQtFSDir.h"
#include "ZLQtFSManager.h"

void ZLQtFSDir::collectSubDirs(std::vector<std::string> &names, bool includeSymlinks) {
    //TODO don't collect not-permissed dirs
    collect(QDir::AllDirs | QDir::NoDotAndDotDot, names, includeSymlinks);
}

void ZLQtFSDir::collectFiles(std::vector<std::string> &names, bool includeSymlinks) {
    collect(QDir::Files, names, includeSymlinks);
}

void ZLQtFSDir::collect(QDir::Filters filters, std::vector<std::string> &names, bool includeSymlinks) {
    QDir dir(QString::fromStdString(path()));
    if (!includeSymlinks) {
        filters |= QDir::NoSymLinks;
    }
    dir.setFilter(filters);
    dir.setSorting(QDir::Name);
    QStringList list = dir.entryList();
    for (int i = 0; i < list.size(); ++i) {
        names.push_back(list[i].toStdString());
        qDebug() << "collecting" << list[i];
     }

}

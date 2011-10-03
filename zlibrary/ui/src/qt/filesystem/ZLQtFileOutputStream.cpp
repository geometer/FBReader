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

#include "ZLQtFileOutputStream.h"
#include "ZLQtFSManager.h"

#include <QtCore/QDebug>

ZLQtFileOutputStream::ZLQtFileOutputStream(const std::string &name) : myName(name), myHasErrors(false) {
}

ZLQtFileOutputStream::~ZLQtFileOutputStream() {
                                                                                                                qDebug() << Q_FUNC_INFO;
        close();
}

bool ZLQtFileOutputStream::open() {
        qDebug() << Q_FUNC_INFO;
        close();

        myFile.setFileName(QString::fromStdString(myName));
        myFile.open(QIODevice::WriteOnly);
        qDebug() << "opening for write" << myFile.fileName();
        //TODO implement for temporary files?
//        myTemporaryName = myName + ".XXXXXX" + '\0';
//        mode_t currentMask = umask(S_IRWXO | S_IRWXG);
//        int temporaryFileDescriptor = ::mkstemp(const_cast<char*>(myTemporaryName.data()));
//        umask(currentMask);
//        if (temporaryFileDescriptor == -1) {
//                return false;
//        }

//        myFile = fdopen(temporaryFileDescriptor, "w+");
        return myFile.isOpen();
}

void ZLQtFileOutputStream::write(const char *data, size_t len) {
        qDebug() << Q_FUNC_INFO;
        if (myFile.write(data,len) != len) {
                myHasErrors = true;
        }
}

void ZLQtFileOutputStream::write(const std::string &str) {
        //qDebug() << Q_FUNC_INFO << QString::fromStdString(str);
        if (myFile.write(str.data(),str.length()) != (size_t)str.length()) {
            qDebug() << "MY HAS ERRORS!";
                myHasErrors = true;
        }
}

void ZLQtFileOutputStream::close() {
    qDebug() << Q_FUNC_INFO;
        if (myFile.isOpen()) {
            myFile.close();
        }
        //TODO implement for temporary files?
//                ::fclose(myFile);
//                myFile = 0;
//                if (!myHasErrors) {
//                        rename(myTemporaryName.c_str(), myName.c_str());
//                }
//        }
}

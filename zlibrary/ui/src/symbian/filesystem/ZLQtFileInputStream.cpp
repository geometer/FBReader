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

#include <QtCore/QDebug>

#include "ZLQtFileInputStream.h"

ZLQtFileInputStream::ZLQtFileInputStream(const std::string &name) : myName(name), myNeedRepositionToStart(false)
{
}

ZLQtFileInputStream::~ZLQtFileInputStream() {
        myFile.close();
}

bool ZLQtFileInputStream::open() {
        if(!myFile.isOpen()) {
            myFile.setFileName(QString::fromStdString(myName));
            //qDebug() << "opening" << myFile.fileName();
            myFile.open(QIODevice::ReadOnly);
	} else {
                myFile.seek(0);
		myNeedRepositionToStart = true;
	}
        return myFile.isOpen();
}

size_t ZLQtFileInputStream::read(char *buffer, size_t maxSize) {
	if (buffer != 0) {
		if (myNeedRepositionToStart) {
                        myFile.seek(0);
			myNeedRepositionToStart = false;
		}
                return myFile.read(buffer, maxSize);
	} else {
		if (myNeedRepositionToStart) {
                        //tell size?
                        myFile.seek(maxSize);
                        myNeedRepositionToStart = false;
                        return myFile.pos();
		} else {
                        //tell how long we can provide?
                        qint64 pos = myFile.pos();
                        myFile.seek(pos+maxSize);
                        return myFile.pos() - pos;
		}
	}
}

void ZLQtFileInputStream::close() {
    //in original it was commented
    myFile.close();
}

size_t ZLQtFileInputStream::sizeOfOpened() {
        if (!myFile.isOpen()) {
            return 0;
        }
        return myFile.size();
}

void ZLQtFileInputStream::seek(int offset, bool absoluteOffset) {
	if (myNeedRepositionToStart) {
		absoluteOffset = true;
		myNeedRepositionToStart = false;
	}
        if (absoluteOffset) {
            //SEEK_SET
            myFile.seek(offset);
        } else {
            //SEEK_CUR
            myFile.seek(myFile.pos() + offset);
        }
}

size_t ZLQtFileInputStream::offset() const {
        return myNeedRepositionToStart ? 0 : myFile.pos();
}

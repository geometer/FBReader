/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <abstract/ZLFile.h>

#include "MobipocketStream.h"

MobipocketStream::MobipocketStream(ZLFile &file) : PalmDocStream(file), myErrorCode(ERROR_NONE) {
}

bool MobipocketStream::open() {
	myErrorCode = ERROR_NONE;

  if (!PdbStream::open()) {
		myErrorCode = ERROR_UNKNOWN;
    return false;
  }

	myBaseSize = myBase->sizeOfOpened();

  unsigned short version;
  PdbUtil::readUnsignedShort(myBase, version);
  if ((version != 1) && (version != 2)) {
		myErrorCode = ERROR_COMPRESSION;
    return false;
  }
  myIsCompressed = (version == 2);
  myBase->seek(6);
  unsigned short records;
  PdbUtil::readUnsignedShort(myBase, records);
  myMaxRecordIndex = std::min(records, (unsigned short)(myHeader.Offsets.size() - 1));
  PdbUtil::readUnsignedShort(myBase, myMaxRecordSize);
  if (myMaxRecordSize == 0) {
		myErrorCode = ERROR_UNKNOWN;
    return false;
  }
  unsigned short reserved2;
  PdbUtil::readUnsignedShort(myBase, reserved2);
  if (reserved2 != 0) {
		myErrorCode = ERROR_ENCRIPTION;
    return false;
  }
  myBuffer = new char[myMaxRecordSize];

  myRecordIndex = 0;

  return true;
}

std::pair<int,int> MobipocketStream::imageLocation(int index) {
  index += myMaxRecordIndex + 1;
  int recordNumber = myHeader.Offsets.size();
  if (index > recordNumber - 1) {
    return std::pair<int,int>(-1, -1);
  } else {
    int start = myHeader.Offsets[index];
    int end = (index < recordNumber - 1) ?
      myHeader.Offsets[index + 1] : myBaseSize;
    return std::pair<int,int>(start, end - start);
  }
}

const std::string &MobipocketStream::error() const {
	switch (myErrorCode) {
		default:
		{
			static const std::string message = "";
			return message;
		}
		case ERROR_UNKNOWN:
		{
			static const std::string message = "Unknown Error";
			return message;
		}
		case ERROR_COMPRESSION:
		{
			static const std::string message = "Unsupported Compression Method";
			return message;
		}
		case ERROR_ENCRIPTION:
		{
			static const std::string message = "Encrypted File";
			return message;
		}
	}
}

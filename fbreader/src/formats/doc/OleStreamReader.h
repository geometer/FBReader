/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __OLESTREAMREADER_H__
#define __OLESTREAMREADER_H__

#include <ZLUnicodeUtil.h>
#include <ZLEncodingConverter.h>

#include "OleStream.h"

class OleStreamReader {
public:
	OleStreamReader(const std::string& encoding);

	bool readStream(OleStream &stream);

protected:
	virtual void parapgraphHandler(std::string paragraph) = 0;

private:
	//TODO implement get8BitChar method
	//TODO may be make getUcs2Char method as static
	ZLUnicodeUtil::Ucs2Char getUcs2Char(OleStream& stream,long *offset,long fileend);

private:
	ZLUnicodeUtil::Ucs2String myBuffer;
	char myTmpBuffer[256];
	bool myBufIsUnicode;

	shared_ptr<ZLEncodingConverter> myConverter;
	const std::string myEncoding;
};

#endif /* __OLESTREAMREADER_H__ */

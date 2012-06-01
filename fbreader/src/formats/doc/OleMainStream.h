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

#ifndef __OLEMAINSTREAM_H__
#define __OLEMAINSTREAM_H__

#include "OleStream.h"

class OleMainStream : public OleStream {
public:
	struct Piece {
		long offset;
		long length;
		bool isANSI;
	};
	typedef std::vector<Piece> Pieces;
public:
	OleMainStream(shared_ptr<OleStorage> storage, OleEntry oleEntry, shared_ptr<ZLInputStream> stream);

public:
	bool open();
	const Pieces& getPieces() const;

private:
	bool readFIB();

private: //pieces table methods
	bool readPieceTable();
	static std::string getPiecesTableBuffer(char* headerBuffer, OleStream& tableStream);
	static long getLastCP(char* buffer);

private:
	long myStartOfText;
	long myEndOfText;

	Pieces myPieces;
};
#endif /* __OLEMAINSTREAM_H__ */

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

#ifndef __NETWORKACTIONS_H__
#define __NETWORKACTIONS_H__

#include <ZLRunnable.h>

class NetworkBookItem;

class NetworkBookDownloadAction : public ZLRunnableWithKey {

public:
	NetworkBookDownloadAction(const NetworkBookItem &book, bool demo, const std::string &tag = std::string());
	ZLResourceKey key() const;
	bool makesSense() const;
	void run();

private:
	const NetworkBookItem &myBook;
	const bool myDemo;
	const std::string myTag;
};

class NetworkBookReadAction : public ZLRunnableWithKey {

public:
	NetworkBookReadAction(const NetworkBookItem &book, bool demo);
	ZLResourceKey key() const;
	bool makesSense() const;
	void run();

private:
	const NetworkBookItem &myBook;
	const bool myDemo;
};

class NetworkBookBuyDirectlyAction : public ZLRunnableWithKey {

public:
	NetworkBookBuyDirectlyAction(const NetworkBookItem &book);
	ZLResourceKey key() const;
	bool makesSense() const;
	std::string text(const ZLResource &resource) const;
	void run();

private:
	const NetworkBookItem &myBook;
};

class NetworkBookBuyInBrowserAction : public ZLRunnableWithKey {

public:
	NetworkBookBuyInBrowserAction(const NetworkBookItem &book);
	ZLResourceKey key() const;
	bool makesSense() const;
	std::string text(const ZLResource &resource) const;
	void run();

private:
	const NetworkBookItem &myBook;
};

class NetworkBookDeleteAction : public ZLRunnableWithKey {

public:
	NetworkBookDeleteAction(const NetworkBookItem &book);
	ZLResourceKey key() const;
	bool makesSense() const;
	void run();

private:
	const NetworkBookItem &myBook;
};

#endif /* __NETWORKACTIONS_H__ */

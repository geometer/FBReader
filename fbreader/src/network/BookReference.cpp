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

#include <ZLFile.h>

#include "BookReference.h"
#include "NetworkLinkCollection.h"

BookReference::BookReference(const std::string &url, Format format, Type type) : URL(url), BookFormat(format), ReferenceType(type) {
}

const std::string &BookReference::cleanURL() const {
	return URL;
}

std::string BuyBookReference::price(const std::string &price, const std::string &currency) {
	if (currency.empty()) {
		return price;
	} else if (currency == "RUB") {
		return price + " р.";
	} else if (currency == "USD") {
		return "$" + price;
	}
	return currency + " " + price;
}

BuyBookReference::BuyBookReference(const std::string &url, Format format, Type type, const std::string &price) : BookReference(url, format, type), Price(price) {
}

DecoratedBookReference::DecoratedBookReference(const BookReference &base, const std::string &url) : BookReference(url, base.BookFormat, base.ReferenceType), myCleanURL(base.cleanURL()) {
}

const std::string &DecoratedBookReference::cleanURL() const {
	return myCleanURL;
}

std::string BookReference::localCopyFileName() const {
	std::string fileName =
		NetworkLinkCollection::Instance().bookFileName(*this);
	if (!fileName.empty() && ZLFile(fileName).exists()) {
		return fileName;
	}

	fileName =
		NetworkLinkCollection::Instance().makeBookFileName(*this);
	if (!fileName.empty() && ZLFile(fileName).exists()) {
		return fileName;
	}

	return std::string();
}

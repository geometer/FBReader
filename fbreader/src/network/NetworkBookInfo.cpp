/*
 * Copyright (C) 2008-2009 Geometer Plus <contact@geometerplus.com>
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

#include "NetworkBookInfo.h"

NetworkBookInfo::NetworkBookInfo(const std::string &id) : Id(id) {
}

std::string NetworkBookInfo::fileName(URLType format) const {
	std::string fName = Author.DisplayName + "_" + Title + "_(" + Language + ").";
	switch (format) {
		case LINK_HTTP:
			break;
		case BOOK_EPUB:
			fName += "epub";
			break;
		case BOOK_MOBIPOCKET:
			fName += "mobi";
			break;
		case BOOK_PDF:
			fName += "pdf";
			break;
	}

	const std::string charsToEscape = ":;<|>+/\\";

	const size_t len = fName.length();
	for (size_t i = 0; i < len; ++i) {
		if (charsToEscape.find(fName[i]) != (size_t)-1) {
			fName[i] = '_';
		}
	}
	return fName;
}

bool NetworkBookInfo::AuthorData::operator < (const AuthorData &data) const {
	const int sComp = SortKey.compare(data.SortKey);
	return (sComp < 0) ||
				 ((sComp == 0) && (DisplayName < data.DisplayName));
}

bool NetworkBookInfo::Comparator::operator () (const shared_ptr<NetworkBookInfo> &first, const shared_ptr<NetworkBookInfo> &second) {
	const bool firstBookHasSeries = !first->Series.empty();
	const bool secondBookHasSeries = !second->Series.empty();
	if (firstBookHasSeries && secondBookHasSeries) {
		const int comp = first->Series.compare(second->Series);
		if (comp != 0) {
			return comp < 0;
		}
		return first->Title < second->Title;
	}

	const std::string &firstKey =
		firstBookHasSeries ? first->Series : first->Title;
	const std::string &secondKey =
		secondBookHasSeries ? second->Series : second->Title;
	const int comp = firstKey.compare(secondKey);
	if (comp != 0) {
		return comp < 0;
	}
	return secondBookHasSeries;
}

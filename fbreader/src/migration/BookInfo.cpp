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

#include "BookInfo.h"

#include "../options/FBCategoryKey.h"

static const std::string EMPTY = "";

BookInfo::BookInfo(const std::string &fileName) : 
	AuthorDisplayNameOption(FBCategoryKey::BOOKS, fileName, "AuthorDisplayName", EMPTY),
	AuthorSortKeyOption(FBCategoryKey::BOOKS, fileName, "AuthorSortKey", EMPTY),
	TitleOption(FBCategoryKey::BOOKS, fileName, "Title", EMPTY),
	SeriesTitleOption(FBCategoryKey::BOOKS, fileName, "Sequence", EMPTY),
	IndexInSeriesOption(FBCategoryKey::BOOKS, fileName, "Number in seq", 0, 100, 0),
	LanguageOption(FBCategoryKey::BOOKS, fileName, "Language", EMPTY),
	EncodingOption(FBCategoryKey::BOOKS, fileName, "Encoding", EMPTY),
	TagsOption(FBCategoryKey::BOOKS, fileName, "TagList", EMPTY) {
}

void BookInfo::reset() {
	AuthorDisplayNameOption.setValue(EMPTY);
	AuthorSortKeyOption.setValue(EMPTY);
	TitleOption.setValue(EMPTY);
	SeriesTitleOption.setValue(EMPTY);
	IndexInSeriesOption.setValue(0);
	LanguageOption.setValue(EMPTY);
	EncodingOption.setValue(EMPTY);
	TagsOption.setValue(EMPTY);
}

bool BookInfo::isFull() const {
	return
		!AuthorDisplayNameOption.value().empty() &&
		!AuthorSortKeyOption.value().empty() &&
		!TitleOption.value().empty() &&
		!EncodingOption.value().empty();
}

const BookInfo &BookInfo::operator = (const BookInfo &bi) {
	AuthorDisplayNameOption.setValue(bi.AuthorDisplayNameOption.value());
	AuthorSortKeyOption.setValue(bi.AuthorSortKeyOption.value());
	TitleOption.setValue(bi.TitleOption.value());
	SeriesTitleOption.setValue(bi.SeriesTitleOption.value());
	IndexInSeriesOption.setValue(bi.IndexInSeriesOption.value());
	LanguageOption.setValue(bi.LanguageOption.value());
	EncodingOption.setValue(bi.EncodingOption.value());
	TagsOption.setValue(bi.TagsOption.value());
	return bi;
}

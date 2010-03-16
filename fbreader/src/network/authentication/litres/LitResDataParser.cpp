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

#include <cstdlib>

#include <ZLStringUtil.h>

#include "LitResDataParser.h"
#include "LitResGenre.h"
#include "../../NetworkLink.h"

static const std::string TAG_CATALOG = "catalit-fb2-books";
static const std::string TAG_BOOK = "fb2-book";
static const std::string TAG_TEXT_DESCRIPTION = "text_description";
static const std::string TAG_HIDDEN = "hidden";
static const std::string TAG_TITLE_INFO = "title-info";
static const std::string TAG_GENRE = "genre";
static const std::string TAG_AUTHOR = "author";
static const std::string TAG_FIRST_NAME = "first-name";
static const std::string TAG_MIDDLE_NAME = "middle-name";
static const std::string TAG_LAST_NAME = "last-name";
static const std::string TAG_BOOK_TITLE = "book-title";
static const std::string TAG_ANNOTATION = "annotation";
static const std::string TAG_DATE = "date";
static const std::string TAG_SEQUENCE = "sequence";
static const std::string TAG_LANGUAGE = "lang";

std::string LitResDataParser::stringAttributeValue(const char **attributes, const char *name) {
	if (attributes == 0) {
		return std::string();
	}
	const char *value = attributeValue(attributes, name);
	return value != 0 ? value : std::string();
}

LitResDataParser::LitResDataParser(const NetworkLink &link, NetworkItem::List &books) : 
	myLink(link),
	myBooks(books), 
	myIndex(0) {
	myState = START;
}


void LitResDataParser::startElementHandler(const char *tag, const char **attributes) {
	processState(tag, false, attributes);
	myState = getNextState(tag, false);
	myBuffer.clear();
}

void LitResDataParser::endElementHandler(const char *tag) {
	processState(tag, true, 0);
	myState = getNextState(tag, true);
	myBuffer.clear();
}

void LitResDataParser::characterDataHandler(const char *data, size_t len) {
	myBuffer.append(data, len);
}

void LitResDataParser::processState(const std::string &tag, bool closed, const char **attributes) {
	switch(myState) {
	case START: 
		break;
	case CATALOG: 
		if (!closed && TAG_BOOK == tag) {
			myBookId = stringAttributeValue(attributes, "hub_id");
			myURLByType[NetworkItem::URL_COVER] =
				stringAttributeValue(attributes, "cover_preview");

			std::string url = stringAttributeValue(attributes, "url");
			if (!url.empty()) {
				myLink.rewriteUrl(url);
				myURLByType[NetworkItem::URL_HTML_PAGE] = url;
			}

			myReferences.push_back(new BookReference(
				"https://robot.litres.ru/pages/catalit_download_book/?art=" + myBookId,
				BookReference::FB2_ZIP,
				BookReference::DOWNLOAD_FULL_CONDITIONAL
			));
		}
		break;
	case BOOK: 
		if (closed && TAG_BOOK == tag) {
			myBooks.push_back(new NetworkBookItem(
				myLink,
				myBookId,
				myIndex++,
				myTitle,
				mySummary,
				myLanguage,
				myDate,
				myAuthors,
				myTags,
				mySeriesTitle,
				myIndexInSeries,
				myURLByType,
				myReferences
			));

			myTitle.erase();
			mySummary.erase();
			myLanguage.erase();
			myDate.erase();
			mySeriesTitle.erase();
			myIndexInSeries = 0;
			myAuthors.clear();
			myTags.clear();
			myURLByType.clear();
			myReferences.clear();
		}
		break;
	case BOOK_DESCRIPTION: 
		break;
	case HIDDEN: 
		break;
	case TITLE_INFO: 
		if (!closed) {
			if (TAG_AUTHOR == tag) {
				myAuthorFirstName.clear();
				myAuthorMiddleName.clear();
				myAuthorLastName.clear();
			} else if (TAG_SEQUENCE == tag) {
				mySeriesTitle = stringAttributeValue(attributes, "name");
				if (!mySeriesTitle.empty()) {
					const char *indexInSeries = attributeValue(attributes, "number");
					myIndexInSeries = indexInSeries != 0 ? atoi(indexInSeries) : 0;
				}
			}
		} 
		break;
	case AUTHOR: 
		if (closed && TAG_AUTHOR == tag) {
			NetworkBookItem::AuthorData data;
			if (!myAuthorFirstName.empty()) {
				data.DisplayName.append(myAuthorFirstName);
			}
			if (!myAuthorMiddleName.empty()) {
				if (!data.DisplayName.empty()) {
					data.DisplayName.append(" ");
				}
				data.DisplayName.append(myAuthorMiddleName);
			}
			if (!myAuthorLastName.empty()) {
				if (!data.DisplayName.empty()) {
					data.DisplayName.append(" ");
				}
				data.DisplayName.append(myAuthorLastName);
			}
			data.SortKey = myAuthorLastName;
			myAuthors.push_back(data);
		}
		break;
	case FIRST_NAME: 
		if (closed && TAG_FIRST_NAME == tag) {
			ZLStringUtil::stripWhiteSpaces(myBuffer);
			myAuthorFirstName = myBuffer;
		}
		break;
	case MIDDLE_NAME: 
		if (closed && TAG_MIDDLE_NAME == tag) {
			ZLStringUtil::stripWhiteSpaces(myBuffer);
			myAuthorMiddleName = myBuffer;
		}
		break;
	case LAST_NAME: 
		if (closed && TAG_LAST_NAME == tag) {
			ZLStringUtil::stripWhiteSpaces(myBuffer);
			myAuthorLastName = myBuffer;
		}
		break;
	case GENRE: 
		if (closed && TAG_GENRE == tag) {
			ZLStringUtil::stripWhiteSpaces(myBuffer);

			const std::map<std::string,shared_ptr<LitResGenre> > &genresMap =
				LitResGenreMap::Instance().genresMap();
			const std::map<shared_ptr<LitResGenre>,std::string> &genresTitles =
				LitResGenreMap::Instance().genresTitles();

			std::map<std::string, shared_ptr<LitResGenre> >::const_iterator it = genresMap.find(myBuffer);
			if (it != genresMap.end()) {
				std::map<shared_ptr<LitResGenre>, std::string>::const_iterator jt = genresTitles.find(it->second);
				if (jt != genresTitles.end()) {
					myTags.push_back(jt->second);
				}
			}
		}
		break;
	case BOOK_TITLE: 
		if (closed && TAG_BOOK_TITLE == tag) {
			ZLStringUtil::stripWhiteSpaces(myBuffer);
			myTitle = myBuffer;
		}
		break;
	case ANNOTATION: 
		if (!closed) {
			ZLStringUtil::stripWhiteSpaces(myBuffer);
			if (!myBuffer.empty()) {
				mySummary.append(myBuffer);
				mySummary.append(" ");
			}
		} else {
			ZLStringUtil::stripWhiteSpaces(myBuffer);
			mySummary.append(myBuffer);
			int size = mySummary.size();
			if (size > 0) {
				if (TAG_ANNOTATION == tag) {
					if (mySummary[size - 1] == '\n') {
						mySummary.erase(size - 1);
					}
				} else if ("p" == tag) {
					if (mySummary[size - 1] != '\n') {
						mySummary.append("\n");
					}
				} else {
					if (!myBuffer.empty() && mySummary[size - 1] != '\n') {
						mySummary.append(" ");
					}
				}
			}
		}
		break;
	case DATE:
		if (closed && TAG_DATE == tag) {
			ZLStringUtil::stripWhiteSpaces(myBuffer);
			myDate = myBuffer;
		}
		break;
	case LANGUAGE:
		if (closed && TAG_LANGUAGE == tag) {
			ZLStringUtil::stripWhiteSpaces(myBuffer);
			myLanguage = myBuffer;
		}
		break;
	}
}

LitResDataParser::State LitResDataParser::getNextState(const std::string &tag, bool closed) {
	switch(myState) {
	case START: 
		if (!closed && TAG_CATALOG == tag) {
			return CATALOG;
		}
		break;
	case CATALOG: 
		if (!closed) {
			if (TAG_BOOK == tag) {
				return BOOK;
			} 
		} else {
			if (TAG_CATALOG == tag) {
				return START;
			} 
		}
		break;
	case BOOK: 
		if (!closed) {
			if (TAG_TEXT_DESCRIPTION == tag) {
				return BOOK_DESCRIPTION;
			} 
		} else {
			if (TAG_BOOK == tag) {
				return CATALOG;
			} 
		}
		break;
	case BOOK_DESCRIPTION: 
		if (!closed) {
			if (TAG_HIDDEN == tag) {
				return HIDDEN;
			} 
		} else {
			if (TAG_TEXT_DESCRIPTION == tag) {
				return BOOK;
			} 
		}
		break;
	case HIDDEN: 
		if (!closed) {
			if (TAG_TITLE_INFO == tag) {
				return TITLE_INFO;
			} 
		} else {
			if (TAG_HIDDEN == tag) {
				return BOOK_DESCRIPTION;
			} 
		}
		break;
	case TITLE_INFO: 
		if (!closed) {
			if (TAG_GENRE == tag) {
				return GENRE;
			} else if (TAG_AUTHOR == tag) {
				return AUTHOR;
			} else if (TAG_BOOK_TITLE == tag) {
				return BOOK_TITLE;
			} else if (TAG_ANNOTATION == tag) {
				return ANNOTATION;
			} else if (TAG_DATE == tag) {
				return DATE;
			} else if (TAG_LANGUAGE == tag) {
				return LANGUAGE;
			} /*else if (TAG_SEQUENCE == tag) {
				return SEQUENCE; // handled without state through attributes 
			}*/ 
		} else {
			if (TAG_TITLE_INFO == tag) {
				return HIDDEN;
			} 
		}
		break;
	case AUTHOR: 
		if (!closed) {
			if (TAG_FIRST_NAME == tag) {
				return FIRST_NAME;
			} else if (TAG_MIDDLE_NAME == tag) {
				return MIDDLE_NAME;
			} else if (TAG_LAST_NAME == tag) {
				return LAST_NAME;
			}
		} else {
			if (TAG_AUTHOR == tag) {
				return TITLE_INFO;
			} 
		}
		break;
	case FIRST_NAME: 
		if (closed && TAG_FIRST_NAME == tag) {
			return AUTHOR;
		}
		break;
	case MIDDLE_NAME: 
		if (closed && TAG_MIDDLE_NAME == tag) {
			return AUTHOR;
		}
		break;
	case LAST_NAME: 
		if (closed && TAG_LAST_NAME == tag) {
			return AUTHOR;
		}
		break;
	case GENRE: 
		if (closed && TAG_GENRE == tag) {
			return TITLE_INFO;
		}
		break;
	case BOOK_TITLE: 
		if (closed && TAG_BOOK_TITLE == tag) {
			return TITLE_INFO;
		}
		break;
	case ANNOTATION: 
		if (closed && TAG_ANNOTATION == tag) {
			return TITLE_INFO;
		}
		break;
	case DATE:
		if (closed && TAG_DATE == tag) {
			return TITLE_INFO;
		}
		break;
	case LANGUAGE:
		if (closed && TAG_LANGUAGE == tag) {
			return TITLE_INFO;
		}
		break;
	}
	return myState;
}


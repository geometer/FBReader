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

#include "OPDSMetadata.h"


const std::string OPDSConstants::REL_BOOKSHELF     = "http://opds-spec.org/bookshelf";
const std::string OPDSConstants::REL_SUBSCRIPTIONS = "http://opds-spec.org/subscriptions";
const std::string OPDSConstants::REL_CATALOG_AUTHOR = "http://data.fbreader.org/catalog/author";
const std::string OPDSConstants::REL_ACQUISITION   = "http://opds-spec.org/acquisition";
const std::string OPDSConstants::REL_ACQUISITION_BUY        = "http://opds-spec.org/acquisition/buy";
const std::string OPDSConstants::REL_ACQUISITION_BORROW     = "http://opds-spec.org/acquisition/borrow";
const std::string OPDSConstants::REL_ACQUISITION_SUBSCRIBE  = "http://opds-spec.org/acquisition/subscribe";
const std::string OPDSConstants::REL_ACQUISITION_SAMPLE     = "http://opds-spec.org/acquisition/sample";
const std::string OPDSConstants::REL_ACQUISITION_CONDITIONAL     = "http://data.fbreader.org/acquisition/conditional";
const std::string OPDSConstants::REL_ACQUISITION_SAMPLE_OR_FULL     = "http://data.fbreader.org/acquisition/sampleOrFull";

const std::string OPDSConstants::REL_COVER         = "http://opds-spec.org/cover";
const std::string OPDSConstants::REL_THUMBNAIL     = "http://opds-spec.org/thumbnail";

const std::string OPDSConstants::MIME_OPDS_FULLENTRY = "application/atom+xml;type=entry";

const std::string OPDSConstants::MIME_APP_FB2ZIP  = "application/fb2+zip";
const std::string OPDSConstants::MIME_APP_EPUB  = "application/epub+zip";
const std::string OPDSConstants::MIME_APP_MOBI  = "application/x-mobipocket-ebook";
const std::string OPDSConstants::MIME_APP_PDF   = "application/pdf";
const std::string OPDSConstants::MIME_APP_ATOM  = "application/atom+xml";
const std::string OPDSConstants::MIME_APP_LITRES  = "application/litres+xml";

const std::string OPDSConstants::MIME_IMG_PNG   = "image/png";
const std::string OPDSConstants::MIME_IMG_JPEG  = "image/jpeg";
const std::string OPDSConstants::MIME_TEXT_HTML = "text/html";




DCDate::DCDate() : 
	ATOMDateConstruct(0) {
}

DCDate::DCDate(int year) : 
	ATOMDateConstruct(year) {
}

DCDate::DCDate(int year, int month, int day) :
	ATOMDateConstruct(year, month, day) {
}

DCDate::DCDate(int year, int month, int day, int hour, int minutes, int seconds) :
	ATOMDateConstruct(year, month, day, hour, minutes, seconds) {
}

DCDate::DCDate(int year, int month, int day, int hour, int minutes, int seconds, float sfract) :
	ATOMDateConstruct(year, month, day, hour, minutes, seconds, sfract) {
}

DCDate::DCDate(int year, int month, int day, int hour, int minutes, int seconds, float sfract, int tzhour, int tzminutes) :
	ATOMDateConstruct(year, month, day, hour, minutes, seconds, sfract, tzhour, tzminutes) {
}

OPDSEntry::OPDSEntry() {
}

OPDSEntry::OPDSEntry(shared_ptr<ATOMId> id, const std::string &title, shared_ptr<ATOMUpdated> updated) :
	ATOMEntry(id, title, updated) {
}

OPDSFeedMetadata::OPDSFeedMetadata() : myOpensearchTotalResults(0), myOpensearchItemsPerPage(0), myOpensearchStartIndex(1) {
}

OPDSFeedMetadata::OPDSFeedMetadata(shared_ptr<ATOMId> id, const std::string &title, shared_ptr<ATOMUpdated> updated) :
	ATOMFeedMetadata(id, title, updated), myOpensearchTotalResults(0), myOpensearchItemsPerPage(0), myOpensearchStartIndex(1) {
}

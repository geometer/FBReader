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

#ifndef __OPDSMETADATA_H__
#define __OPDSMETADATA_H__

#include <map>

#include "../atom/ATOMContainers.h"


class OPDSConstants {

private:
	OPDSConstants();

public:
	// Feed level
	static const std::string REL_BOOKSHELF;
	static const std::string REL_SUBSCRIPTIONS;

	// Entry level / catalog types
	static const std::string REL_CATALOG_AUTHOR;

	// Entry level / acquisition links
	static const std::string REL_ACQUISITION;
	static const std::string REL_ACQUISITION_BUY;
	static const std::string REL_ACQUISITION_BORROW;
	static const std::string REL_ACQUISITION_SUBSCRIBE;
	static const std::string REL_ACQUISITION_SAMPLE;
	static const std::string REL_ACQUISITION_CONDITIONAL;
	static const std::string REL_ACQUISITION_SAMPLE_OR_FULL;

	// Entry level / other
	static const std::string REL_COVER;
	static const std::string REL_THUMBNAIL;

	// MIME types / MIME type for "Full Entry" atom:link element
	static const std::string MIME_OPDS_FULLENTRY;

	// MIME types / application
	static const std::string MIME_APP_FB2ZIP;
	static const std::string MIME_APP_EPUB;
	static const std::string MIME_APP_MOBI;
	static const std::string MIME_APP_PDF;
	static const std::string MIME_APP_ATOM;

	// a special MIME type for the litres OPDS catalog
	static const std::string MIME_APP_LITRES;

	// MIME types / image
	static const std::string MIME_IMG_PNG;
	static const std::string MIME_IMG_JPEG;

	// MIME types / text
	static const std::string MIME_TEXT_HTML;
};


class DCDate : public ATOMDateConstruct {

public:
	DCDate();
	DCDate(int year);
	DCDate(int year, int month, int day);
	DCDate(int year, int month, int day, int hour, int minutes, int seconds);
	DCDate(int year, int month, int day, int hour, int minutes, int seconds, float sfract);
	DCDate(int year, int month, int day, int hour, int minutes, int seconds, float sfract, int tzhour, int tzminutes);
};

class OPDSEntry : public ATOMEntry {

public:
	OPDSEntry();
	OPDSEntry(shared_ptr<ATOMId> id, const std::string &title, shared_ptr<ATOMUpdated> updated);

	const std::string &dcLanguage() const { return myDCLanguage; }
	const std::string &dcPublisher() const { return myDCPublisher; }
	shared_ptr<DCDate> dcIssued() { return myDCIssued; }
	const std::string &seriesTitle() const { return mySeriesTitle; }
	int seriesIndex() const { return mySeriesIndex; }

	void setDCLanguage(const std::string &language) { myDCLanguage = language; }
	void setDCPublisher(const std::string &publisher) { myDCPublisher = publisher; }
	void setDCIssued(shared_ptr<DCDate> issued) { myDCIssued = issued; }
	void setSeriesTitle(const std::string &seriesTitle) { mySeriesTitle = seriesTitle; }
	void setSeriesIndex(int seriesIndex) { mySeriesIndex = seriesIndex; }

private:
	std::string myDCLanguage;
	std::string myDCPublisher;
	shared_ptr<DCDate> myDCIssued;

	std::string mySeriesTitle;
	int mySeriesIndex;
};



class OPDSFeedMetadata : public ATOMFeedMetadata {

public:
	OPDSFeedMetadata();
	OPDSFeedMetadata(shared_ptr<ATOMId> id, const std::string &title, shared_ptr<ATOMUpdated> updated);

	unsigned long getOpensearchTotalResults() const;
	unsigned long getOpensearchItemsPerPage() const;
	unsigned long getOpensearchStartIndex() const;

	void setOpensearchTotalResults(unsigned long number);
	void setOpensearchItemsPerPage(unsigned long number);
	void setOpensearchStartIndex(unsigned long number);

private:
	unsigned long myOpensearchTotalResults;
	unsigned long myOpensearchItemsPerPage;
	unsigned long myOpensearchStartIndex;
};

inline unsigned long OPDSFeedMetadata::getOpensearchTotalResults() const { return myOpensearchTotalResults; }
inline unsigned long OPDSFeedMetadata::getOpensearchItemsPerPage() const { return myOpensearchItemsPerPage; }
inline unsigned long OPDSFeedMetadata::getOpensearchStartIndex() const { return myOpensearchStartIndex; }
inline void OPDSFeedMetadata::setOpensearchTotalResults(unsigned long number) { myOpensearchTotalResults = number; }
inline void OPDSFeedMetadata::setOpensearchItemsPerPage(unsigned long number) { myOpensearchItemsPerPage = number; }
inline void OPDSFeedMetadata::setOpensearchStartIndex(unsigned long number) { myOpensearchStartIndex = number; }

#endif /* __OPDSMETADATA_H__ */

/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __OPDSBOOKITEM_H__
#define __OPDSBOOKITEM_H__

#include "OPDSLink.h"
#include "OPDSMetadata.h"
#include "OPDSFeedReader.h"

class OPDSBookItem : public NetworkBookItem {

public:
	OPDSBookItem(const OPDSLink &networkLink, OPDSEntry &entry, std::string baseUrl, unsigned int index);

public:
	bool isFullyLoaded() const;
	void loadFullInformation();
	std::vector<shared_ptr<NetworkItem> > getRelatedCatalogsItems() const;

public:
	static BookReference::Format formatByZLMimeType(const std::string &mimeType);
	static BookReference::Type typeByRelation(const std::string &rel);

protected:
	static std::string getAnnotation(OPDSEntry &entry);
	static std::string getDate(OPDSEntry &entry);
	static std::vector<AuthorData> getAuthors(OPDSEntry &entry);
	static std::vector<std::string> getTags(OPDSEntry &entry);
	static UrlInfoCollection getUrls(const OPDSLink &networkLink, OPDSEntry &entry, std::string baseUrl);
	//TODO implement one UrlInfoCollection to not duplicate similar methods
	static std::vector<shared_ptr<BookReference> > getReferences(const OPDSLink &networkLink, OPDSEntry &entry, std::string baseUrl);

private:
	class FullEntryReader : public OPDSFeedReader {

		public:
			FullEntryReader(OPDSBookItem &item, const OPDSLink &link, std::string url);

		public:
			void processFeedEntry(shared_ptr<OPDSEntry> entry);
			void processFeedStart();
			void processFeedMetadata(shared_ptr<OPDSFeedMetadata> feed);
			void processFeedEnd();

		private:
			OPDSBookItem &myItem;
			const OPDSLink &myLink;
			std::string myUrl;
		};

	class RelatedUrlInfo {
	public:
		RelatedUrlInfo(const std::string& title, shared_ptr<ZLMimeType> mimeType, const std::string url);

		std::string Title;
		shared_ptr<ZLMimeType> MimeType;
		std::string Url;
	};

	typedef std::vector<shared_ptr<RelatedUrlInfo> > RelatedUrlsList;
	RelatedUrlsList myRelatedInfos;
protected:
	static RelatedUrlsList getRelatedUrls(const OPDSLink &networkLink, OPDSEntry &entry, std::string baseUrl);
private:
	bool myInformationIsFull;

};

#endif /* __OPDSBOOKITEM_H__ */

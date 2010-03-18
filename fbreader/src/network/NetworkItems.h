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

#ifndef __NETWORKITEMS_H__
#define __NETWORKITEMS_H__

#include <string>
#include <vector>
#include <map>

#include <shared_ptr.h>

#include <ZLFile.h>
#include <ZLTypeId.h>

#include "BookReference.h"

class NetworkAuthenticationManager;
class NetworkLink;

class NetworkItem : public ZLObjectWithRTTI {

public:
	typedef std::vector<shared_ptr<NetworkItem> > List;

	enum URLType {
		URL_NONE,
		URL_CATALOG,
		URL_HTML_PAGE,
		URL_COVER
	};

protected:
	static const ZLTypeId TYPE_ID;

protected:
	NetworkItem(
		const NetworkLink &link,
		const std::string &title,
		const std::string &summary,
		const std::map<URLType,std::string> &urlByType
	);

public:
	virtual ~NetworkItem();

	virtual const ZLTypeId &typeId() const = 0;

public:
	const NetworkLink &Link;
	const std::string Title;
	const std::string Summary;
	/*const*/ std::map<URLType,std::string> URLByType;

private: // disable copying
	NetworkItem(const NetworkItem &item);
	const NetworkItem &operator = (const NetworkItem &);
};

class NetworkCatalogItem : public NetworkItem {

public:
	static const ZLTypeId TYPE_ID;

	enum VisibilityType {
		Always,
		LoggedUsers
	};

	enum CatalogType {
		OTHER,
		BY_AUTHORS,
	};

public:
	NetworkCatalogItem(
		const NetworkLink &link,
		const std::string &title,
		const std::string &summary,
		const std::map<URLType,std::string> &urlByType,
		VisibilityType visibility = Always,
		CatalogType type = OTHER
	);

	const ZLTypeId &typeId() const;

	// method is called each time the View Node is created for the Item.
	virtual void onDisplayItem();
	// returns error message
	virtual std::string loadChildren(List &children) = 0;

public:
	const VisibilityType Visibility;
	const CatalogType Type;
};

class NetworkBookItem : public NetworkItem {

public:
	struct AuthorData {
		std::string DisplayName;
		std::string SortKey;

		bool operator < (const AuthorData &data) const;
	};

public:
	static const ZLTypeId TYPE_ID;

public:
	NetworkBookItem(
		const NetworkLink &link,
		const std::string &id,
		unsigned int index,
		const std::string &title,
		const std::string &summary,
		const std::string &language,
		const std::string &date,
		const std::vector<AuthorData> &authors,
		const std::vector<std::string> &tags,
		const std::string &seriesTitle,
		unsigned int indexInSeries,
		const std::map<URLType,std::string> &urlByType,
		const std::vector<shared_ptr<BookReference> > references
	);
	NetworkBookItem(const NetworkBookItem &book, unsigned int index);

	const ZLTypeId &typeId() const;

public:
	shared_ptr<BookReference> reference(BookReference::Type type) const;

	std::string localCopyFileName() const;
	void removeLocalFiles() const;

public:
	/*const*/ unsigned int Index;
	const std::string Id;
	const std::string Language;
	const std::string Date;
	const std::vector<AuthorData> Authors;
	const std::vector<std::string> Tags;
	const std::string SeriesTitle;
	const int IndexInSeries;

private:
	std::vector<shared_ptr<BookReference> > myReferences;
};

#endif /* __NETWORKITEMS_H__ */

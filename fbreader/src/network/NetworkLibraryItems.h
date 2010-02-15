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

#ifndef __NETWORKLIBRARYITEMS_H__
#define __NETWORKLIBRARYITEMS_H__

#include <string>
#include <vector>
#include <map>

#include <shared_ptr.h>

#include <ZLTypeId.h>

class NetworkAuthenticationManager;
class NetworkLink;

class NetworkLibraryItem : public ZLObjectWithRTTI {

public:
	typedef std::vector<shared_ptr<NetworkLibraryItem> > List;

protected:
	static const ZLTypeId TYPE_ID;

protected:
	NetworkLibraryItem(const std::string &title, const std::string &coverURL);
	explicit NetworkLibraryItem(const NetworkLibraryItem &item);

public:
	virtual ~NetworkLibraryItem();

	virtual const ZLTypeId &typeId() const = 0;

public:
	/*const*/ std::string Title;
	/*const*/ std::string CoverURL;

private: // disable copying
	const NetworkLibraryItem &operator = (const NetworkLibraryItem &);
};

class NetworkCatalogItem : public NetworkLibraryItem {

public:
	static const ZLTypeId TYPE_ID;

	enum CatalogType {
		OTHER,
		BY_AUTHORS,
	};

	enum VisibilityType {
		Always,
		Never,
		LoggedUsers
	};

public:
	NetworkCatalogItem(
		const NetworkLink &link,
		const std::string &url,
		const std::string &htmlURL,
		const std::string &title,
		const std::string &summary,
		const std::string &coverURL,
		VisibilityType visibility = Always
	);

	const ZLTypeId &typeId() const;

	// method is called each time the View Node is created for the Item.
	virtual void onDisplayItem();
	// returns error message
	virtual std::string loadChildren(List &children) = 0;

	virtual CatalogType catalogType() const;

public:
	const NetworkLink &Link;
	const std::string URL;
	const std::string HtmlURL;
	const std::string Summary;
	const VisibilityType Visibility;

private: // disable copying
	NetworkCatalogItem(const NetworkCatalogItem &);
	const NetworkCatalogItem &operator = (const NetworkCatalogItem &);
};

class NetworkBookItem : public NetworkLibraryItem {

public:
	enum URLType {
		NONE,
		BOOK_EPUB,
		BOOK_MOBIPOCKET,
		BOOK_FB2_ZIP,
		//BOOK_PDF,
		BOOK_DEMO_FB2_ZIP,
		LINK_HTTP,
	};

	struct AuthorData {
		std::string DisplayName;
		std::string SortKey;

		bool operator < (const AuthorData &data) const;
	};

public:
	static const ZLTypeId TYPE_ID;

public:
	NetworkBookItem(
		const std::string &id,
		unsigned int index,
		const std::string &title,
		const std::string &summary,
		const std::string &coverURL,
		const std::string &language
	);
	explicit NetworkBookItem(const NetworkBookItem &book);

	const ZLTypeId &typeId() const;

	//std::string fileName(URLType format) const;
	const std::string &id() const;

public:
	unsigned int index() const;
	void setIndex(unsigned int index);

	const std::string &language() const;
	const std::string &date() const;
	const std::string &seriesTitle() const;
	int indexInSeries() const;
	const std::string &price() const;
	const std::string &annotation() const;

	void setLanguage(const std::string &language);
	void setDate(const std::string &date);
	void setPrice(const std::string &price);
	std::string &annotation();
	void setAnnotation(const std::string &annotation);
	void setSeries(const std::string &name, int index);

	const std::map<URLType, std::string> &urlByType() const;
	const std::vector<AuthorData> &authors() const;
	const std::vector<std::string> &tags() const;

	std::map<URLType, std::string> &urlByType();
	std::vector<AuthorData> &authors();
	std::vector<std::string> &tags();

	shared_ptr<NetworkAuthenticationManager> authenticationManager() const;
	void setAuthenticationManager(shared_ptr<NetworkAuthenticationManager> manager);

	URLType bestBookFormat() const;
	URLType bestDemoFormat() const;

private:
	unsigned int myIndex;
	const std::string myId;
	std::string myAnnotation;
	std::string myLanguage;
	std::string myDate;
	std::string mySeriesTitle;
	int myIndexInSeries;
	std::string myPrice; // number with curency code (see http://en.wikipedia.org/wiki/List_of_circulating_currencies for example)

	std::map<URLType, std::string> myURLByType;
	std::vector<AuthorData> myAuthors;
	std::vector<std::string> myTags;

	shared_ptr<NetworkAuthenticationManager> myAuthenticationManager;

private: // disable copying
	const NetworkBookItem &operator = (const NetworkBookItem &);
};

inline unsigned int NetworkBookItem::index() const { return myIndex; }
inline void NetworkBookItem::setIndex(unsigned int index) { myIndex = index; }

inline const std::string &NetworkBookItem::language() const { return myLanguage; }
inline const std::string &NetworkBookItem::date() const { return myDate; }
inline const std::string &NetworkBookItem::seriesTitle() const { return mySeriesTitle; }
inline int NetworkBookItem::indexInSeries() const { return myIndexInSeries; }
inline const std::string &NetworkBookItem::price() const { return myPrice; }
inline const std::string &NetworkBookItem::annotation() const { return myAnnotation; }
inline std::string &NetworkBookItem::annotation() { return myAnnotation; }
inline const std::map<NetworkBookItem::URLType, std::string> &NetworkBookItem::urlByType() const { return myURLByType; }
inline const std::vector<NetworkBookItem::AuthorData> &NetworkBookItem::authors() const { return myAuthors; }
inline const std::vector<std::string> &NetworkBookItem::tags() const { return myTags; }
inline std::map<NetworkBookItem::URLType, std::string> &NetworkBookItem::urlByType() { return myURLByType; }
inline std::vector<NetworkBookItem::AuthorData> &NetworkBookItem::authors() { return myAuthors; }
inline std::vector<std::string> &NetworkBookItem::tags() { return myTags; }

#endif /* __NETWORKLIBRARYITEMS_H__ */

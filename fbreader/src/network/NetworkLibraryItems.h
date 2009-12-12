/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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

class NetworkAuthenticationManager;
class NetworkLink;

class NetworkLibraryItem {

protected:
	NetworkLibraryItem();
	explicit NetworkLibraryItem(const NetworkLibraryItem &item);

public:
	virtual ~NetworkLibraryItem();

	virtual const std::string &typeId() const = 0;

	const std::string &title() const;
	const std::string &cover() const;

	std::string &title();
	std::string &cover();

private:
	std::string myTitle;
	std::string myCover;

private: // disable copying
	const NetworkLibraryItem &operator = (const NetworkLibraryItem &);
};

inline const std::string &NetworkLibraryItem::title() const { return myTitle; }
inline const std::string &NetworkLibraryItem::cover() const { return myCover; }
inline std::string &NetworkLibraryItem::title() { return myTitle; }
inline std::string &NetworkLibraryItem::cover() { return myCover; }

typedef std::vector<shared_ptr<NetworkLibraryItem> > NetworkLibraryItemList;



class NetworkLibraryCatalogItem : public NetworkLibraryItem {

public:
	static const std::string TYPE_ID;

public:
	NetworkLibraryCatalogItem(
		NetworkLink &link,
		const std::string &url,
		const std::string &htmlURL,
		const std::string &title,
		const std::string &summary,
		const std::string &coverURL,
		bool dependsOnAccount = false
	);

	const std::string &typeId() const;

	NetworkLink &link() const;
	const std::string &url() const;
	const std::string &htmlURL() const;
	const std::string &summary() const;

	// returns error message
	virtual std::string loadChildren(NetworkLibraryItemList &children) = 0;

	bool dependsOnAccount() const;

private:
	NetworkLink &myLink;
	const std::string myURL;
	const std::string myHtmlURL;
	const std::string mySummary;
	const bool myDependsOnAccount;

private: // disable copying
	NetworkLibraryCatalogItem(const NetworkLibraryCatalogItem &);
	const NetworkLibraryCatalogItem &operator = (const NetworkLibraryCatalogItem &);
};

inline NetworkLink &NetworkLibraryCatalogItem::link() const { return myLink; }
inline const std::string &NetworkLibraryCatalogItem::url() const { return myURL; }
inline const std::string &NetworkLibraryCatalogItem::htmlURL() const { return myHtmlURL; }
inline const std::string &NetworkLibraryCatalogItem::summary() const { return mySummary; }
inline bool NetworkLibraryCatalogItem::dependsOnAccount() const { return myDependsOnAccount; }

class NetworkLibraryBookItem : public NetworkLibraryItem {

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
	static const std::string TYPE_ID;

public:
	NetworkLibraryBookItem(const std::string &id, unsigned int index);
	explicit NetworkLibraryBookItem(const NetworkLibraryBookItem &book);

	const std::string &typeId() const;

	//std::string fileName(URLType format) const;
	const std::string &id() const;

public:
	unsigned int index() const;
	void setIndex(unsigned int index);

	const std::string &language() const;
	const std::string &date() const;
	const std::string &seriesName() const;
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
	std::string myLanguage;
	std::string myDate;
	std::string mySeriesName;
	int myIndexInSeries;
	std::string myPrice; // number with curency code (see http://en.wikipedia.org/wiki/List_of_circulating_currencies for example)
	std::string myAnnotation;

	std::map<URLType, std::string> myURLByType;
	std::vector<AuthorData> myAuthors;
	std::vector<std::string> myTags;

	shared_ptr<NetworkAuthenticationManager> myAuthenticationManager;

private: // disable copying
	const NetworkLibraryBookItem &operator = (const NetworkLibraryBookItem &);
};

inline unsigned int NetworkLibraryBookItem::index() const { return myIndex; }
inline void NetworkLibraryBookItem::setIndex(unsigned int index) { myIndex = index; }

inline const std::string &NetworkLibraryBookItem::language() const { return myLanguage; }
inline const std::string &NetworkLibraryBookItem::date() const { return myDate; }
inline const std::string &NetworkLibraryBookItem::seriesName() const { return mySeriesName; }
inline int NetworkLibraryBookItem::indexInSeries() const { return myIndexInSeries; }
inline const std::string &NetworkLibraryBookItem::price() const { return myPrice; }
inline const std::string &NetworkLibraryBookItem::annotation() const { return myAnnotation; }
inline std::string &NetworkLibraryBookItem::annotation() { return myAnnotation; }
inline const std::map<NetworkLibraryBookItem::URLType, std::string> &NetworkLibraryBookItem::urlByType() const { return myURLByType; }
inline const std::vector<NetworkLibraryBookItem::AuthorData> &NetworkLibraryBookItem::authors() const { return myAuthors; }
inline const std::vector<std::string> &NetworkLibraryBookItem::tags() const { return myTags; }
inline std::map<NetworkLibraryBookItem::URLType, std::string> &NetworkLibraryBookItem::urlByType() { return myURLByType; }
inline std::vector<NetworkLibraryBookItem::AuthorData> &NetworkLibraryBookItem::authors() { return myAuthors; }
inline std::vector<std::string> &NetworkLibraryBookItem::tags() { return myTags; }

#endif /* __NETWORKLIBRARYITEMS_H__ */

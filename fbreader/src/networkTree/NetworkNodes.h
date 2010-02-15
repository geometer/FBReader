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

#ifndef __NETWORKNODES_H__
#define __NETWORKNODES_H__

#include "../blockTree/FBReaderNode.h"

#include "../network/NetworkItems.h"

class NetworkBookCollection;
class NetworkLink;
class NetworkAuthenticationManager;

class NetworkContainerNode : public FBReaderNode {

public:
	static const ZLTypeId TYPE_ID;

protected:
	NetworkContainerNode(ZLBlockTreeView::RootNode *parent, size_t atPosition = (size_t)-1);
	NetworkContainerNode(NetworkContainerNode *parent, size_t atPosition = (size_t)-1);

private:
	const ZLTypeId &typeId() const;
};

class NetworkCatalogNode : public NetworkContainerNode {

public:
	static const ZLTypeId TYPE_ID;

private:
	class ExpandCatalogAction;
	class OpenInBrowserAction;
	class ReloadAction;

protected:
	NetworkCatalogNode(ZLBlockTreeView::RootNode *parent, shared_ptr<NetworkItem> item, size_t atPosition = (size_t)-1);
	NetworkCatalogNode(NetworkCatalogNode *parent, shared_ptr<NetworkItem> item, size_t atPosition = (size_t)-1);

private:
	const ZLTypeId &typeId() const;

friend class NetworkNodesFactory;

public:
	NetworkCatalogItem &item();
	const NetworkItem::List &childrenItems();

	void updateChildren();

	shared_ptr<ZLRunnable> expandCatalogAction();
	shared_ptr<ZLRunnable> openInBrowserAction();
	shared_ptr<ZLRunnable> reloadAction();

protected:
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;
	std::string summary() const;
	virtual shared_ptr<ZLImage> lastResortCoverImage() const;
	void paint(ZLPaintContext &context, int vOffset);

	virtual void paintHyperlinks(ZLPaintContext &context, int vOffset);

private:
	shared_ptr<NetworkItem> myItem;
	NetworkItem::List myChildrenItems;
	shared_ptr<ZLRunnable> myExpandCatalogAction;
	shared_ptr<ZLRunnable> myOpenInBrowserAction;
	shared_ptr<ZLRunnable> myReloadAction;
};

class NetworkCatalogRootNode : public NetworkCatalogNode {

public:
	static const ZLTypeId TYPE_ID;

private:
	class LoginAction;
	class LogoutAction;

	class RefillAccountAction : public ZLRunnable {

	public:
		RefillAccountAction(NetworkAuthenticationManager &mgr);
		void run();

	private:
		NetworkAuthenticationManager &myManager;
	};

	class DontShowAction : public ZLRunnable {

	public:
		DontShowAction(NetworkLink &link);
		void run();

	private:
		NetworkLink &myLink;
	};

	class PasswordRecoveryAction : public ZLRunnable {

	public:
		PasswordRecoveryAction(NetworkAuthenticationManager &mgr);
		void run();

	private:
		NetworkAuthenticationManager &myManager;
	};

	class RegisterUserAction : public ZLRunnable {

	public:
		RegisterUserAction(NetworkAuthenticationManager &mgr);
		void run();

	private:
		NetworkAuthenticationManager &myManager;
	};

public:
	NetworkCatalogRootNode(ZLBlockTreeView::RootNode *parent, NetworkLink &link, size_t atPosition = (size_t)-1);

	const NetworkLink &link() const;

private:
	const ZLTypeId &typeId() const;

	void paintHyperlinks(ZLPaintContext &context, int vOffset);
	shared_ptr<ZLImage> lastResortCoverImage() const;
	bool hasAuxHyperlink() const;

private:
	NetworkLink &myLink;
	shared_ptr<ZLRunnable> myLoginAction;
	shared_ptr<ZLRunnable> myLogoutAction;
	shared_ptr<ZLRunnable> myDontShowAction;
	shared_ptr<ZLRunnable> myRefillAccountAction;
	shared_ptr<ZLRunnable> myPasswordRecoveryAction;
	shared_ptr<ZLRunnable> myRegisterUserAction;
};

class SearchResultNode : public NetworkContainerNode {

public:
	static const ZLTypeId TYPE_ID;

public:
	SearchResultNode(ZLBlockTreeView::RootNode *parent, shared_ptr<NetworkBookCollection> searchResult, const std::string &summary, size_t atPosition = (size_t)-1);

	shared_ptr<NetworkBookCollection> searchResult();

private:
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;
	std::string summary() const;
	void paint(ZLPaintContext &context, int vOffset);

private:
	shared_ptr<NetworkBookCollection> mySearchResult;
	std::string mySummary;
};

class NetworkAuthorNode : public NetworkContainerNode {

public:
	static const ZLTypeId TYPE_ID;

protected:
	NetworkAuthorNode(NetworkContainerNode *parent, const NetworkBookItem::AuthorData &author);

friend class NetworkNodesFactory;

public:
	const NetworkBookItem::AuthorData &author();

private:
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;
	void paint(ZLPaintContext &context, int vOffset);

private:
	NetworkBookItem::AuthorData myAuthor;
};

class NetworkSeriesNode : public NetworkContainerNode {

public:
	static const ZLTypeId TYPE_ID;

	enum SummaryType { AUTHORS, BOOKS };

protected:
	NetworkSeriesNode(NetworkContainerNode *parent, const std::string &seriesTitle, SummaryType summaryType);

friend class NetworkNodesFactory;

public:
	const std::string &seriesTitle();

private:
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;
	std::string summary() const;
	void paint(ZLPaintContext &context, int vOffset);

private:
	std::string mySeriesTitle;
	SummaryType mySummaryType;
	mutable std::string mySummary;
};

class NetworkBookNode : public FBReaderNode {

public:
	static const ZLTypeId TYPE_ID;

private:
	class ReadAction;
	class DownloadAction;
	class ReadDemoAction;
	class BuyAction;
	class DeleteAction;

private:
	NetworkBookNode(NetworkContainerNode *parent, shared_ptr<NetworkItem> book);

private:
	void init();

friend class NetworkNodesFactory;

public:
	shared_ptr<NetworkItem> book();

private:
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;
	std::string summary() const;
	void paint(ZLPaintContext &context, int vOffset);

	const NetworkBookItem &bookItem() const;
	NetworkBookItem &bookItem();

	bool hasLocalCopy();
	bool hasDirectLink();
	bool canBePurchased();
	static bool hasLocalCopy(NetworkBookItem &book, NetworkItem::URLType format);

private:
	shared_ptr<NetworkItem> myBook;
	shared_ptr<ZLRunnable> myReadAction;
	shared_ptr<ZLRunnable> myDownloadAction;
	shared_ptr<ZLRunnable> myReadDemoAction;
	shared_ptr<ZLRunnable> myDownloadDemoAction;
	shared_ptr<ZLRunnable> myBuyAction;
	shared_ptr<ZLRunnable> myDeleteAction;
};

inline shared_ptr<NetworkItem> NetworkBookNode::book() { return myBook; }
inline const NetworkBookItem &NetworkBookNode::bookItem() const { return (const NetworkBookItem&)*myBook; }
inline NetworkBookItem &NetworkBookNode::bookItem() { return (NetworkBookItem&)*myBook; }

#endif /* __NETWORKNODES_H__ */

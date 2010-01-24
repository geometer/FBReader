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

#include "../network/NetworkLibraryItems.h"

class NetworkBookCollection;
class NetworkLink;
class NetworkAuthenticationManager;

class NetworkContainerNode : public FBReaderNode {

protected:
	NetworkContainerNode(ZLBlockTreeView::RootNode *parent, size_t atPosition = (size_t)-1);
	NetworkContainerNode(NetworkContainerNode *parent, size_t atPosition = (size_t)-1);
};

class NetworkCatalogNode : public NetworkContainerNode {

public:
	static const std::string TYPE_ID;

private:
	class ExpandCatalogAction;
	class OpenInBrowserAction;
	class ReloadAction;

protected:
	NetworkCatalogNode(ZLBlockTreeView::RootNode *parent, shared_ptr<NetworkLibraryItem> item, size_t atPosition = (size_t)-1);
	NetworkCatalogNode(NetworkCatalogNode *parent, shared_ptr<NetworkLibraryItem> item, size_t atPosition = (size_t)-1);

friend class NetworkNodesFactory;

public:
	NetworkLibraryCatalogItem &item();
	const NetworkLibraryItemList &childrenItems();

	void updateChildren();

	shared_ptr<ZLRunnable> expandCatalogAction();
	shared_ptr<ZLRunnable> openInBrowserAction();
	shared_ptr<ZLRunnable> reloadAction();

protected:
	const std::string &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;
	virtual shared_ptr<ZLImage> lastResortCoverImage() const;
	void paint(ZLPaintContext &context, int vOffset);

	virtual void paintHyperlinks(ZLPaintContext &context, int vOffset);

private:
	shared_ptr<NetworkLibraryItem> myItem;
	NetworkLibraryItemList myChildrenItems;
	shared_ptr<ZLRunnable> myExpandCatalogAction;
	shared_ptr<ZLRunnable> myOpenInBrowserAction;
	shared_ptr<ZLRunnable> myReloadAction;
};

class NetworkCatalogRootNode : public NetworkCatalogNode {

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
	static const std::string TYPE_ID;

public:
	SearchResultNode(ZLBlockTreeView::RootNode *parent, shared_ptr<NetworkBookCollection> searchResult, const std::string &summary, size_t atPosition = (size_t)-1);

	shared_ptr<NetworkBookCollection> searchResult();

private:
	const std::string &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;
	void paint(ZLPaintContext &context, int vOffset);

private:
	shared_ptr<NetworkBookCollection> mySearchResult;
	std::string mySummary;
};

class NetworkAuthorNode : public NetworkContainerNode {

public:
	static const std::string TYPE_ID;

protected:
	NetworkAuthorNode(NetworkContainerNode *parent, const NetworkLibraryBookItem::AuthorData &author);

friend class NetworkNodesFactory;

public:
	const NetworkLibraryBookItem::AuthorData &author();

private:
	const std::string &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;
	void paint(ZLPaintContext &context, int vOffset);

private:
	NetworkLibraryBookItem::AuthorData myAuthor;
};

class NetworkSeriesNode : public NetworkContainerNode {

public:
	static const std::string TYPE_ID;

protected:
	NetworkSeriesNode(NetworkContainerNode *parent, const std::string &seriesTitle);

friend class NetworkNodesFactory;

public:
	const std::string &seriesTitle();

private:
	const std::string &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;
	void paint(ZLPaintContext &context, int vOffset);

private:
	std::string mySeriesTitle;
};

class NetworkBookInfoNode : public FBReaderNode {

public:
	static const std::string TYPE_ID;

private:
	class ReadAction;
	class DownloadAction;
	class ReadDemoAction;
	class BuyAction;
	class DeleteAction;

private:
	NetworkBookInfoNode(NetworkContainerNode *parent, shared_ptr<NetworkLibraryItem> book);

private:
	void init();

friend class NetworkNodesFactory;

public:
	shared_ptr<NetworkLibraryItem> book();

private:
	const std::string &typeId() const;
	shared_ptr<ZLImage> extractCoverImage() const;
	std::string title() const;
	void paint(ZLPaintContext &context, int vOffset);

	const NetworkLibraryBookItem &bookItem() const;
	NetworkLibraryBookItem &bookItem();

	bool hasLocalCopy();
	bool hasDirectLink();
	bool canBePurchased();
	static bool hasLocalCopy(NetworkLibraryBookItem &book, NetworkLibraryBookItem::URLType format);

private:
	shared_ptr<NetworkLibraryItem> myBook;
	shared_ptr<ZLRunnable> myReadAction;
	shared_ptr<ZLRunnable> myDownloadAction;
	shared_ptr<ZLRunnable> myReadDemoAction;
	shared_ptr<ZLRunnable> myDownloadDemoAction;
	shared_ptr<ZLRunnable> myBuyAction;
	shared_ptr<ZLRunnable> myDeleteAction;
};

inline shared_ptr<NetworkLibraryItem> NetworkBookInfoNode::book() { return myBook; }
inline const NetworkLibraryBookItem &NetworkBookInfoNode::bookItem() const { return (const NetworkLibraryBookItem&)*myBook; }
inline NetworkLibraryBookItem &NetworkBookInfoNode::bookItem() { return (NetworkLibraryBookItem&)*myBook; }

#endif /* __NETWORKNODES_H__ */

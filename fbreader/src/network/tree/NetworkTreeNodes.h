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

#ifndef __NETWORKTREENODES_H__
#define __NETWORKTREENODES_H__

#include <ZLResource.h>
#include <ZLExecutionUtil.h>

#include <ZLTreeNode.h>
#include <ZLTreeDialog.h>
#include <ZLTreePageNode.h>

#include "../NetworkLink.h"
#include "../../tree/FBTree.h"

//maybe RootTree should be nested class for NetworkLibrary?
class RootTree : public ZLTreeNode {

public:
	static const ZLTypeId TYPE_ID;

private:
	const ZLTypeId &typeId() const;

public:
	RootTree();
	void setDialog(shared_ptr<ZLTreeDialog> dialog);


protected:
	ZLTreeListener *listener() const;

private:
	shared_ptr<ZLTreeDialog> myListener;
};

class NetworkTree : public FBTree {

public:
	static const ZLTypeId TYPE_ID;

private:
	const ZLTypeId &typeId() const;

public:
	NetworkTree(RootTree *parent, std::size_t position);
	NetworkTree(NetworkTree *parent, std::size_t position = (std::size_t)-1);
};

class NetworkCatalogTree : public NetworkTree {

public:
	static const ZLTypeId TYPE_ID;

private:
	const ZLTypeId &typeId() const;

public:
	NetworkCatalogTree(RootTree *parent, shared_ptr<NetworkItem> item, std::size_t position = (std::size_t)-1);
	NetworkCatalogTree(NetworkCatalogTree *parent, shared_ptr<NetworkItem> item, std::size_t position = (std::size_t)-1);

	std::string title() const;
	std::string subtitle() const;
	shared_ptr<const ZLImage> image() const;

	void requestChildren(shared_ptr<ZLNetworkRequest::Listener> listener);
	void requestMoreChildren(shared_ptr<ZLNetworkRequest::Listener> listener);
	virtual void onChildrenReceived(NetworkItem::List &childrens, const std::string &error);

	NetworkCatalogItem &item();

private:
	void init();
	void notifyListeners(const std::string &error);

private:
	void onAuthCheck(ZLUserDataHolder &data, const std::string &error);
	void onInitialization(ZLUserDataHolder &data, const std::string &error);

private:
	const ZLResource &resource() const;

private:
	shared_ptr<NetworkItem> myItem;
	NetworkItem::List myChildrenItems;

	std::vector<shared_ptr<ZLNetworkRequest::Listener> > myListeners;
	mutable shared_ptr<const ZLImage> myImage;

friend class NetworkTreeFactory;
};

class NetworkCatalogRootTree : public NetworkCatalogTree {

public:
	static const ZLTypeId TYPE_ID;

private:
	const ZLTypeId &typeId() const;

private:
	class LoginAction;
	class LogoutAction;
	class RefillAccountAction;
	class PasswordRecoveryAction;
	class RegisterUserAction;

public:
	NetworkCatalogRootTree(RootTree *parent, NetworkLink &link, std::size_t position);
	void init();

private:
	const ZLResource &resource() const;

private:
	NetworkLink &myLink;
};

class SearchCatalogTree : public NetworkCatalogTree {

public:
	static const ZLTypeId TYPE_ID;

private:
	const ZLTypeId &typeId() const;

public:
	SearchCatalogTree(RootTree *parent, shared_ptr<NetworkItem> item, std::size_t position = (std::size_t)-1);

	void requestChildren(shared_ptr<ZLNetworkRequest::Listener> listener);
	void onChildrenReceived(NetworkItem::List &childrens, const std::string &error);
};

class NetworkAuthorTree : public NetworkTree {

public:
	static const ZLTypeId TYPE_ID;

private:
	const ZLTypeId &typeId() const;

protected:
	NetworkAuthorTree(NetworkTree *parent, const NetworkBookItem::AuthorData &author);

friend class NetworkTreeFactory;

public:
	const NetworkBookItem::AuthorData &author();

private:
	void init();
	const ZLResource &resource() const;
	shared_ptr<const ZLImage> image() const;
	std::string title() const;

private:
	NetworkBookItem::AuthorData myAuthor;
};

class NetworkSeriesTree : public NetworkTree {

public:
	static const ZLTypeId TYPE_ID;

private:
	const ZLTypeId &typeId() const;

public:
	enum SummaryType { AUTHORS, BOOKS };

protected:
	NetworkSeriesTree(NetworkTree *parent, const std::string &seriesTitle, SummaryType summaryType);

friend class NetworkTreeFactory;

private:
	void init();
	const ZLResource &resource() const;
	shared_ptr<const ZLImage> image() const;
	std::string title() const;
	std::string subtitle() const;

private:
	std::string mySeriesTitle;
	SummaryType mySummaryType;
	mutable std::string mySummary;
};

class NetworkBookTree : public ZLTreePageNode {

public:
	static const ZLTypeId TYPE_ID;

private:
	const ZLTypeId &typeId() const;

public:
	enum SummaryType { AUTHORS, NONE };

private:
	NetworkBookTree(NetworkTree *parent, shared_ptr<NetworkItem> book, SummaryType summaryType);
	void init();

friend class NetworkTreeFactory;

public:
	const NetworkBookItem &book() const;

public:
	const ZLResource &resource() const;
	shared_ptr<const ZLImage> image() const;
	shared_ptr<const ZLImage> fullImage() const;
	std::string title() const;
	std::string subtitle() const;

	shared_ptr<ZLTreePageInfo> getPageInfo();

private:
	class BookItemWrapper : public ZLTreePageInfo {

	public:
		BookItemWrapper(NetworkBookTree &tree, shared_ptr<NetworkItem> bookItem);

		bool isPageInfoLoaded();
		void loadAll(shared_ptr<ZLNetworkRequest::Listener> listener);
		void onInformationLoaded(ZLUserDataHolder &data, const std::string &error);
		void onCoverLoaded(ZLUserDataHolder &data, const std::string &error);

	public:
		std::string title() const;
		std::vector<std::string> authors() const;
		std::vector<std::string> tags() const;
		std::string summary() const;
		shared_ptr<const ZLImage> image() const;

		//TODO maybe store actions in other place?
		const std::vector<shared_ptr<ZLTreeAction> > &actions() const;
		std::string actionText(const shared_ptr<ZLTreeAction> &action) const;
		const std::vector<shared_ptr<ZLTreeAction> > relatedActions() const;

	private:
		NetworkBookItem &book() const;

	private:
		NetworkBookTree &myTree;
		shared_ptr<NetworkItem> myBookItem;
		mutable bool myIsInitialized;

		mutable std::vector<shared_ptr<ZLTreeAction> > myRelatedActions;
	};

private:
	shared_ptr<NetworkItem> myBook;
	SummaryType mySummaryType;
	mutable shared_ptr<const ZLImage> myImage;
	shared_ptr<ZLTreePageInfo> myPageInfo;
};

#endif /* __NETWORKTREENODES_H__ */

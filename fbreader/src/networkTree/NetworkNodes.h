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

#include "../tree/FBNode.h"

#include <ZLTreeTitledNode.h>
#include <ZLTreePageNode.h>
#include <ZLTreeActionNode.h>
#include <ZLTreeListener.h>

#include "../network/NetworkItems.h"

class NetworkBookCollection;
class NetworkLink;
class LoadSubCatalogRunnable;

class NetworkContainerNode : public ZLTreeTitledNode {

public:
	static const ZLTypeId TYPE_ID;

protected:
	NetworkContainerNode();
	~NetworkContainerNode();

private:
	const ZLTypeId &typeId() const;
};

class NetworkCatalogNode : public NetworkContainerNode {

public:
	static const ZLTypeId TYPE_ID;

protected:

	class ReloadAction : public ZLTreeAction {

	public:
		ReloadAction(NetworkCatalogNode &node);
		ZLResourceKey key() const;
		bool makesSense() const;
		void run();

	private:
		NetworkCatalogNode &myNode;
	};

	class OpenInBrowserAction;

protected:
	NetworkCatalogNode(shared_ptr<NetworkItem> item);
	~NetworkCatalogNode();
	
	void requestChildren(shared_ptr<ZLExecutionData::Listener> listener = 0);

private:
	void init();
	const ZLResource &resource() const;
	const ZLTypeId &typeId() const;

friend class NetworkNodesFactory;

public:
	NetworkCatalogItem &item();
	const NetworkItem::List &childrenItems();

	void updateChildren(shared_ptr<ZLExecutionData::Listener> listener);

protected:
	void onChildrenReceived(LoadSubCatalogRunnable *runnable);
	shared_ptr<ZLImage> image() const;
	std::string imageUrl() const;
	std::string title() const;
	std::string subtitle() const;
	virtual shared_ptr<ZLImage> lastResortCoverImage() const;

private:
	friend class LoadSubCatalogRunnable;
	shared_ptr<NetworkItem> myItem;
	NetworkItem::List myChildrenItems;
	std::vector<shared_ptr<ZLExecutionData::Listener> > myListeners;
};

class NetworkCatalogRootNode : public NetworkCatalogNode {

public:
	static const ZLTypeId TYPE_ID;

private:
	class LoginAction;
	class LogoutAction;
	class RefillAccountAction;
	class DontShowAction;
	class PasswordRecoveryAction;
	class RegisterUserAction;

public:
	NetworkCatalogRootNode(ZLTreeListener::RootNode *parent, NetworkLink &link, size_t atPosition = (size_t)-1);

	const NetworkLink &link() const;

private:
	void init();
	const ZLResource &resource() const;
	const ZLTypeId &typeId() const;

	shared_ptr<ZLImage> lastResortCoverImage() const;

private:
	NetworkLink &myLink;
};

class SearchResultNode : public NetworkContainerNode {

public:
	static const ZLTypeId TYPE_ID;

public:
	SearchResultNode(ZLTreeListener::RootNode *parent, shared_ptr<NetworkBookCollection> searchResult, const std::string &subtitle, size_t atPosition = (size_t)-1);

	shared_ptr<NetworkBookCollection> searchResult();

private:
	void init();
	const ZLResource &resource() const;
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> image() const;
	std::string imageUrl() const;
	std::string title() const;
	std::string subtitle() const;

private:
	shared_ptr<NetworkBookCollection> mySearchResult;
	std::string mySummary;
};

class NetworkAuthorNode : public NetworkContainerNode {

public:
	static const ZLTypeId TYPE_ID;

protected:
	NetworkAuthorNode(NetworkContainerNode *parent, const NetworkBookItem::AuthorData &author);
	NetworkAuthorNode(const NetworkBookItem::AuthorData &author);

friend class NetworkNodesFactory;

public:
	const NetworkBookItem::AuthorData &author();

private:
	void init();
	const ZLResource &resource() const;
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> image() const;
	std::string imageUrl() const;
	std::string title() const;

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

private:
	void init();
	const ZLResource &resource() const;
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> image() const;
	std::string imageUrl() const;
	std::string title() const;
	std::string subtitle() const;

private:
	std::string mySeriesTitle;
	SummaryType mySummaryType;
	mutable std::string mySummary;
};

class NetworkBookNode : public ZLTreeActionNode {

public:
	static const ZLTypeId TYPE_ID;

	enum SummaryType { AUTHORS, NONE };

private:
	NetworkBookNode(NetworkContainerNode *parent, shared_ptr<NetworkItem> book, SummaryType summaryType);
	void init();

friend class NetworkNodesFactory;

public:
	const NetworkBookItem &book() const;

private:
	const ZLResource &resource() const;
	const ZLTypeId &typeId() const;
	shared_ptr<ZLImage> image() const;
	std::string imageUrl() const;
	std::string title() const;
	std::string subtitle() const;
	bool activate();

private:
	shared_ptr<NetworkItem> myBook;
	SummaryType mySummaryType;
};

#endif /* __NETWORKNODES_H__ */

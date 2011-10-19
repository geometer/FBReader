/*
 * Copyright (C) 2008-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __NETWORKOPERATIONRUNNABLE_H__
#define __NETWORKOPERATIONRUNNABLE_H__

#include <string>

#include <ZLRunnable.h>
#include <ZLBoolean3.h>
#include <ZLNetworkSSLCertificate.h>

#include "../network/NetworkItems.h"
#include "../network/NetworkBookCollection.h"

class ZLProgressDialog;

class NetworkAuthenticationManager;
class NetworkCatalogNode;

class NetworkOperationRunnable : public ZLRunnable, public ZLExecutionData::Listener {

public:
	~NetworkOperationRunnable();
	
	static void showErrorMessage(const std::string &message);
	static bool tryConnect();

protected:
//	NetworkOperationRunnable(const std::string &uiMessageKey);
	NetworkOperationRunnable();
	
	void destroy();
	void showPercent(int ready, int full);

public:
	void executeWithUI();
	bool hasErrors() const;
	void showErrorMessage() const;
	const std::string &errorMessage() const;

protected:
	std::string myErrorMessage;
	shared_ptr<ZLExecutionData::Listener> myListenerHolder;
	shared_ptr<ZLRunnable> myRunnableHolder;
	shared_ptr<ZLProgressDialog> myDialog;
};

class DownloadBookRunnable;

class DownloadBookListener {

public:
	virtual ~DownloadBookListener();
	
	virtual void bookDownloadingProgress(DownloadBookRunnable *downloader, int downloaded, int size) = 0;
	virtual void bookDownloaded(DownloadBookRunnable *runnable) = 0;
};

class DownloadBookRunnable : public NetworkOperationRunnable {

public:
	DownloadBookRunnable(shared_ptr<BookReference> reference, shared_ptr<NetworkAuthenticationManager> authManager);
	DownloadBookRunnable(const std::string &url);
	~DownloadBookRunnable();
	void setListener(DownloadBookListener *listener);
	void run();
	void finished(const std::string &error = std::string());

	shared_ptr<BookReference> reference() const;
	const std::string &fileName() const;

private:
	shared_ptr<BookReference> myReference;
	shared_ptr<NetworkAuthenticationManager> myAuthManager;
	std::string myFileName;
	DownloadBookListener *myListener;
};

class LogOutRunnable : public NetworkOperationRunnable {

public:
	LogOutRunnable(NetworkAuthenticationManager &mgr, shared_ptr<ZLExecutionData::Listener> listener);
	void run();
	void finished(const std::string &error = std::string());

private:
	NetworkAuthenticationManager &myManager;
	shared_ptr<ZLExecutionData::Listener> myListener;
};


class SearchRunnable : public NetworkOperationRunnable {

protected:
	SearchRunnable();

public:
	shared_ptr<NetworkBookCollection> result();

protected:
	shared_ptr<NetworkBookCollection> mySearchResult;
};

inline shared_ptr<NetworkBookCollection> SearchRunnable::result() { return mySearchResult; }


class SimpleSearchRunnable : public SearchRunnable {

public:
	SimpleSearchRunnable(const std::string &pattern);
	void run();

private:
	const std::string myPattern;
};


class AdvancedSearchRunnable : public SearchRunnable {

public:
	AdvancedSearchRunnable(const std::string &titleAndSeries, const std::string &author, const std::string &category, const std::string &description);
	void run();

private:
	const std::string myTitleAndSeries;
	const std::string myAuthor;
	const std::string myCategory;
	const std::string myDescription;
};


class LoadSubCatalogRunnable : public NetworkOperationRunnable {

public:
	LoadSubCatalogRunnable(NetworkCatalogNode *node);
	
	inline NetworkItem::List children() const { return myChildren; }
	
protected:
	void run();
	void finished(const std::string &error = std::string());

private:
	NetworkCatalogNode *myNode;
	NetworkItem::List myChildren;
};

#endif /* __NETWORKOPERATIONRUNNABLE_H__ */

/*
 * Copyright (C) 2008-2009 Geometer Plus <contact@geometerplus.com>
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

#ifndef __NETLIBRARYVIEW_H__
#define __NETLIBRARYVIEW_H__

#include <ZLResource.h>

#include "FBView.h"

#include "../network/NetworkBookInfo.h"

class SearchRunnable;

class NetLibraryView : public FBView {

private:
	static const std::string SEARCH_PARAMETER_ID;
	static const std::string DownloadMobi;
	static const std::string ReadLocalMobi;
	static const std::string DownloadEpub;
	static const std::string ReadLocalEpub;
	static const std::string OpenInBrowser;

public:
	NetLibraryView(FBReader &reader, shared_ptr<ZLPaintContext> context);
	~NetLibraryView();
	const std::string &caption() const;

	void search();
	void search(const std::string &title, const std::string &author, const std::string &series, const std::string &category, const std::string &description);

private:
	bool hasContents() const;

	bool _onStylusPress(int x, int y);
	bool _onStylusMove(int x, int y);

	void search(SearchRunnable &runnable);

	void addBookIcon(ZLTextTreeModel &model, const NetworkBookInfo &book, NetworkBookInfo::URLType format, const std::string &localIcon, const std::string &downloadIcon);
	void rebuildModel();

private:
	const ZLResource &myResource;
	ZLImageMap myImageMap;
	NetworkBookList myBookList;
	std::map<int,shared_ptr<NetworkBookInfo> > myParagraphToBookMap;
};

#endif /* __NETLIBRARYVIEW_H__ */

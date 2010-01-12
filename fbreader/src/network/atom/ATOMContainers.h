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

#ifndef __ATOMCONTAINTERS_H__
#define __ATOMCONTAINTERS_H__

#include <vector>

#include <shared_ptr.h>

#include "ATOMMetadata.h"



class ATOMEntry : public ATOMCommonAttributes {

public:
	ATOMEntry();
	ATOMEntry(shared_ptr<ATOMId> id, const std::string &title, shared_ptr<ATOMUpdated> updated);

	std::vector<shared_ptr<ATOMAuthor> > &authors() { return myAuthors; }
	std::vector<shared_ptr<ATOMCategory> > &categories() { return myCategories; }
	std::vector<shared_ptr<ATOMContributor> > &contributors() { return myContributors; }
	std::vector<shared_ptr<ATOMLink> > &links() { return myLinks; }

	shared_ptr<ATOMPublished> published() { return myPublished; }
	const std::string &rights() { return myRights; }
	const std::string &summary() { return mySummary; }
	const std::string &title() { return myTitle; }
	shared_ptr<ATOMUpdated> updated() { return myUpdated; }

	void setPublished(shared_ptr<ATOMPublished> published) { myPublished = published; }
	void setRights(const std::string &rights) { myRights = rights; }
	void setSummary(const std::string &summary) { mySummary = summary; }
	void setTitle(const std::string &title) { myTitle = title; }
	void setUpdated(shared_ptr<ATOMUpdated> updated) { myUpdated = updated; }
	
	shared_ptr<ATOMId> id() { return myId; }
	void setId(shared_ptr<ATOMId> id) { myId = id; }

private:
	shared_ptr<ATOMId> myId;

	std::vector<shared_ptr<ATOMAuthor> > myAuthors;
	std::vector<shared_ptr<ATOMCategory> > myCategories;
	//shared_ptr<ATOMContent> myContent; TODO: implement ATOMContent
	std::vector<shared_ptr<ATOMContributor> > myContributors;
	std::vector<shared_ptr<ATOMLink> > myLinks;
	shared_ptr<ATOMPublished> myPublished;
	std::string myRights;  // TODO: implement ATOMTextConstruct
	//shared_ptr<ATOMSource> mySource; // TODO: implement ATOMSource
	std::string mySummary; // TODO: implement ATOMTextConstruct
	std::string myTitle;   // TODO: implement ATOMTextConstruct
	shared_ptr<ATOMUpdated> myUpdated;
};


class ATOMFeedMetadata : public ATOMCommonAttributes {

public:
	ATOMFeedMetadata();
	ATOMFeedMetadata(shared_ptr<ATOMId> id, const std::string &title, shared_ptr<ATOMUpdated> updated);

	std::vector<shared_ptr<ATOMAuthor> > &authors() { return myAuthors; }
	std::vector<shared_ptr<ATOMCategory> > &categories() { return myCategories; }
	std::vector<shared_ptr<ATOMContributor> > &contributors() { return myContributors; }
	std::vector<shared_ptr<ATOMLink> > &links() { return myLinks; }

	shared_ptr<ATOMGenerator> generator() { return myGenerator; }
	shared_ptr<ATOMIcon> icon() { return myIcon; }
	shared_ptr<ATOMLogo> logo() { return myLogo; }
	const std::string &rights() { return myRights; }
	const std::string &subtitle() { return mySubtitle; }
	const std::string &title() { return myTitle; }
	shared_ptr<ATOMUpdated> updated() { return myUpdated; }

	void setGenerator(shared_ptr<ATOMGenerator> generator) { myGenerator = generator; }
	void setIcon(shared_ptr<ATOMIcon> icon) { myIcon = icon; }
	void setLogo(shared_ptr<ATOMLogo> logo) { myLogo = logo; }
	void setRights(const std::string &rights) { myRights = rights; }
	void setSubtitle(const std::string &subtitle) { mySubtitle = subtitle; }
	void setTitle(const std::string &title) { myTitle = title; }
	void setUpdated(shared_ptr<ATOMUpdated> updated) { myUpdated = updated; }
	
	shared_ptr<ATOMId> id() { return myId; }
	void setId(shared_ptr<ATOMId> id) { myId = id; }

private:
	shared_ptr<ATOMId> myId;

	std::vector<shared_ptr<ATOMAuthor> > myAuthors;
	std::vector<shared_ptr<ATOMCategory> > myCategories;
	std::vector<shared_ptr<ATOMContributor> > myContributors;
	shared_ptr<ATOMGenerator> myGenerator;
	shared_ptr<ATOMIcon> myIcon;
	std::vector<shared_ptr<ATOMLink> > myLinks;
	shared_ptr<ATOMLogo> myLogo;
	std::string myRights;   // TODO: implement ATOMTextConstruct
	std::string mySubtitle; // TODO: implement ATOMTextConstruct
	std::string myTitle;    // TODO: implement ATOMTextConstruct
	shared_ptr<ATOMUpdated> myUpdated;
};


/*
class ATOMFeed : public ATOMFeedMetadata {

public:
	ATOMFeed();
	ATOMFeed(const ATOMId &id, const std::string &title, shared_ptr<ATOMUpdated> updated);

	std::vector<shared_ptr<ATOMEntry> > entries() { return myEntries; }

private:
	std::vector<shared_ptr<ATOMEntry> > myEntries;
};
*/


#endif /* __ATOMCONTAINTERS_H__ */

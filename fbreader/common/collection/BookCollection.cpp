/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <queue>
#include <algorithm>

#include <abstract/ZLStringUtil.h>
#include <abstract/ZLFSDir.h>
#include <abstract/ZLZipInputStream.h>

#include "BookCollection.h"
#include "../description/BookDescription.h"
#include "../description/Author.h"

ZLBooleanOption BookCollection::ScanSubdirsOption("Options", "ScanSubdirs", false);

bool DescriptionComparator::operator() (const BookDescription *d1, const BookDescription *d2) {
	return d1->title() < d2->title();
}

BookCollection::BookCollection() {
	myPath = PathOption.value();
	myScanSubdirs = ScanSubdirsOption.value();

	std::set<std::string> dirs;
	collectDirNames(dirs);

	for (std::set<std::string>::iterator it = dirs.begin(); it != dirs.end(); it++) {
		ZLFSDir dir(*it);
		if (dir.open()) {
			std::vector<std::string> files;
			dir.collectRegularFiles(files);
			dir.close();
			for (std::vector<std::string>::const_iterator jt = files.begin(); jt != files.end(); jt++) {
				if (ZLStringUtil::stringEndsWith(*jt, ".fb2") || ZLStringUtil::stringEndsWith(*jt, ".xml")) {
					addDescription(BookDescription::create(dir.name() + '/' + *jt));
				} else if (ZLStringUtil::stringEndsWith(*jt, ".zip")) {
					std::list<ZLZipEntry> entries = ZLZipEntry::entriesList(dir.name() + '/' + *jt);
					for (std::list<ZLZipEntry>::iterator zit = entries.begin(); zit != entries.end(); zit++) {
						const std::string &name = zit->name();
						if (ZLStringUtil::stringEndsWith(name, ".fb2") || ZLStringUtil::stringEndsWith(name, ".xml")) {
							addDescription(BookDescription::create(zit->name()));
						}
					}
				}
			}
		}
	}
	std::sort(myAuthors.begin(), myAuthors.end(), AuthorComparator());
	DescriptionComparator descriptionComparator;
	for (std::map<const Author*,Books>::iterator it = myCollection.begin(); it != myCollection.end(); it++) {
		std::sort(it->second.begin(), it->second.end(), descriptionComparator);
	}
}

bool BookCollection::isActual() const {
	return
		(myScanSubdirs == ScanSubdirsOption.value()) &&
		(myPath == PathOption.value());
}

void BookCollection::collectDirNames(std::set<std::string> &nameSet) {
	std::queue<std::string> nameQueue;

	std::string path = myPath;
	int pos = path.find(':');
	while (pos != -1) {
		nameQueue.push(path.substr(0, pos));
		path.erase(0, pos + 1);
		pos = path.find(':');
	}
	if (!path.empty()) {
		nameQueue.push(path);
	}

	while (!nameQueue.empty()) {
		std::string name = nameQueue.front();
		nameQueue.pop();
		if (nameSet.find(name) == nameSet.end()) {
			if (myScanSubdirs) {
				ZLFSDir dir(name);
				if (dir.open()) {
					std::vector<std::string> subdirs;
					dir.collectSubDirs(subdirs);
					dir.close();
					for (std::vector<std::string>::const_iterator it = subdirs.begin(); it != subdirs.end(); it++) {
						nameQueue.push(dir.name() + '/' + *it);
					}
				}
			}
			nameSet.insert(name);
		}
	}
}

BookCollection::~BookCollection() {
	for (std::map<const Author*,Books>::iterator it = myCollection.begin(); it != myCollection.end(); it++) {
		for (Books::iterator jt = it->second.begin(); jt != it->second.end(); jt++) {
			delete *jt;
		}
	}
}

void BookCollection::addDescription(BookDescription *description) {
	if (description == 0) {
		return;
	}

	const Author *author = description->author();
	const std::string &displayName = author->displayName();
	const std::string &sortKey = author->sortKey();

	std::map<const Author*,Books>::iterator it = myCollection.begin();
	for (; it != myCollection.end(); it++) {
		const Author *author1 = it->first;
		if ((author1->sortKey() == sortKey) && (author1->displayName() == displayName)) {
			break;
		}
	}
	if (it != myCollection.end()) {
		it->second.push_back(description);
	} else {
		Books books;
		books.push_back(description);
		myCollection.insert(std::pair<const Author*,Books>(author, books));
		myAuthors.push_back(author);
	}
}

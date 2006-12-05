/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <queue>
#include <algorithm>

#include <ZLStringUtil.h>
#include <ZLFile.h>
#include <ZLDir.h>

#include "BookCollection.h"
#include "BookList.h"
#include "../description/BookDescription.h"
#include "../description/BookDescriptionUtil.h"
#include "../description/Author.h"
#include "../formats/FormatPlugin.h"

bool DescriptionComparator::operator() (const BookDescriptionPtr d1, const BookDescriptionPtr d2) {
	const std::string &sequenceName1 = d1->sequenceName();
	const std::string &sequenceName2 = d2->sequenceName();
	if (sequenceName1.empty() && sequenceName2.empty()) {
		return d1->title() < d2->title();
	}
	if (sequenceName1.empty()) {
		return d1->title() < sequenceName2;
	}
	if (sequenceName2.empty()) {
		return sequenceName1 <= d2->title();
	}
	if (sequenceName1 != sequenceName2) {
		return sequenceName1 < sequenceName2;
	}
	return d1->numberInSequence() < d2->numberInSequence();
}

static const std::string OPTIONS = "Options";

BookCollection::BookCollection() :
	PathOption(ZLOption::CONFIG_CATEGORY, OPTIONS, "BookPath", ""),
	ScanSubdirsOption(ZLOption::CONFIG_CATEGORY, OPTIONS, "ScanSubdirs", false),
	myDoStrongRebuild(true),
	myDoWeakRebuild(false) {
}

void BookCollection::rebuild(bool strong) {
	if (strong) {
		myDoStrongRebuild = true;
	} else {
		myDoWeakRebuild = true;
	}
}

void BookCollection::collectBookFileNames(std::set<std::string> &bookFileNames) const {
	std::set<std::string> dirs;
	collectDirNames(dirs);

	for (std::set<std::string>::iterator it = dirs.begin(); it != dirs.end(); ++it) {
		std::vector<std::string> files;
		shared_ptr<ZLDir> dir = ZLFile(*it).directory();
		if (dir.isNull()) {
			continue;
		}
		dir->collectFiles(files, false);
		if (!files.empty()) {
			const std::string dirName = dir->name() + '/';
			for (std::vector<std::string>::const_iterator jt = files.begin(); jt != files.end(); ++jt) {
				const std::string fileName = dirName + *jt;
				ZLFile file(fileName);
				if (PluginCollection::instance().plugin(file, true) != 0) {
					bookFileNames.insert(fileName);
				} else if (file.extension() == "zip") {
					if (!BookDescriptionUtil::checkInfo(file)) {
						BookDescriptionUtil::resetZipInfo(file);
						BookDescriptionUtil::saveInfo(file);
					}
					std::vector<std::string> zipEntries;
					BookDescriptionUtil::listZipEntries(file, zipEntries);
					for (std::vector<std::string>::const_iterator zit = zipEntries.begin(); zit != zipEntries.end(); ++zit) {
						bookFileNames.insert(*zit);
					}
				}
			}
		}
	}
}

void BookCollection::synchronize() const {
	bool doStrongRebuild =
		myDoStrongRebuild ||
		(myScanSubdirs != ScanSubdirsOption.value()) ||
		(myPath != PathOption.value());

	if (!doStrongRebuild && !myDoWeakRebuild) {
		return;
	}

	myPath = PathOption.value();
	myScanSubdirs = ScanSubdirsOption.value();
	myDoWeakRebuild = false;
	myDoStrongRebuild = false;

	if (doStrongRebuild) {
		myAuthors.clear();
		myCollection.clear();
		myExternalBooks.clear();

		std::set<std::string> fileNamesSet;
		collectBookFileNames(fileNamesSet);
		for (std::set<std::string>::iterator it = fileNamesSet.begin(); it != fileNamesSet.end(); ++it) {
			addDescription(BookDescription::getDescription(*it));
		}

		BookList bookList;
		const std::set<std::string> &bookListSet = bookList.fileNames();
		for (std::set<std::string>::const_iterator it = bookListSet.begin(); it != bookListSet.end(); ++it) {
			if (fileNamesSet.find(*it) == fileNamesSet.end()) {
				BookDescriptionPtr description = BookDescription::getDescription(*it);
				if (!description.isNull()) {
					addDescription(description);
					myExternalBooks.insert(description);
				}
			}
		}
	} else {
		std::vector<std::string> fileNames;
		for (std::map<AuthorPtr,Books>::const_iterator it = myCollection.begin(); it != myCollection.end(); ++it) {
			const Books &books = it->second;
			for (Books::const_iterator jt = books.begin(); jt != books.end(); ++jt) {
				fileNames.push_back((*jt)->fileName());
			}
		}
		myCollection.clear();
		myAuthors.clear();
		for (std::vector<std::string>::iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
			addDescription(BookDescription::getDescription(*it, false));
		}
	}

	std::sort(myAuthors.begin(), myAuthors.end(), AuthorComparator());
	DescriptionComparator descriptionComparator;
	for (std::map<AuthorPtr,Books>::iterator it = myCollection.begin(); it != myCollection.end(); ++it) {
		std::sort((*it).second.begin(), (*it).second.end(), descriptionComparator);
	}
}

void BookCollection::collectDirNames(std::set<std::string> &nameSet) const {
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
				shared_ptr<ZLDir> dir = ZLFile(name).directory();
				if (!dir.isNull()) {
					std::vector<std::string> subdirs;
					dir->collectSubDirs(subdirs, false);
					for (std::vector<std::string>::const_iterator it = subdirs.begin(); it != subdirs.end(); ++it) {
						nameQueue.push(dir->name() + '/' + *it);
					}
				}
			}
			nameSet.insert(name);
		}
	}
}

BookCollection::~BookCollection() {
}

void BookCollection::addDescription(BookDescriptionPtr description) const {
	if (description.isNull()) {
		return;
	}

	AuthorPtr author = description->author();
	const std::string &displayName = author->displayName();
	const std::string &sortKey = author->sortKey();

	std::map<AuthorPtr,Books>::iterator it = myCollection.begin();
	for (; it != myCollection.end(); ++it) {
		AuthorPtr author1 = (*it).first;
		if ((author1->sortKey() == sortKey) && (author1->displayName() == displayName)) {
			break;
		}
	}
	if (it != myCollection.end()) {
		(*it).second.push_back(description);
	} else {
		Books books;
		books.push_back(description);
		myCollection.insert(std::pair<AuthorPtr,Books>(author, books));
		myAuthors.push_back(author);
	}
}

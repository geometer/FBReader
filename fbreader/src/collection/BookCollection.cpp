/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLibrary.h>
#include <ZLStringUtil.h>
#include <ZLFile.h>
#include <ZLDir.h>

#include "BookCollection.h"
#include "BookList.h"
#include "../description/BookDescription.h"
#include "../description/BookDescriptionUtil.h"
#include "../description/Author.h"
#include "../formats/FormatPlugin.h"

class DescriptionComparator {

public:
	bool operator() (const BookDescriptionPtr d1, const BookDescriptionPtr d2);
};

bool DescriptionComparator::operator() (const BookDescriptionPtr d1, const BookDescriptionPtr d2) {
	AuthorPtr author1 = d1->author();
	AuthorPtr author2 = d2->author();
	const std::string sortKey1 = author1->sortKey();
	const std::string sortKey2 = author2->sortKey();
	if (sortKey1 != sortKey2) {
		return sortKey1 < sortKey2;
	}
	const std::string displayName1 = author1->displayName();
	const std::string displayName2 = author2->displayName();
	if (displayName1 != displayName2) {
		return displayName1 < displayName2;
	}

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
	PathOption(ZLCategoryKey::CONFIG, OPTIONS, "BookPath", ""),
	ScanSubdirsOption(ZLCategoryKey::CONFIG, OPTIONS, "ScanSubdirs", false),
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
			for (std::vector<std::string>::const_iterator jt = files.begin(); jt != files.end(); ++jt) {
				const std::string fileName = dir->itemPath(*jt);
				ZLFile file(fileName);
				if (PluginCollection::instance().plugin(file, true) != 0) {
					bookFileNames.insert(fileName);
				// TODO: zip -> any archive
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

bool BookCollection::synchronize() const {
	bool doStrongRebuild =
		myDoStrongRebuild ||
		(myScanSubdirs != ScanSubdirsOption.value()) ||
		(myPath != PathOption.value());

	if (!doStrongRebuild && !myDoWeakRebuild) {
		return false;
	}

	myPath = PathOption.value();
	myScanSubdirs = ScanSubdirsOption.value();
	myDoWeakRebuild = false;
	myDoStrongRebuild = false;

	if (doStrongRebuild) {
		myBooks.clear();
		myAuthors.clear();
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
		BookList bookList;
		const std::set<std::string> &bookListSet = bookList.fileNames();
		std::vector<std::string> fileNames;
		for (Books::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
			if ((myExternalBooks.find(*it) == myExternalBooks.end()) || 
					(bookListSet.find((*it)->fileName()) != bookListSet.end())) {
				fileNames.push_back((*it)->fileName());
			}
		}
		myBooks.clear();
		myAuthors.clear();
		for (std::vector<std::string>::iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
			addDescription(BookDescription::getDescription(*it, false));
		}
	}

	std::sort(myBooks.begin(), myBooks.end(), DescriptionComparator());
	return true;
}

void BookCollection::collectDirNames(std::set<std::string> &nameSet) const {
	std::queue<std::string> nameQueue;

	std::string path = myPath;
	int pos = path.find(ZLibrary::PathDelimiter);
	while (pos != -1) {
		nameQueue.push(path.substr(0, pos));
		path.erase(0, pos + 1);
		pos = path.find(ZLibrary::PathDelimiter);
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
						nameQueue.push(dir->itemPath(*it));
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
	if (!description.isNull()) {
		myBooks.push_back(description);
	}
}

void BookCollection::collectSequenceNames(AuthorPtr author, std::set<std::string> &set) const {
	synchronize();
	if (myBooks.empty()) {
		return;
	}
	AuthorComparator comparator;
	Books::const_iterator left = myBooks.begin();
	if (comparator(author, (*left)->author())) {
		return;
	}
	Books::const_iterator right = myBooks.end() - 1;
	if (comparator((*right)->author(), author)) {
		return;
	}
	while (right > left) {
		Books::const_iterator middle = left + (right - left) / 2;
		if (comparator((*middle)->author(), author)) {
			left = middle + 1;
		} else if (comparator(author, (*middle)->author())) {
			right = middle;
		} else {
			for (Books::const_iterator it = middle; !comparator((*it)->author(), author); --it) {
				set.insert((*it)->sequenceName());
				if (it == left) {
					break;
				}
			}
			for (Books::const_iterator it = middle; !comparator(author, (*it)->author()); ++it) {
				set.insert((*it)->sequenceName());
				if (it == right) {
					break;
				}
			}
			break;
		}
	}
}

const std::vector<AuthorPtr> &BookCollection::authors() const {
	synchronize();
	if (myAuthors.empty() && !myBooks.empty()) {
		AuthorPtr author;
		for (Books::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
			AuthorPtr newAuthor = (*it)->author();
			if (author.isNull() || (author->sortKey() != newAuthor->sortKey()) || (author->displayName() != newAuthor->displayName())) {
				author = newAuthor;
				myAuthors.push_back(author);
			}
		}
	}
	return myAuthors;
}

void BookCollection::removeTag(const std::string &tag, bool includeSubTags) {
	synchronize();
	for (Books::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
		WritableBookDescription wbd(**it);
		wbd.removeTag(tag, includeSubTags);
	}
}

void BookCollection::renameTag(const std::string &from, const std::string &to) {
	synchronize();
	for (Books::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
		WritableBookDescription wbd(**it);
		wbd.renameTag(from, to);
	}
}

void BookCollection::cloneTag(const std::string &from, const std::string &to) {
	synchronize();
	for (Books::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
		WritableBookDescription wbd(**it);
		wbd.cloneTag(from, to);
	}
}

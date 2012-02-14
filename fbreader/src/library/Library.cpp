/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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
#include <ZLLogger.h>

#include <ZLibrary.h>
#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>
#include <ZLFile.h>
#include <ZLDir.h>
#include <ZLDialogManager.h>
#include <ZLInputStream.h>

#include "Library.h"
#include "Book.h"
#include "Author.h"
#include "Tag.h"
#include "../fbreader/FBReader.h"

#include "../formats/FormatPlugin.h"

#include "../database/booksdb/BooksDBUtil.h"
#include "../database/booksdb/BooksDB.h"

shared_ptr<Library> Library::ourInstance;
const size_t Library::MaxRecentListSize = 10;

Library &Library::Instance() {
	if (ourInstance.isNull()) {
		ourInstance = new Library();
	}
	return *ourInstance;
}

static const std::string OPTIONS = "Options";

Library::Library() :
	PathOption(ZLCategoryKey::CONFIG, OPTIONS, "BookPath", ""),
	ScanSubdirsOption(ZLCategoryKey::CONFIG, OPTIONS, "ScanSubdirs", false),
	CollectAllBooksOption(ZLCategoryKey::CONFIG, OPTIONS, "CollectAllBooks", false),
    myWatcher(new Watcher(*this)),
	myBuildMode(BUILD_ALL),
	myRevision(0) {
	BooksDBUtil::getRecentBooks(myRecentBooks);
}

#include <ctime>
#include <iostream>

struct FunctionTimer
{
	FunctionTimer(const char *f) : funcName(f), start(clock()) {
		std::cerr << "Entered " << funcName << std::endl;
		std::cerr.flush();
	}
	~FunctionTimer() {
		std::cerr << funcName << " finished at "
		          << (double(clock() - start) * 1000. / CLOCKS_PER_SEC) << " ms"
		          << std::endl;
		std::cerr.flush();
	}

	const char *funcName;
	clock_t start;
};

void Library::collectBookFileNames(std::set<std::string> &bookFileNames, std::vector<shared_ptr<ZLInputStream> > &inputStreamCache) const {
	FunctionTimer timer(__PRETTY_FUNCTION__);
	std::set<std::string> dirs;
	collectDirNames(dirs);

	while (!dirs.empty()) {
		std::string dirname = *dirs.begin();
		dirs.erase(dirs.begin());
		
		ZLFile dirfile(dirname);
		std::vector<std::string> files;
		bool inZip = false;

		shared_ptr<ZLDir> dir = dirfile.directory();
		if (dir.isNull()) {
			continue;
		}
		
		if (!dirfile.isDirectory())
			inputStreamCache.push_back(dirfile.inputStream());

		if (dirfile.extension() == "zip") {
			ZLFile phys(dirfile.physicalFilePath());
			if (!BooksDBUtil::checkInfo(phys)) {
				BooksDBUtil::resetZipInfo(phys);
				BooksDBUtil::saveInfo(phys);
			}
			BooksDBUtil::listZipEntries(dirfile, files);
			inZip = true;
		} else {
			dir->collectFiles(files, true);
		}
		std::cerr.precision(5);
		if (!files.empty()) {
			const bool collectBookWithoutMetaInfo = CollectAllBooksOption.value();
			for (std::vector<std::string>::const_iterator jt = files.begin(); jt != files.end(); ++jt) {
				const std::string fileName = (inZip) ? (*jt) : (dir->itemPath(*jt));
				ZLFile file(fileName);
//				std::cerr << "Check file \"" << fileName << "\" ... ";
//				std::cerr.flush();
//				clock_t start = clock();
				if (PluginCollection::Instance().plugin(file, !collectBookWithoutMetaInfo) != 0) {
					bookFileNames.insert(fileName);
				// TODO: zip -> any archive
				} else if (file.extension() == "zip") {
					if (myScanSubdirs || !inZip) {
						dirs.insert(fileName);
					}
				}
//				std::cerr << (double(clock() - start) * 1000. / CLOCKS_PER_SEC) << " ms" << std::endl;
//				std::cerr.flush();
			}
		}
	}
}

void Library::rebuildBookSet() const {
	FunctionTimer timer(__PRETTY_FUNCTION__);
	myBooks.clear();
	myExternalBooks.clear();
	
	std::map<std::string, shared_ptr<Book> > booksMap;
	BooksDBUtil::getBooks(booksMap);

	std::set<std::string> fileNamesSet;
	std::vector<shared_ptr<ZLInputStream> > inputStreamCache;
	collectBookFileNames(fileNamesSet, inputStreamCache);

	// collect books from book path
	for (std::set<std::string>::iterator it = fileNamesSet.begin(); it != fileNamesSet.end(); ++it) {
		std::map<std::string, shared_ptr<Book> >::iterator jt = booksMap.find(*it);
		if (jt == booksMap.end()) {
			std::cerr << "Check file \"" << (*it) << "\" ... ";
			std::cerr.flush();
			clock_t start = clock();
			insertIntoBookSet(BooksDBUtil::getBook(*it));
			std::cerr << (double(clock() - start) * 1000. / CLOCKS_PER_SEC) << " ms" << std::endl;
			std::cerr.flush();
		} else {
			insertIntoBookSet(jt->second);
			booksMap.erase(jt);
		}
	}

	// other books from our database
	for (std::map<std::string, shared_ptr<Book> >::iterator jt = booksMap.begin(); jt != booksMap.end(); ++jt) {
		shared_ptr<Book> book = jt->second;
		if (!book.isNull()) {
			if (BooksDB::Instance().checkBookList(*book)) {
				insertIntoBookSet(book);
				myExternalBooks.insert(book);
			}
		}
	}
	
	// help file
	shared_ptr<Book> help = FBReader::Instance().helpFile();
	if (!help.isNull()) {
		insertIntoBookSet(help);
		myExternalBooks.insert(help);
	}
}

size_t Library::revision() const {
	if (myBuildMode == BUILD_NOTHING &&
			(myScanSubdirs != ScanSubdirsOption.value() ||
			 myPath != PathOption.value())) {
		myPath = PathOption.value();
		myScanSubdirs = ScanSubdirsOption.value();
		myBuildMode = BUILD_ALL;
	}

	return (myBuildMode == BUILD_NOTHING) ? myRevision : myRevision + 1;
}

Library::Watcher::Watcher(Library &library) : myLibrary(library) {
}

void Library::Watcher::onPathChanged(const std::string &) {
	myLibrary.myBuildMode = Library::BUILD_ALL;
}

class LibrarySynchronizer : public ZLRunnable {

public:
	LibrarySynchronizer(Library::BuildMode mode);

private:
	void run();

private:
	const Library::BuildMode myBuildMode;
};

LibrarySynchronizer::LibrarySynchronizer(Library::BuildMode mode) : myBuildMode(mode) {
}

void LibrarySynchronizer::run() {
	FunctionTimer timer(__PRETTY_FUNCTION__);
	Library &library = Library::Instance();

	if (myBuildMode & Library::BUILD_COLLECT_FILES_INFO) {
		library.rebuildBookSet();
	}

	if (myBuildMode & Library::BUILD_UPDATE_BOOKS_INFO) {
		library.rebuildMaps();
	}
}

void Library::synchronize() const {
	if (myScanSubdirs != ScanSubdirsOption.value() ||
			myPath != PathOption.value()) {
                std::vector<std::string> oldPathes;
                ZLStringUtil::split(myPath, oldPathes, ZLibrary::PathDelimiter);
                for (size_t i = 0; i < oldPathes.size(); ++i) {
                    ZLFSWatcher::removeWatcher(oldPathes.at(i), myWatcher);
                }
		myPath = PathOption.value();
		myScanSubdirs = ScanSubdirsOption.value();
                std::vector<std::string> pathes;
                ZLStringUtil::split(myPath, pathes, ZLibrary::PathDelimiter);
                for (size_t i = 0; i < pathes.size(); ++i) {
                    ZLFSWatcher::addWatcher(pathes.at(i), myWatcher);
                }
		myBuildMode = BUILD_ALL;
	}

	if (myBuildMode == BUILD_NOTHING) {
		return;
	}

	LibrarySynchronizer synchronizer(myBuildMode);
	myBuildMode = BUILD_NOTHING;
	ZLDialogManager::Instance().wait(ZLResourceKey("loadingBookList"), synchronizer);

	++myRevision;
}

void Library::rebuildMaps() const {
	FunctionTimer timer(__PRETTY_FUNCTION__);
	myAuthors.clear();
	myBooksByAuthor.clear();
	myTags.clear();
	myBooksByTag.clear();
        myFirstLetters.clear();
        myBooksByFirstLetter.clear();

	for (BookSet::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
		if ((*it).isNull()) {
			continue;
		}

		const AuthorList &bookAuthors = (*it)->authors();
		if (bookAuthors.empty()) {
			myBooksByAuthor[0].push_back(*it);
		} else {
			for(AuthorList::const_iterator jt = bookAuthors.begin(); jt != bookAuthors.end(); ++jt) {
				myBooksByAuthor[*jt].push_back(*it);
			}
		}

		const TagList &bookTags = (*it)->tags();
		if (bookTags.empty()) {
			myBooksByTag[0].push_back(*it);
		} else {
			for(TagList::const_iterator kt = bookTags.begin(); kt != bookTags.end(); ++kt) {
				myBooksByTag[*kt].push_back(*it);
			}
		}

                std::string firstLetter = getFirstTitleLetter((*it)->title());
                if (!firstLetter.empty()) {
                    myBooksByFirstLetter[firstLetter].push_back(*it);
                }

	}
	for (BooksByAuthor::iterator mit = myBooksByAuthor.begin(); mit != myBooksByAuthor.end(); ++mit) {
		myAuthors.push_back(mit->first);
		std::sort(mit->second.begin(), mit->second.end(), BookComparator());
	}
	for (BooksByTag::iterator mjt = myBooksByTag.begin(); mjt != myBooksByTag.end(); ++mjt) {
		myTags.push_back(mjt->first);
		std::sort(mjt->second.begin(), mjt->second.end(), BookComparator());
	}
        for (BooksByFirstLetter::iterator mit = myBooksByFirstLetter.begin(); mit != myBooksByFirstLetter.end(); ++mit) {
                myFirstLetters.push_back(mit->first);
                std::sort(mit->second.begin(), mit->second.end(), BookComparator());
        }
}

void Library::collectDirNames(std::set<std::string> &nameSet) const {
	FunctionTimer timer(__PRETTY_FUNCTION__);
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

	std::set<std::string> resolvedNameSet;
	while (!nameQueue.empty()) {
		std::string name = nameQueue.front();
		nameQueue.pop();
		ZLFile f(name);
		const std::string resolvedName = f.resolvedPath();
		if (resolvedNameSet.find(resolvedName) == resolvedNameSet.end()) {
			if (myScanSubdirs) {
				shared_ptr<ZLDir> dir = f.directory();
				if (!dir.isNull()) {
					std::vector<std::string> subdirs;
					dir->collectSubDirs(subdirs, true);
					for (std::vector<std::string>::const_iterator it = subdirs.begin(); it != subdirs.end(); ++it) {
						nameQueue.push(dir->itemPath(*it));
					}
				}
			}
			resolvedNameSet.insert(resolvedName);
			nameSet.insert(name);
		}
	}
}

void Library::updateBook(shared_ptr<Book> book) {
	BooksDB::Instance().saveBook(book);
	myBuildMode = (BuildMode)(myBuildMode | BUILD_UPDATE_BOOKS_INFO);
}

void Library::addBook(shared_ptr<Book> book) {
	if (!book.isNull()) {
		BooksDB::Instance().saveBook(book);
		insertIntoBookSet(book);
		myBuildMode = (BuildMode)(myBuildMode | BUILD_UPDATE_BOOKS_INFO);
	}
}

void Library::removeBook(shared_ptr<Book> book) {
	if (!book.isNull()) {
		BookSet::iterator it = myBooks.find(book);
		if (it != myBooks.end()) {
			myBooks.erase(it);
			myBuildMode = (BuildMode)(myBuildMode | BUILD_UPDATE_BOOKS_INFO);
		}
		BooksDB::Instance().deleteFromBookList(*book);
		bool recentListChanged = false;
		for (BookList::iterator it = myRecentBooks.begin(); it != myRecentBooks.end();) {
			if ((*it)->file() == book->file()) {
				it = myRecentBooks.erase(it);
				recentListChanged = true;
			} else {
				++it;
			}
		}
		if (recentListChanged) {
			BooksDB::Instance().saveRecentBooks(myRecentBooks);
		}
	}
}

const AuthorList &Library::authors() const {
	synchronize();
	return myAuthors;
}

const TagList &Library::tags() const {
	synchronize();
	return myTags;
}

const FirstLetterList &Library::firstLetterTitles() const {
        synchronize();
        return myFirstLetters;
}

const BookList &Library::books(shared_ptr<Author> author) const {
	synchronize();
	return myBooksByAuthor[author];
}

const BookList &Library::books(shared_ptr<Tag> tag) const {
	synchronize();
	return myBooksByTag[tag];
}

const BookList &Library::books(std::string firstLetter) const {
        synchronize();
        return myBooksByFirstLetter[firstLetter];
}

std::string Library::getFirstTitleLetter(std::string bookTitle) {
    const static std::string EMPTY_STRING = "";
    ZLStringUtil::stripWhiteSpaces(bookTitle);
    if (bookTitle.empty()) {
        return EMPTY_STRING;
    }
    if (!ZLUnicodeUtil::isUtf8String(bookTitle)) {
        ZLLogger::Instance().println(ZLLogger::DEFAULT_CLASS, "Library::getFirstTitleLetter   Warning! bookTitle is not in UTF-8");
        return EMPTY_STRING;
    }
    ZLUnicodeUtil::Ucs4String ucs4Title;
    ZLUnicodeUtil::utf8ToUcs4(ucs4Title,bookTitle);
    if (ucs4Title.empty()) {
        return EMPTY_STRING;
    }

    std::string result = EMPTY_STRING;
    for (size_t i=0; i < ucs4Title.size(); ++i) {
        ZLUnicodeUtil::Ucs4Char ch = ucs4Title.at(i);
        if (ZLUnicodeUtil::isLetter(ch) || ZLUnicodeUtil::isDigit(ch)) {
            ZLUnicodeUtil::ucs4ToUtf8(result, ZLUnicodeUtil::Ucs4String(1,ZLUnicodeUtil::toUpper(ch)));
            return result;
        }
    }
    ZLUnicodeUtil::ucs4ToUtf8(result, ZLUnicodeUtil::Ucs4String(1,ZLUnicodeUtil::toUpper(ucs4Title.at(0))));
    return result;
}

void Library::collectSeriesTitles(shared_ptr<Author> author, std::set<std::string> &titles) const {
	const BookList &bookList = books(author);
	for (BookList::const_iterator it = bookList.begin(); it != bookList.end(); ++it) {
		const std::string &current = (*it)->seriesTitle();
		if (!current.empty()) {
			titles.insert(current);
		}
	}
}

void Library::removeTag(shared_ptr<Tag> tag, bool includeSubTags) {
	for (BookSet::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
		if ((*it)->removeTag(tag, includeSubTags)) {
			BooksDB::Instance().saveTags(*it);
		}
	}
	myBuildMode = (BuildMode)(myBuildMode | BUILD_UPDATE_BOOKS_INFO);
}

void Library::renameTag(shared_ptr<Tag> from, shared_ptr<Tag> to, bool includeSubTags) {
	if (to != from) {
		synchronize();
		for (BookSet::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
			BooksDBUtil::renameTag(*it, from, to, includeSubTags);
		}
	}
	myBuildMode = (BuildMode)(myBuildMode | BUILD_UPDATE_BOOKS_INFO);
}

void Library::cloneTag(shared_ptr<Tag> from, shared_ptr<Tag> to, bool includeSubTags) {
	if (to != from) {
		synchronize();
		for (BookSet::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
			BooksDBUtil::cloneTag(*it, from, to, includeSubTags);
		}
	}
	myBuildMode = (BuildMode)(myBuildMode | BUILD_UPDATE_BOOKS_INFO);
}

bool Library::hasBooks(shared_ptr<Tag> tag) const {
	synchronize();
	const BooksByTag::const_iterator it = myBooksByTag.find(tag);
	return it != myBooksByTag.end() && !it->second.empty();
}

bool Library::hasSubtags(shared_ptr<Tag> tag) const {
	const TagList &tagList = tags();
	const TagList::const_iterator it =
		std::upper_bound(tagList.begin(), tagList.end(), tag, TagComparator());
	return it != tagList.end() && tag->isAncestorOf(*it);
}

void Library::replaceAuthor(shared_ptr<Author> from, shared_ptr<Author> to) {
	if (to != from) {
		for (BookSet::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
			if ((*it)->replaceAuthor(from, to)) {
				BooksDB::Instance().saveAuthors(*it);
			}
		}
		myBuildMode = (BuildMode)(myBuildMode | BUILD_UPDATE_BOOKS_INFO);
	}
}

Library::RemoveType Library::canRemove(shared_ptr<Book> book) const {
	synchronize();
	return (RemoveType)(
		(myExternalBooks.find(book) != myExternalBooks.end() ?
			REMOVE_FROM_LIBRARY : REMOVE_DONT_REMOVE) |
		(BooksDBUtil::canRemoveFile(book->file().path()) ?
			REMOVE_FROM_DISK : REMOVE_DONT_REMOVE)
	);
}

void Library::insertIntoBookSet(shared_ptr<Book> book) const {
	if (!book.isNull()) {
		myBooks.insert(book);
	}
}

const BookList &Library::recentBooks() const {
	return myRecentBooks;
}

void Library::addBookToRecentList(shared_ptr<Book> book) {
	if (book.isNull()) {
		return;
	}
	for (BookList::iterator it = myRecentBooks.begin(); it != myRecentBooks.end(); ++it) {
		if ((*it)->file() == book->file()) {
			if (it == myRecentBooks.begin()) {
				return;
			}
			myRecentBooks.erase(it);
			break;
		}
	}
	myRecentBooks.insert(myRecentBooks.begin(), book);
	if (myRecentBooks.size() > MaxRecentListSize) {
		myRecentBooks.erase(myRecentBooks.begin() + MaxRecentListSize, myRecentBooks.end());
	}
	BooksDB::Instance().saveRecentBooks(myRecentBooks);
}


const BookList Library::searchBooks(std::string pattern) /*const*/ {
    //TODO make this method as const
    BookList bookList;
    for (BookSet::const_iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
        if ((*it).isNull()) {
                continue;
        }
        shared_ptr<Book> book = *it;
        if (book->matches(pattern)) {
            bookList.push_back(book);
        }
    }
    return bookList;
}

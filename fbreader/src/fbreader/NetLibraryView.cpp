/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
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

#include <algorithm>

#include <ZLibrary.h>
#include <ZLFileImage.h>
#include <ZLFile.h>
#include <ZLDir.h>

#include "NetLibraryView.h"
#include "CollectionView.h"
#include "FBReader.h"

#include "../bookmodel/FBTextKind.h"

#include "../network/NetworkLink.h"

const std::string NetLibraryView::SEARCH_PARAMETER_ID = "networkSearchPattern";

const std::string NetLibraryView::DownloadMobi = "downloadMobi";
const std::string NetLibraryView::ReadLocalMobi = "readLocalMobi";
const std::string NetLibraryView::DownloadEpub = "downloadEpub";
const std::string NetLibraryView::ReadLocalEpub = "readLocalEpub";
const std::string NetLibraryView::OpenInBrowser = "openInBrowser";

NetLibraryView::NetLibraryView(FBReader &reader, shared_ptr<ZLPaintContext> context) : FBView(reader, context), myResource(ZLResource::resource("networkLibrary")) {
	const std::string prefix = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter;
	myImageMap[DownloadMobi] = new ZLFileImage("image/png", prefix + "tree-download-mobi.png", 0);
	myImageMap[ReadLocalMobi] = new ZLFileImage("image/png", prefix + "tree-local-mobi.png", 0);
	myImageMap[DownloadEpub] = new ZLFileImage("image/png", prefix + "tree-download-epub.png", 0);
	myImageMap[ReadLocalEpub] = new ZLFileImage("image/png", prefix + "tree-local-epub.png", 0);
	myImageMap[OpenInBrowser] = new ZLFileImage("image/png", prefix + "tree-open-in-browser.png", 0);

	ZLTextPlainModel *model = new ZLTextPlainModel(8192);
	model->createParagraph(ZLTextParagraph::TEXT_PARAGRAPH);
	model->addControl(LIBRARY_ENTRY, true);
	model->addText(myResource["startText"].value());
	setModel(model, ZLibrary::Language());
}

const std::string &NetLibraryView::caption() const {
	return myResource["caption"].value();
}

NetLibraryView::~NetLibraryView() {
}

class SimpleSearchRunnable : public ZLRunnable {

public:
	SimpleSearchRunnable(const std::string &pattern, NetworkBookList &bookList);
	void run();

private:
	const std::string myPattern;
	NetworkBookList &myBookList;
};

SimpleSearchRunnable::SimpleSearchRunnable(const std::string &pattern, NetworkBookList &bookList) : myPattern(pattern), myBookList(bookList) {
}

void SimpleSearchRunnable::run() {
	NetworkLinkCollection::instance().simpleSearch(myBookList, myPattern);
}

class AdvancedSearchRunnable : public ZLRunnable {

public:
	AdvancedSearchRunnable(const std::string &title, const std::string &author, const std::string &series, const std::string &category, const std::string &description, NetworkBookList &bookList);
	void run();

private:
	const std::string myTitle;
	const std::string myAuthor;
	const std::string mySeries;
	const std::string myCategory;
	const std::string myDescription;
	NetworkBookList &myBookList;
};

AdvancedSearchRunnable::AdvancedSearchRunnable(const std::string &title, const std::string &author, const std::string &series, const std::string &category, const std::string &description, NetworkBookList &bookList) : myTitle(title), myAuthor(author), mySeries(series), myCategory(category), myDescription(description), myBookList(bookList) {
}

void AdvancedSearchRunnable::run() {
	NetworkLinkCollection::instance().advancedSearch(myBookList, myTitle, myAuthor, mySeries, myCategory, myDescription);
}

void NetLibraryView::rebuildModel() {
	ZLTextTreeModel *resultsModel = new ZLTextTreeModel();
	myParagraphToBookMap.clear();

	std::map<NetworkBookInfo::AuthorData,NetworkBookList> bookMap;
	for (NetworkBookList::const_iterator it = myBookList.begin(); it != myBookList.end(); ++it) {
		bookMap[(*it)->Author].push_back(*it);
	}

	for (std::map<NetworkBookInfo::AuthorData,NetworkBookList>::iterator jt = bookMap.begin(); jt != bookMap.end(); ++jt) {
		ZLTextTreeParagraph *authorParagraph = resultsModel->createParagraph();
		resultsModel->addControl(LIBRARY_ENTRY, true);
		resultsModel->addText(jt->first.DisplayName);
		std::sort(jt->second.begin(), jt->second.end(), NetworkBookInfo::Comparator());
		std::string seriesName;
		ZLTextTreeParagraph *para = authorParagraph;
		for (NetworkBookList::const_iterator kt = jt->second.begin(); kt != jt->second.end(); ++kt) {
			const NetworkBookInfo &book = **kt;
			if (book.Series.empty()) {
				para = authorParagraph;
			} else if (book.Series != seriesName) {
				para = resultsModel->createParagraph(authorParagraph);
				resultsModel->addControl(LIBRARY_ENTRY, true);
				resultsModel->addText(book.Series);
			}
			myParagraphToBookMap[resultsModel->paragraphsNumber()] = *kt;
			seriesName = book.Series;
			resultsModel->createParagraph(para);
			resultsModel->addControl(LIBRARY_ENTRY, true);
			resultsModel->addText(book.Title);
			if (ZLFile(fileName(book, NetworkBookInfo::BOOK_EPUB)).exists()) {
				resultsModel->addFixedHSpace(1);
				resultsModel->addImage(ReadLocalEpub, myImageMap, 0);
			} else if (book.URLByType.find(NetworkBookInfo::BOOK_EPUB) != book.URLByType.end()) {
				resultsModel->addFixedHSpace(1);
				resultsModel->addImage(DownloadEpub, myImageMap, 0);
			}
			if (ZLFile(fileName(book, NetworkBookInfo::BOOK_MOBIPOCKET)).exists()) {
				resultsModel->addFixedHSpace(1);
				resultsModel->addImage(ReadLocalMobi, myImageMap, 0);
			} else if (book.URLByType.find(NetworkBookInfo::BOOK_MOBIPOCKET) != book.URLByType.end()) {
				resultsModel->addFixedHSpace(1);
				resultsModel->addImage(DownloadMobi, myImageMap, 0);
			}
			if (book.URLByType.find(NetworkBookInfo::LINK_HTTP) != book.URLByType.end()) {
				resultsModel->addFixedHSpace(1);
				resultsModel->addImage(OpenInBrowser, myImageMap, 0);
			}
		}
	}
	setModel(resultsModel, ZLibrary::Language());
}

void NetLibraryView::search(ZLRunnable &runnable) {
	myBookList.clear();
	ZLDialogManager::instance().wait(ZLResourceKey("downloadBookList"), runnable);

	if (!myBookList.empty()) {
		rebuildModel();
	} else {
		ZLTextPlainModel *resultsModel = new ZLTextPlainModel(8192);
		resultsModel->createParagraph(ZLTextParagraph::TEXT_PARAGRAPH);
		resultsModel->addControl(LIBRARY_ENTRY, true);
		resultsModel->addText(myResource["notFound"].value());
		setModel(resultsModel, ZLibrary::Language());
	}
}

void NetLibraryView::search(const std::string &title, const std::string &author, const std::string &series, const std::string &category, const std::string &description) {
	AdvancedSearchRunnable runnable(title, author, series, category, description, myBookList);
	search(runnable);
}

void NetLibraryView::search() {
	const std::string pattern = fbreader().visualParameter(NetLibraryView::SEARCH_PARAMETER_ID);

	if (pattern.empty()) {
		return;
	}

	SimpleSearchRunnable runnable(pattern, myBookList);
	search(runnable);
}

class DownloadBookRunnable : public ZLRunnable {

public:
	DownloadBookRunnable(const NetworkBookInfo &book, NetworkBookInfo::URLType format, const std::string &fileName);
	void run();
	bool wasSuccessful();

private:
	const NetworkBookInfo &myBook;
	const NetworkBookInfo::URLType myFormat;
	const std::string myFileName;
	bool myWasSuccessful;
};

DownloadBookRunnable::DownloadBookRunnable(const NetworkBookInfo &book, NetworkBookInfo::URLType format, const std::string &fileName) : myBook(book), myFormat(format), myFileName(fileName), myWasSuccessful(false) {
}

void DownloadBookRunnable::run() {
	myWasSuccessful = NetworkLinkCollection::instance().downloadBook(myBook, myFormat, myFileName);
}

bool DownloadBookRunnable::wasSuccessful() {
	return myWasSuccessful;
}

std::string NetLibraryView::fileName(const NetworkBookInfo &book, NetworkBookInfo::URLType format) const {
	ZLFile dirFile(NetworkLinkCollection::instance().DirectoryOption.value());
	dirFile.directory(true);
	return NetworkLinkCollection::instance().DirectoryOption.value() + ZLibrary::FileNameDelimiter + book.fileName(format);
}

bool NetLibraryView::_onStylusPress(int x, int y) {
	fbreader().setHyperlinkCursor(false);

	if (model().isNull()) {
		return false;
	}

	const ZLTextElementArea *imageArea = elementByCoordinates(x, y);
	if ((imageArea == 0) || (imageArea->Kind != ZLTextElement::IMAGE_ELEMENT)) {
		return false;
	}

	ZLTextWordCursor cursor = startCursor();
	cursor.moveToParagraph(imageArea->ParagraphIndex);
	cursor.moveTo(imageArea->ElementIndex, 0);
	const ZLTextElement &element = cursor.element();
	if (element.kind() != ZLTextElement::IMAGE_ELEMENT) {
		return false;
	}
	const ZLTextImageElement &imageElement = (ZLTextImageElement&)element;

	const std::string &id = imageElement.id();

	shared_ptr<NetworkBookInfo> book = myParagraphToBookMap[imageArea->ParagraphIndex];
	if (book.isNull()) {
		return false;
	}

	if ((id == DownloadEpub) || (id == DownloadMobi)) {
		NetworkBookInfo::URLType format = (id == DownloadEpub) ? NetworkBookInfo::BOOK_EPUB : NetworkBookInfo::BOOK_MOBIPOCKET;
		const std::string fName = fileName(*book, format);
		DownloadBookRunnable runnable(*book, format, fName);
		ZLDialogManager::instance().wait(ZLResourceKey("downloadBook"), runnable);
		if (runnable.wasSuccessful()) {
			BookDescriptionPtr description = BookDescription::getDescription(fName);
			WritableBookDescription wDescription(*description);
			wDescription.clearAuthor();
			wDescription.addAuthor(book->Author.DisplayName, book->Author.SortKey);
			wDescription.title() = book->Title;
			wDescription.language() = book->Language;
			for (std::vector<std::string>::const_iterator it = book->Tags.begin(); it != book->Tags.end(); ++it) {
				wDescription.addTag(*it);
			}
			wDescription.saveInfo();

			fbreader().openBook(description);
			fbreader().setMode(FBReader::BOOK_TEXT_MODE);
			rebuildModel();
		} else {
			ZLDialogManager::instance().errorBox(ZLResourceKey("downloadError"));
		}
		return true;
	} else if ((id == ReadLocalEpub) || (id == ReadLocalMobi)) {
		NetworkBookInfo::URLType format = (id == DownloadEpub) ? NetworkBookInfo::BOOK_EPUB : NetworkBookInfo::BOOK_MOBIPOCKET;
		fbreader().openFile(fileName(*book, format));
		fbreader().setMode(FBReader::BOOK_TEXT_MODE);
		return true;
	} else if (id == OpenInBrowser) {
		std::string url = book->URLByType[NetworkBookInfo::LINK_HTTP];
		shared_ptr<ProgramCollection> collection = fbreader().webBrowserCollection();
		if (!url.empty() && !collection.isNull()) {
			shared_ptr<Program> program = collection->currentProgram();
			if (!program.isNull()) {
				program->run("openLink", url);
			}
		}
		return true;
	}
	return false;
}

bool NetLibraryView::onStylusMove(int x, int y) {
	if (FBView::onStylusMove(x, y)) {
		return true;
	}

	if (!model().isNull()) {
		const ZLTextElementArea *imageArea = elementByCoordinates(x, y);
		if ((imageArea != 0) && (imageArea->Kind == ZLTextElement::IMAGE_ELEMENT)) {
			fbreader().setHyperlinkCursor(true);
			return true;
		}
	}

	fbreader().setHyperlinkCursor(false);
	return false;
}

bool NetLibraryView::hasContents() const {
	return !myBookList.empty();
}

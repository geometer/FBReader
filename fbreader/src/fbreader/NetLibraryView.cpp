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

#include <algorithm>

#include <ZLibrary.h>
#include <ZLFileImage.h>
#include <ZLFile.h>
#include <ZLDir.h>
#include <ZLNetworkManager.h>

#include "NetLibraryView.h"
#include "CollectionView.h"
#include "FBReader.h"
#include "NetworkOperationRunnable.h"

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

class SearchRunnable : public NetworkOperationRunnable {

protected:
	SearchRunnable();

public:
	const NetworkBookList &bookList() const;

protected:
	NetworkBookList myBookList;
};

SearchRunnable::SearchRunnable() : NetworkOperationRunnable("downloadBookList") {
}

const NetworkBookList &SearchRunnable::bookList() const {
	return myBookList;
}

class SimpleSearchRunnable : public SearchRunnable {

public:
	SimpleSearchRunnable(const std::string &pattern);
	void run();

private:
	const std::string myPattern;
};

SimpleSearchRunnable::SimpleSearchRunnable(const std::string &pattern) : myPattern(pattern) {
}

void SimpleSearchRunnable::run() {
	myErrorMessage = NetworkLinkCollection::instance().simpleSearch(myBookList, myPattern);
}

class AdvancedSearchRunnable : public SearchRunnable {

public:
	AdvancedSearchRunnable(const std::string &title, const std::string &author, const std::string &series, const std::string &category, const std::string &description);
	void run();

private:
	const std::string myTitle;
	const std::string myAuthor;
	const std::string mySeries;
	const std::string myCategory;
	const std::string myDescription;
};

AdvancedSearchRunnable::AdvancedSearchRunnable(const std::string &title, const std::string &author, const std::string &series, const std::string &category, const std::string &description) : myTitle(title), myAuthor(author), mySeries(series), myCategory(category), myDescription(description) {
}

void AdvancedSearchRunnable::run() {
	myErrorMessage = NetworkLinkCollection::instance().advancedSearch(myBookList, myTitle, myAuthor, mySeries, myCategory, myDescription);
}

void NetLibraryView::addBookIcon(ZLTextTreeModel &model, const NetworkBookInfo &book, NetworkBookInfo::URLType format, const std::string &localIcon, const std::string &downloadIcon) {
	const std::map<NetworkBookInfo::URLType,std::string>::const_iterator it = book.URLByType.find(format);
	if (it == book.URLByType.end()) {
		return;
	}

	const std::string &fileName = NetworkLinkCollection::instance().bookFileName(it->second);
	const std::string &icon = (!fileName.empty() && ZLFile(fileName).exists()) ? localIcon : downloadIcon;
	model.addFixedHSpace(1);
	model.addImage(icon, myImageMap, 0);
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
			ZLTextTreeParagraph *bookPara = resultsModel->createParagraph(para);
			resultsModel->addControl(LIBRARY_ENTRY, true);
			resultsModel->addText(book.Title);
			addBookIcon(*resultsModel, book, NetworkBookInfo::BOOK_EPUB, ReadLocalEpub, DownloadEpub);
			addBookIcon(*resultsModel, book, NetworkBookInfo::BOOK_MOBIPOCKET, ReadLocalMobi, DownloadMobi);
			if (book.URLByType.find(NetworkBookInfo::LINK_HTTP) != book.URLByType.end()) {
				resultsModel->addFixedHSpace(1);
				resultsModel->addImage(OpenInBrowser, myImageMap, 0);
			}
			if (book.Annotation.length() > 6) {
				resultsModel->createParagraph(bookPara);
				//resultsModel->addControl(LIBRARY_ENTRY, true);
				int start = 0;
				int end;
				while ((end = book.Annotation.find('<', start)) >= 0) {
					resultsModel->addText(book.Annotation.substr(start, end - start));
					start = book.Annotation.find('>', end);
					if (start >= 0) {
						++start;
						resultsModel->addText(" ");
					}
				}
				if (start >= 0) {
					resultsModel->addText(book.Annotation.substr(start));
				}
			}
		}
	}
	setModel(resultsModel, ZLibrary::Language());
}

void NetLibraryView::search(SearchRunnable &runnable) {
	if (!ZLNetworkManager::instance().connect()) {
		NetworkOperationRunnable::showErrorMessage(
			ZLResource::resource("dialog")
				["networkError"]
				["couldntConnectToNetworkMessage"].value()
		);
		return;
	}

	runnable.executeWithUI();

	runnable.showErrorMessage();

	const NetworkBookList &bookList = runnable.bookList();
	if (!bookList.empty()) {
		myBookList = bookList;
		rebuildModel();
	} else if (myBookList.empty()) {
		ZLTextPlainModel *resultsModel = new ZLTextPlainModel(8192);
		resultsModel->createParagraph(ZLTextParagraph::TEXT_PARAGRAPH);
		resultsModel->addControl(LIBRARY_ENTRY, true);
		resultsModel->addText(myResource["notFound"].value());
		setModel(resultsModel, ZLibrary::Language());
	}
}

void NetLibraryView::search(const std::string &title, const std::string &author, const std::string &series, const std::string &category, const std::string &description) {
	AdvancedSearchRunnable runnable(title, author, series, category, description);
	search(runnable);
}

void NetLibraryView::search() {
	const std::string pattern = fbreader().visualParameter(NetLibraryView::SEARCH_PARAMETER_ID);

	if (pattern.empty()) {
		return;
	}

	SimpleSearchRunnable runnable(pattern);
	search(runnable);
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
		if (!ZLNetworkManager::instance().connect()) {
			NetworkOperationRunnable::showErrorMessage(
				ZLResource::resource("dialog")
					["networkError"]
					["couldntConnectToNetworkMessage"].value()
			);
			return false;
		}

		NetworkBookInfo::URLType format = (id == DownloadEpub) ? NetworkBookInfo::BOOK_EPUB : NetworkBookInfo::BOOK_MOBIPOCKET;
		DownloadBookRunnable downloader(*book, format);
		downloader.executeWithUI();
		if (downloader.hasErrors()) {
			downloader.showErrorMessage();
		} else {
			BookDescriptionPtr description = BookDescription::getDescription(downloader.fileName());
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
		}
		return true;
	} else if ((id == ReadLocalEpub) || (id == ReadLocalMobi)) {
		NetworkBookInfo::URLType format = (id == DownloadEpub) ? NetworkBookInfo::BOOK_EPUB : NetworkBookInfo::BOOK_MOBIPOCKET;
		fbreader().openFile(NetworkLinkCollection::instance().bookFileName(book->URLByType[format]));
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

bool NetLibraryView::_onStylusMove(int x, int y) {
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

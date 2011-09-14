#include <QApplication>

#include <ZLDialogManager.h>

#include "../../../../../fbreader/src/libraryTree/LibraryTreeNodes.h"
#include "../dialogs/ZLQtTreeDialog.h"

#include "../../../../../fbreader/src/library/Library.h"
#include "../../../../../fbreader/src/fbreader/FBReader.h"
#include "../../../../../fbreader/src/formats/FormatPlugin.h"

#include "../menu/DrillDownMenu.h"

#include "LibraryActions.h"

void ShowMenuLibraryAction::run() {
	ZLQtTreeDialog dialog;

	size_t index = 0;
	dialog.rootNode().insert(new AuthorTreeNode, index++);
	dialog.rootNode().insert(new TagTreeNode, index++);
	dialog.rootNode().insert(new RecentBooksTreeNode, index++);

	dialog.run();
}

void AuthorsMenuAction::run_init() {
	const AuthorList &authors = Library::Instance().authors();
	for (AuthorList::const_iterator it=authors.begin(); it!=authors.end(); ++it) {
		myMenu->addItem( (*it)->name(),  new BooksByAuthorMenuAction(myDialog,*it) );
	}
}

void TagsMenuAction::run_init() {
	const TagList &tags = Library::Instance().tags();
	for (TagList::const_iterator it=tags.begin(); it!=tags.end(); ++it) {
		myMenu->addItem( (*it)->name()  , new BooksByTagMenuAction(myDialog,*it) );
	}
}

void BooksByTagMenuAction::run_init() {
	const BookList &books = Library::Instance().books(myTag);
	for (BookList::const_iterator it=books.begin(); it!=books.end(); ++it) {
		myMenu->addItem( (*it)->title(),new BookMenuAction(myDialog,*it));
	}
}

void BooksByAuthorMenuAction::run_init() {
	const BookList &books = Library::Instance().books(myAuthor);
	for (BookList::const_iterator it=books.begin(); it!=books.end(); ++it) {
		myMenu->addItem( (*it)->title(), new BookMenuAction(myDialog,*it) );
	}
}

void BookMenuAction::run() {
	FBReader &fbreader = FBReader::Instance();
	fbreader.openBook(myBook);
	myDialog->close();
}

bool AddBookMenuAction::FileFilter::accepts(const ZLFile &file) const {
	return file.isArchive() || !PluginCollection::Instance().plugin(file, false).isNull();
}

void AddBookMenuAction::run() {
	const ZLResourceKey dialogKey("addFileDialog");
	FileFilter filter;
	shared_ptr<ZLOpenFileDialog> dialog = ZLDialogManager::Instance().createOpenFileDialog(dialogKey, "", "", filter);
	bool code = dialog->run();
	if (code) {
		shared_ptr<Book> book;
		ZLFile file(dialog->filePath());
		FBReader::Instance().createBook(file, book);
		if (!book.isNull()) {
			FBReader::Instance().openBook(book);
			myDialog->accept();
		}
	}
}

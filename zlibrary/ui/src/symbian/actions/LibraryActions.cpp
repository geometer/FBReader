#include <QApplication>

#include <ZLDialogManager.h>

#include "../../../../../fbreader/src/libraryTree/LibraryTreeNodes.h"
#include "../dialogs/ZLQtTreeDialog.h"

#include "../../../../../fbreader/src/library/Library.h"
#include "../../../../../fbreader/src/fbreader/FBReader.h"
#include "../../../../../fbreader/src/formats/FormatPlugin.h"

#include "../menu/DrillDownMenu.h"

#include "LibraryActions.h"

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

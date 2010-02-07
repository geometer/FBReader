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

#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>

#include "FBReaderActions.h"
#include "FBReader.h"
#include "FBFileHandler.h"
#include "BookInfoDialog.h"
#include "../library/Book.h"

class BookFileFilter : public ZLOpenFileDialog::Filter {

private:
	bool accepts(const std::string &filePath) const;
};

bool BookFileFilter::accepts(const std::string &filePath) const {
	return true;
}

AddBookAction::AddBookAction(int visibleInModes) : ModeDependentAction(visibleInModes) {
}

void AddBookAction::run() {
	BookFileFilter filter;
	shared_ptr<ZLOpenFileDialog> dialog = ZLDialogManager::Instance().createOpenFileDialog("", filter);
	if (!dialog.isNull()) {
		dialog->run();
		return;
	}

	FBReader &fbreader = FBReader::Instance();

	const ZLResourceKey dialogKey("addFileDialog");
	FBFileHandler handler;
	if (ZLDialogManager::Instance().selectionDialog(dialogKey, handler)) {
		shared_ptr<Book> book = handler.description();
		if (!book.isNull()) {
			if (BookInfoDialog(book).dialog().run()) {
				Library::Instance().addBook(book);
				fbreader.openBook(book);
				fbreader.setMode(FBReader::BOOK_TEXT_MODE);
			} else {
				Library::Instance().removeBook(book);
			}
			fbreader.refreshWindow();
		}
	}
}

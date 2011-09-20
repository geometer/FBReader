#ifndef __LIBRARYACTIONS_H__
#define __LIBRARYACTIONS_H__

#include <ZLOpenFileDialog.h>

#include "../../../../../fbreader/src/library/Library.h"

#include "DrillDownMenuActions.h"


class AddBookMenuAction : public MenuAction {
private:
		class FileFilter : public ZLOpenFileDialog::Filter {
		private:
			bool accepts(const ZLFile &file) const;
		};
public:
		AddBookMenuAction(DrillDownMenuDialog* dialog) : MenuAction(dialog) { }
		void run();
};

class BookMenuAction : public MenuAction {
public:
		BookMenuAction(DrillDownMenuDialog* dialog, const shared_ptr<Book> book) : MenuAction(dialog), myBook(book) { }
		void run();
private:
		const shared_ptr<Book> myBook;
};


#endif /* __LIBRARYACTIONS_H__ */

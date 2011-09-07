#ifndef __LIBRARYACTIONS_H__
#define __LIBRARYACTIONS_H__

#include <ZLOpenFileDialog.h>

#include "DrillDownMenuActions.h"

class ShowMenuLibraryAction : public ZLApplication::Action {
public:
	void run();
};

class AuthorsMenuAction : public OpenNodeMenuAction {
public:
		AuthorsMenuAction(DrillDownMenuDialog* dialog) : OpenNodeMenuAction(dialog) { }
		void run_init();
};

class TagsMenuAction : public OpenNodeMenuAction {
public:
		TagsMenuAction(DrillDownMenuDialog* dialog) : OpenNodeMenuAction(dialog) { }
		void run_init();
};

class BooksByTagMenuAction : public OpenNodeMenuAction {
public:
		BooksByTagMenuAction(DrillDownMenuDialog* dialog, const shared_ptr<Tag> tag): OpenNodeMenuAction(dialog), myTag(tag) { }
		void run_init();
private:
		const shared_ptr<Tag> myTag;
};

class BooksByAuthorMenuAction : public OpenNodeMenuAction {
public:
		BooksByAuthorMenuAction(DrillDownMenuDialog* dialog, const shared_ptr<Author> author) : OpenNodeMenuAction(dialog), myAuthor(author) { }
		void run_init();
private:
		const shared_ptr<Author> myAuthor;
};


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

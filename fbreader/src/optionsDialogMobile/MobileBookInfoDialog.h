#ifndef __MOBILEBOOKINFODIALOG_H__
#define __MOBILEBOOKINFODIALOG_H__

#include <string>

#include <ZLDialog.h>
#include <ZLOptionEntry.h>

#include "../formats/FormatPlugin.h"
#include "../library/Book.h"

class ZLOptionsDialog;
class ZLDialogContent;

class MobileBookInfoDialog {

public:
	MobileBookInfoDialog(shared_ptr<Book> book);
	
	static ZLResourceKey resourceKey();
        static void fillContent(ZLDialogContent &content, const AbstractBookInfo &info, bool cover=false);
        static void fillContent(ZLDialog &content, const AbstractBookInfo &info, bool cover=false);

	ZLDialog &dialog();

private:
	template <typename T>
        static void doFillContent(T &content, const AbstractBookInfo &info, bool cover);
	shared_ptr<Book> myBook;
	shared_ptr<ZLDialog> myDialog;

friend class MobileBookLanguageEntry;
};

inline ZLDialog &MobileBookInfoDialog::dialog() { return *myDialog; }
#endif /* __MOBILEBOOKINFODIALOG_H__ */

#ifndef __MOBILEBOOKINFODIALOG_H__
#define __MOBILEBOOKINFODIALOG_H__

#include <string>

#include <ZLDialog.h>
#include <ZLOptionEntry.h>

#include "../formats/FormatPlugin.h"

class ZLOptionsDialog;
class ZLDialogContent;

class MobileBookInfoDialog {

public:
	MobileBookInfoDialog(shared_ptr<Book> book);

	ZLDialog &dialog();

private:
	shared_ptr<Book> myBook;
	shared_ptr<ZLDialog> myDialog;

friend class MobileBookLanguageEntry;
};

inline ZLDialog &MobileBookInfoDialog::dialog() { return *myDialog; }
#endif /* __MOBILEBOOKINFODIALOG_H__ */

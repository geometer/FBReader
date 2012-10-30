#include "ZLTreeDialog.h"

ZLTreeDialog::ZLTreeDialog(const ZLResource &resource) : myResource(resource) {
}

void ZLTreeDialog::setSearcher(shared_ptr<ZLTreeSearcher> searcher) {
	mySearcher = searcher;
}


const ZLResource &ZLTreeDialog::resource() const {
	return myResource;
}

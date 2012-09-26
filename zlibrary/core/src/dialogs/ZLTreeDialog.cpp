#include "ZLTreeDialog.h"

ZLTreeDialog::ZLTreeDialog(const ZLResource &resource) : myResource(resource) {
}

ZLTreeDialog::~ZLTreeDialog() {
}


const ZLResource &ZLTreeDialog::resource() const {
	return myResource;
}

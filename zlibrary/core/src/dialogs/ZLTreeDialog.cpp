#include "ZLTreeDialog.h"

ZLTreeDialog::ZLTreeDialog(const ZLResource &resource) : myResource(resource) {
}


const ZLResource &ZLTreeDialog::resource() const {
	return myResource;
}

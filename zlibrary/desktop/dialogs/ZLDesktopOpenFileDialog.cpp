#include "ZLDesktopOpenFileDialog.h"

static const std::string OPTION_GROUP_NAME = "OpenFileDialog";

ZLDesktopOpenFileDialog::ZLDesktopOpenFileDialog(const ZLTreeHandler &handler) : ZLOpenFileDialog(handler), WidthOption(OPTION_GROUP_NAME, "Width", 400), HeightOption(OPTION_GROUP_NAME, "Height", 300) {
}

ZLDesktopOpenFileDialog::~ZLDesktopOpenFileDialog() {
}

void ZLDesktopOpenFileDialog::runWithSize() {
	setSize(WidthOption.value(), HeightOption.value());
	run();
	WidthOption.setValue(width());
	HeightOption.setValue(height());
}

#ifndef __MOBILEOPTIONSDIALOG_H__
#define __MOBILEOPTIONSDIALOG_H__

#include "../optionsDialog/AbstractOptionsDialog.h"

class MobileOptionsDialog : public AbstractOptionsDialog {
public:
    MobileOptionsDialog();
private:
	void addItem(std::string item);
};

#endif // __MOBILEOPTIONSDIALOG_H__

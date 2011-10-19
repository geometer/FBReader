#ifndef __MOBILEOPTIONSDIALOG_H__
#define __MOBILEOPTIONSDIALOG_H__

#include <ZLOptionEntry.h>

#include "../optionsDialog/AbstractOptionsDialog.h"

class ScrollingZonesEntry : public ZLComboOptionEntry {

public:
        enum ScrollingZones {
            LEFT_AND_RIGHT = 0,
            UP_AND_BOTTOM
        };

public:
        ScrollingZonesEntry(const ZLResource &resource, ZLIntegerRangeOption &typeOption);
        const std::string &initialValue() const;

private:
        const std::vector<std::string> &values() const;
        void onAccept(const std::string &value);

private:
        ZLIntegerRangeOption &myOption;
        std::vector<std::string> myValues;
};

class MobileOptionsDialog : public AbstractOptionsDialog {
public:
    MobileOptionsDialog();
private:
	void addItem(std::string item);
};

#endif // __MOBILEOPTIONSDIALOG_H__

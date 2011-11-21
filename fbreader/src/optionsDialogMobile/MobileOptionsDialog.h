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

class AbstractFontStyleEntry : public ZLComboOptionEntry {

protected:
    const std::vector<std::string> &values() const;
    void onAccept(const std::string &value);
    virtual void _onAccept(size_t value) = 0;

protected:
    std::vector<std::string> myValues;

};

class FontStyleEntry : public AbstractFontStyleEntry {

public:
        enum FontStyle {
            REGULAR = 0,
            BOLD,
            ITALIC,
            BOLD_ITALIC
        };

public:
        FontStyleEntry(const ZLResource &resource, ZLBooleanOption& boldOption, ZLBooleanOption& italicOption);
        const std::string &initialValue() const;

private:
        void _onAccept(size_t i);

private:
        ZLBooleanOption& myBoldOption;
        ZLBooleanOption& myItalicOption;
};

class FontStyleBoolean3Entry : public AbstractFontStyleEntry {

public:
        enum FontBoolean3Style {
            ON = 0,
            OFF,
            UNCHANGED
        };

public:
        FontStyleBoolean3Entry(const ZLResource &resource, ZLBoolean3Option &option);
        const std::string &initialValue() const;

private:
        void _onAccept(size_t i);

private:
        ZLBoolean3Option & myOption;
};



class MobileOptionsDialog : public AbstractOptionsDialog {
public:
    MobileOptionsDialog();
private:
	void addItem(std::string item);
};

#endif // __MOBILEOPTIONSDIALOG_H__

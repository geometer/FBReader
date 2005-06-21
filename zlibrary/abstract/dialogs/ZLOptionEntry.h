/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __ZLOPTIONENTRY_H__
#define __ZLOPTIONENTRY_H__

#include <vector>
#include <string>

#include <abstract/ZLOptions.h>

enum ZLOptionKind {
	CHOICE,
	BOOLEAN,
	STRING,
	SPIN,
	COMBO,
	COLOR,
	UNKNOWN,
};

class ZLOptionEntry;

class OptionView {

public:
	OptionView(ZLOptionEntry *option) DIALOG_SECTION;
	virtual ~OptionView() DIALOG_SECTION;
	virtual void setVisible(bool visible) DIALOG_SECTION = 0;
	virtual void setActive(bool active) DIALOG_SECTION = 0;

protected:
	ZLOptionEntry *myOption;
};

class ZLOptionEntry {

public:
	ZLOptionEntry() DIALOG_SECTION;
	virtual ~ZLOptionEntry() DIALOG_SECTION;
	virtual ZLOptionKind kind() const DIALOG_SECTION = 0;
	virtual const std::string &name() const DIALOG_SECTION = 0;

	void setView(OptionView *view) DIALOG_SECTION;

	void setVisible(bool visible) DIALOG_SECTION;
	bool isVisible() const DIALOG_SECTION;

	void setActive(bool active) DIALOG_SECTION;
	bool isActive() const DIALOG_SECTION;

private:
	OptionView *myView;
	bool myIsVisible;
	bool myIsActive;
};

class ZLUserDefinedOptionEntry : public ZLOptionEntry {

protected:
	ZLUserDefinedOptionEntry() DIALOG_SECTION;

public:
	virtual ~ZLUserDefinedOptionEntry() DIALOG_SECTION;
	ZLOptionKind kind() const DIALOG_SECTION;
	virtual OptionView *createView() DIALOG_SECTION = 0;
};

class ZLChoiceOptionEntry : public ZLOptionEntry {

protected:
	ZLChoiceOptionEntry() DIALOG_SECTION;

public:
	virtual ~ZLChoiceOptionEntry() DIALOG_SECTION;
	ZLOptionKind kind() const DIALOG_SECTION;

	virtual const std::string &text(int index) const DIALOG_SECTION = 0;
	virtual int choiceNumber() const DIALOG_SECTION = 0;
	virtual int initialCheckedIndex() const DIALOG_SECTION = 0;
	virtual void onAccept(int index) const DIALOG_SECTION = 0;
};

class ZLStringOptionEntry : public ZLOptionEntry {

protected:
	ZLStringOptionEntry() DIALOG_SECTION;

public:
	virtual ~ZLStringOptionEntry() DIALOG_SECTION;
	ZLOptionKind kind() const DIALOG_SECTION;

	virtual const std::string &initialValue() const DIALOG_SECTION = 0;
	virtual void onAccept(const std::string &value) const DIALOG_SECTION = 0;
};

class ZLSimpleStringOptionEntry : public ZLStringOptionEntry {

public:
	ZLSimpleStringOptionEntry(const std::string &name, const ZLStringOption &option) DIALOG_SECTION;
	virtual ~ZLSimpleStringOptionEntry() DIALOG_SECTION;
	const std::string &name() const DIALOG_SECTION;
	const std::string &initialValue() const DIALOG_SECTION;
	void onAccept(const std::string &value) const DIALOG_SECTION;

private:
	std::string myName;
	const ZLStringOption &myOption;
};

class ZLBooleanOptionEntry : public ZLOptionEntry {

protected:
	ZLBooleanOptionEntry() DIALOG_SECTION;

public:
	virtual ~ZLBooleanOptionEntry() DIALOG_SECTION;
	ZLOptionKind kind() const DIALOG_SECTION;

	virtual bool initialState() const DIALOG_SECTION = 0;
	virtual void onValueChange(bool state) DIALOG_SECTION;
	virtual void onAccept(bool state) const DIALOG_SECTION = 0;
};

class ZLSimpleBooleanOptionEntry : public ZLBooleanOptionEntry {

public:
	ZLSimpleBooleanOptionEntry(const std::string &name, const ZLBooleanOption &option) DIALOG_SECTION;
	virtual ~ZLSimpleBooleanOptionEntry() DIALOG_SECTION;
	const std::string &name() const DIALOG_SECTION;
	bool initialState() const DIALOG_SECTION;
	void onAccept(bool state) const DIALOG_SECTION;

private:
	std::string myName;
	const ZLBooleanOption &myOption;
};

class ZLSpinOptionEntry : public ZLOptionEntry {

protected:
	ZLSpinOptionEntry() DIALOG_SECTION;

public:
	virtual ~ZLSpinOptionEntry() DIALOG_SECTION;
	ZLOptionKind kind() const DIALOG_SECTION;

	virtual int initialValue() const DIALOG_SECTION = 0;
	virtual int minValue() const DIALOG_SECTION = 0;
	virtual int maxValue() const DIALOG_SECTION = 0;
	virtual int step() const DIALOG_SECTION = 0;
	virtual void onAccept(int value) const DIALOG_SECTION = 0;
};

class ZLSimpleSpinOptionEntry : public ZLSpinOptionEntry {

public:
	ZLSimpleSpinOptionEntry(const std::string &name, const ZLIntegerOption &option, int minValue, int maxValue, int step) DIALOG_SECTION;
	virtual ~ZLSimpleSpinOptionEntry() DIALOG_SECTION;
	const std::string &name() const DIALOG_SECTION;
	int initialValue() const DIALOG_SECTION;
	int minValue() const DIALOG_SECTION;
	int maxValue() const DIALOG_SECTION;
	int step() const DIALOG_SECTION;
	void onAccept(int value) const DIALOG_SECTION;

private:
	std::string myName;
	const ZLIntegerOption &myOption;
	int myMinValue;
	int myMaxValue;
	int myStep;
};

class ZLComboOptionEntry : public ZLOptionEntry {

protected:
	ZLComboOptionEntry() DIALOG_SECTION;

public:
	virtual ~ZLComboOptionEntry() DIALOG_SECTION;
	ZLOptionKind kind() const DIALOG_SECTION;

	virtual const std::string &initialValue() const DIALOG_SECTION = 0;
	virtual const std::vector<std::string> &values() const DIALOG_SECTION = 0;
	virtual void onValueChange(const std::string&) DIALOG_SECTION;
	virtual void onAccept(const std::string &value) const DIALOG_SECTION = 0;
};

class ZLSimpleBoolean3OptionEntry : public ZLComboOptionEntry {

public:
	ZLSimpleBoolean3OptionEntry(const std::string &name, const ZLBoolean3Option &option) DIALOG_SECTION;
	virtual ~ZLSimpleBoolean3OptionEntry() DIALOG_SECTION;
	const std::string &name() const DIALOG_SECTION;
	const std::string &initialValue() const DIALOG_SECTION;
	const std::vector<std::string> &values() const DIALOG_SECTION;
	void onAccept(const std::string &value) const DIALOG_SECTION;

private:
	static std::vector<std::string> ourValues;
	std::string myName;
	const ZLBoolean3Option &myOption;
};

class ZLColorOptionEntry : public ZLOptionEntry {

protected:
	ZLColorOptionEntry() DIALOG_SECTION;

public:
	virtual ~ZLColorOptionEntry() DIALOG_SECTION;
	ZLOptionKind kind() const DIALOG_SECTION;

	virtual const ZLColor color() const DIALOG_SECTION = 0;
	virtual void onAccept(ZLColor color) DIALOG_SECTION = 0;
};

class ZLSimpleColorOptionEntry : public ZLColorOptionEntry {

public:
	ZLSimpleColorOptionEntry(ZLColorOption &option) DIALOG_SECTION;
	virtual ~ZLSimpleColorOptionEntry() DIALOG_SECTION;
	const std::string &name() const DIALOG_SECTION;
	const ZLColor color() const DIALOG_SECTION;
	void onAccept(ZLColor color) DIALOG_SECTION;

private:
	ZLColorOption &myOption; 
};

inline ZLOptionEntry::ZLOptionEntry() : myView(0), myIsVisible(true), myIsActive(true) {}
inline ZLOptionEntry::~ZLOptionEntry() {}
inline void ZLOptionEntry::setView(OptionView *view) { myView = view; }
inline bool ZLOptionEntry::isVisible() const { return myIsVisible; }
inline bool ZLOptionEntry::isActive() const { return myIsActive; }

inline ZLUserDefinedOptionEntry::ZLUserDefinedOptionEntry() {}
inline ZLUserDefinedOptionEntry::~ZLUserDefinedOptionEntry() {}
inline ZLOptionKind ZLUserDefinedOptionEntry::kind() const { return UNKNOWN; }

inline ZLChoiceOptionEntry::ZLChoiceOptionEntry() {}
inline ZLChoiceOptionEntry::~ZLChoiceOptionEntry() {}
inline ZLOptionKind ZLChoiceOptionEntry::kind() const { return CHOICE; }

inline ZLStringOptionEntry::ZLStringOptionEntry() {}
inline ZLStringOptionEntry::~ZLStringOptionEntry() {}
inline ZLOptionKind ZLStringOptionEntry::kind() const { return STRING; }

inline ZLSimpleStringOptionEntry::ZLSimpleStringOptionEntry(const std::string &name, const ZLStringOption &option) : myName(name), myOption(option) {}
inline ZLSimpleStringOptionEntry::~ZLSimpleStringOptionEntry() {}
inline const std::string &ZLSimpleStringOptionEntry::name() const { return myName; }
inline const std::string &ZLSimpleStringOptionEntry::initialValue() const { return myOption.value(); }
inline void ZLSimpleStringOptionEntry::onAccept(const std::string &value) const { myOption.setValue(value); }

inline ZLBooleanOptionEntry::ZLBooleanOptionEntry() {}
inline ZLBooleanOptionEntry::~ZLBooleanOptionEntry() {}
inline ZLOptionKind ZLBooleanOptionEntry::kind() const { return BOOLEAN; }
inline void ZLBooleanOptionEntry::onValueChange(bool) {}

inline ZLSimpleBooleanOptionEntry::ZLSimpleBooleanOptionEntry(const std::string &name, const ZLBooleanOption &option) : myName(name), myOption(option) {}
inline ZLSimpleBooleanOptionEntry::~ZLSimpleBooleanOptionEntry() {}
inline const std::string &ZLSimpleBooleanOptionEntry::name() const { return myName; }
inline bool ZLSimpleBooleanOptionEntry::initialState() const { return myOption.value(); }
inline void ZLSimpleBooleanOptionEntry::onAccept(bool state) const { myOption.setValue(state); }

inline ZLSpinOptionEntry::ZLSpinOptionEntry() {}
inline ZLSpinOptionEntry::~ZLSpinOptionEntry() {}
inline ZLOptionKind ZLSpinOptionEntry::kind() const { return SPIN; }

inline ZLSimpleSpinOptionEntry::ZLSimpleSpinOptionEntry(const std::string &name, const ZLIntegerOption &option, int minValue, int maxValue, int step) : myName(name), myOption(option), myMinValue(minValue), myMaxValue(maxValue), myStep(step) {}
inline ZLSimpleSpinOptionEntry::~ZLSimpleSpinOptionEntry() {}
inline const std::string &ZLSimpleSpinOptionEntry::name() const { return myName; }
inline int ZLSimpleSpinOptionEntry::initialValue() const { return myOption.value(); }
inline int ZLSimpleSpinOptionEntry::minValue() const { return myMinValue; }
inline int ZLSimpleSpinOptionEntry::maxValue() const { return myMaxValue; }
inline int ZLSimpleSpinOptionEntry::step() const { return myStep; }
inline void ZLSimpleSpinOptionEntry::onAccept(int value) const { myOption.setValue(value); }

inline ZLComboOptionEntry::ZLComboOptionEntry() {}
inline ZLComboOptionEntry::~ZLComboOptionEntry() {}
inline ZLOptionKind ZLComboOptionEntry::kind() const { return COMBO; }
inline void ZLComboOptionEntry::onValueChange(const std::string&) {}

inline ZLSimpleBoolean3OptionEntry::ZLSimpleBoolean3OptionEntry(const std::string &name, const ZLBoolean3Option &option) : myName(name), myOption(option) {}
inline ZLSimpleBoolean3OptionEntry::~ZLSimpleBoolean3OptionEntry() {}
inline const std::string &ZLSimpleBoolean3OptionEntry::name() const { return myName; }

inline ZLColorOptionEntry::ZLColorOptionEntry() {}
inline ZLColorOptionEntry::~ZLColorOptionEntry() {}
inline ZLOptionKind ZLColorOptionEntry::kind() const { return COLOR; }

inline ZLSimpleColorOptionEntry::ZLSimpleColorOptionEntry(ZLColorOption &option) : myOption(option) {}
inline ZLSimpleColorOptionEntry::~ZLSimpleColorOptionEntry() {}
inline const ZLColor ZLSimpleColorOptionEntry::color() const { return myOption.value(); }
inline void ZLSimpleColorOptionEntry::onAccept(ZLColor color) { myOption.setValue(color); }

#endif /* __ZLOPTIONENTRY_H__ */

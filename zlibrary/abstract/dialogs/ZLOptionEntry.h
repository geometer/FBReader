/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __ZLOPTIONENTRY_H__
#define __ZLOPTIONENTRY_H__

#include <vector>
#include <string>

#include <ZLOptions.h>

enum ZLOptionKind {
	CHOICE,
	BOOLEAN,
	STRING,
	SPIN,
	COMBO,
	COLOR,
	KEY,
	ORDER,
	UNKNOWN,
};

class ZLOptionEntry;

class OptionView {

public:
	OptionView(ZLOptionEntry *option);
	virtual ~OptionView();
	virtual void setVisible(bool visible) = 0;
	virtual void setActive(bool active) = 0;

protected:
	ZLOptionEntry *myOption;
};

class ZLOptionEntry {

public:
	ZLOptionEntry();
	virtual ~ZLOptionEntry();
	virtual ZLOptionKind kind() const = 0;
	virtual const std::string &name() const = 0;

	void setView(OptionView *view);

	void setVisible(bool visible);
	bool isVisible() const;

	void setActive(bool active);
	bool isActive() const;

private:
	OptionView *myView;
	bool myIsVisible;
	bool myIsActive;
};

class ZLUserDefinedOptionEntry : public ZLOptionEntry {

protected:
	ZLUserDefinedOptionEntry();

public:
	virtual ~ZLUserDefinedOptionEntry();
	ZLOptionKind kind() const;
	virtual OptionView *createView() = 0;
};

class ZLChoiceOptionEntry : public ZLOptionEntry {

protected:
	ZLChoiceOptionEntry();

public:
	virtual ~ZLChoiceOptionEntry();
	ZLOptionKind kind() const;

	virtual const std::string &text(int index) const = 0;
	virtual int choiceNumber() const = 0;
	virtual int initialCheckedIndex() const = 0;
	virtual void onAccept(int index) = 0;
};

class ZLStringOptionEntry : public ZLOptionEntry {

protected:
	ZLStringOptionEntry();

public:
	virtual ~ZLStringOptionEntry();
	ZLOptionKind kind() const;

	virtual const std::string &initialValue() const = 0;
	virtual void onAccept(const std::string &value) = 0;
};

class ZLSimpleStringOptionEntry : public ZLStringOptionEntry {

public:
	ZLSimpleStringOptionEntry(const std::string &name, ZLStringOption &option);
	virtual ~ZLSimpleStringOptionEntry();
	const std::string &name() const;
	const std::string &initialValue() const;
	void onAccept(const std::string &value);

private:
	std::string myName;
	ZLStringOption &myOption;
};

class ZLBooleanOptionEntry : public ZLOptionEntry {

protected:
	ZLBooleanOptionEntry();

public:
	virtual ~ZLBooleanOptionEntry();
	ZLOptionKind kind() const;

	virtual bool initialState() const = 0;
	virtual void onValueChange(bool state);
	virtual void onAccept(bool state) = 0;
};

class ZLSimpleBooleanOptionEntry : public ZLBooleanOptionEntry {

public:
	ZLSimpleBooleanOptionEntry(const std::string &name, ZLBooleanOption &option);
	virtual ~ZLSimpleBooleanOptionEntry();
	const std::string &name() const;
	bool initialState() const;
	void onAccept(bool state);

private:
	std::string myName;
	ZLBooleanOption &myOption;
};

class ZLSpinOptionEntry : public ZLOptionEntry {

protected:
	ZLSpinOptionEntry();

public:
	virtual ~ZLSpinOptionEntry();
	ZLOptionKind kind() const;

	virtual int initialValue() const = 0;
	virtual int minValue() const = 0;
	virtual int maxValue() const = 0;
	virtual int step() const = 0;
	virtual void onAccept(int value) = 0;
};

class ZLSimpleSpinOptionEntry : public ZLSpinOptionEntry {

public:
	ZLSimpleSpinOptionEntry(const std::string &name, ZLIntegerRangeOption &option, int step);
	virtual ~ZLSimpleSpinOptionEntry();
	const std::string &name() const;
	int initialValue() const;
	int minValue() const;
	int maxValue() const;
	int step() const;
	void onAccept(int value);

private:
	std::string myName;
	ZLIntegerRangeOption &myOption;
	int myStep;
};

class ZLComboOptionEntry : public ZLOptionEntry {

protected:
	ZLComboOptionEntry();

public:
	virtual ~ZLComboOptionEntry();
	ZLOptionKind kind() const;

	virtual const std::string &initialValue() const = 0;
	virtual const std::vector<std::string> &values() const = 0;
	virtual void onValueChange(const std::string&);
	virtual void onAccept(const std::string &value) = 0;
};

class ZLSimpleBoolean3OptionEntry : public ZLComboOptionEntry {

public:
	ZLSimpleBoolean3OptionEntry(const std::string &name, ZLBoolean3Option &option);
	virtual ~ZLSimpleBoolean3OptionEntry();
	const std::string &name() const;
	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

private:
	static std::vector<std::string> ourValues;
	std::string myName;
	ZLBoolean3Option &myOption;
};

class ZLColorOptionEntry : public ZLOptionEntry {

protected:
	ZLColorOptionEntry();

public:
	virtual ~ZLColorOptionEntry();
	ZLOptionKind kind() const;

	virtual const ZLColor color() const = 0;
	virtual void onAccept(ZLColor color) = 0;
};

class ZLSimpleColorOptionEntry : public ZLColorOptionEntry {

public:
	ZLSimpleColorOptionEntry(ZLColorOption &option);
	virtual ~ZLSimpleColorOptionEntry();
	const std::string &name() const;
	const ZLColor color() const;
	void onAccept(ZLColor color);

private:
	ZLColorOption &myOption; 
};

class ZLKeyOptionEntry : public ZLOptionEntry {

public:
	ZLKeyOptionEntry(const std::string &name);
	~ZLKeyOptionEntry();
	void addActionName(const std::string &actionName);
	const std::vector<std::string> &actionNames() const;
	virtual void onAccept() = 0;
	virtual int actionIndex(const std::string &key) = 0;
	virtual void onValueChange(const std::string &key, int index) = 0;
	void reset();

protected:
	ZLOptionKind kind() const;
	const std::string &name() const;

private:
	std::string myName;
	std::vector<std::string> myActionNames;
};

class ZLOrderOptionEntry : public ZLOptionEntry {

public:
	ZLOrderOptionEntry();
	std::vector<std::string> &values();

protected:
	ZLOptionKind kind() const;
	const std::string &name() const;

private:
	std::string myName;
	std::vector<std::string> myValues;
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

inline ZLSimpleStringOptionEntry::ZLSimpleStringOptionEntry(const std::string &name, ZLStringOption &option) : myName(name), myOption(option) {}
inline ZLSimpleStringOptionEntry::~ZLSimpleStringOptionEntry() {}
inline const std::string &ZLSimpleStringOptionEntry::name() const { return myName; }
inline const std::string &ZLSimpleStringOptionEntry::initialValue() const { return myOption.value(); }
inline void ZLSimpleStringOptionEntry::onAccept(const std::string &value) { myOption.setValue(value); }

inline ZLBooleanOptionEntry::ZLBooleanOptionEntry() {}
inline ZLBooleanOptionEntry::~ZLBooleanOptionEntry() {}
inline ZLOptionKind ZLBooleanOptionEntry::kind() const { return BOOLEAN; }
inline void ZLBooleanOptionEntry::onValueChange(bool) {}

inline ZLSimpleBooleanOptionEntry::ZLSimpleBooleanOptionEntry(const std::string &name, ZLBooleanOption &option) : myName(name), myOption(option) {}
inline ZLSimpleBooleanOptionEntry::~ZLSimpleBooleanOptionEntry() {}
inline const std::string &ZLSimpleBooleanOptionEntry::name() const { return myName; }
inline bool ZLSimpleBooleanOptionEntry::initialState() const { return myOption.value(); }
inline void ZLSimpleBooleanOptionEntry::onAccept(bool state) { myOption.setValue(state); }

inline ZLSpinOptionEntry::ZLSpinOptionEntry() {}
inline ZLSpinOptionEntry::~ZLSpinOptionEntry() {}
inline ZLOptionKind ZLSpinOptionEntry::kind() const { return SPIN; }

inline ZLSimpleSpinOptionEntry::ZLSimpleSpinOptionEntry(const std::string &name, ZLIntegerRangeOption &option, int step) : myName(name), myOption(option), myStep(step) {}
inline ZLSimpleSpinOptionEntry::~ZLSimpleSpinOptionEntry() {}
inline const std::string &ZLSimpleSpinOptionEntry::name() const { return myName; }
inline int ZLSimpleSpinOptionEntry::initialValue() const { return myOption.value(); }
inline int ZLSimpleSpinOptionEntry::minValue() const { return myOption.minValue(); }
inline int ZLSimpleSpinOptionEntry::maxValue() const { return myOption.maxValue(); }
inline int ZLSimpleSpinOptionEntry::step() const { return myStep; }
inline void ZLSimpleSpinOptionEntry::onAccept(int value) { myOption.setValue(value); }

inline ZLComboOptionEntry::ZLComboOptionEntry() {}
inline ZLComboOptionEntry::~ZLComboOptionEntry() {}
inline ZLOptionKind ZLComboOptionEntry::kind() const { return COMBO; }
inline void ZLComboOptionEntry::onValueChange(const std::string&) {}

inline ZLSimpleBoolean3OptionEntry::ZLSimpleBoolean3OptionEntry(const std::string &name, ZLBoolean3Option &option) : myName(name), myOption(option) {}
inline ZLSimpleBoolean3OptionEntry::~ZLSimpleBoolean3OptionEntry() {}
inline const std::string &ZLSimpleBoolean3OptionEntry::name() const { return myName; }

inline ZLColorOptionEntry::ZLColorOptionEntry() {}
inline ZLColorOptionEntry::~ZLColorOptionEntry() {}
inline ZLOptionKind ZLColorOptionEntry::kind() const { return COLOR; }

inline ZLSimpleColorOptionEntry::ZLSimpleColorOptionEntry(ZLColorOption &option) : myOption(option) {}
inline ZLSimpleColorOptionEntry::~ZLSimpleColorOptionEntry() {}
inline const ZLColor ZLSimpleColorOptionEntry::color() const { return myOption.value(); }
inline void ZLSimpleColorOptionEntry::onAccept(ZLColor color) { myOption.setValue(color); }

inline ZLKeyOptionEntry::ZLKeyOptionEntry(const std::string &name) : myName(name) {}
inline ZLKeyOptionEntry::~ZLKeyOptionEntry() {}
inline void ZLKeyOptionEntry::addActionName(const std::string &actionName) { myActionNames.push_back(actionName); }
inline const std::vector<std::string> &ZLKeyOptionEntry::actionNames() const { return myActionNames; }
inline ZLOptionKind ZLKeyOptionEntry::kind() const { return KEY; }
inline const std::string &ZLKeyOptionEntry::name() const { return myName; }

inline ZLOrderOptionEntry::ZLOrderOptionEntry() {}
inline ZLOptionKind ZLOrderOptionEntry::kind() const { return ORDER; }
inline const std::string &ZLOrderOptionEntry::name() const { return myName; }
inline std::vector<std::string> &ZLOrderOptionEntry::values() { return myValues; }

#endif /* __ZLOPTIONENTRY_H__ */

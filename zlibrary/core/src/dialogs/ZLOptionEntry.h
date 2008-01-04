/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

class ZLOptionView;

class ZLOptionEntry {

public:
	enum ZLOptionKind {
		CHOICE,
		BOOLEAN,
		BOOLEAN3,
		STRING,
		SPIN,
		COMBO,
		COLOR,
		KEY,
		ORDER,
		MULTILINE,
	};

public:
	ZLOptionEntry();
	virtual ~ZLOptionEntry();
	virtual ZLOptionKind kind() const = 0;

	void setView(ZLOptionView *view);
	void resetView();

	virtual void setVisible(bool visible);
	bool isVisible() const;

	virtual void setActive(bool active);
	bool isActive() const;

private:
	ZLOptionView *myView;
	bool myIsVisible;
	bool myIsActive;
};

class ZLChoiceOptionEntry : public ZLOptionEntry {

protected:
	ZLChoiceOptionEntry();

public:
	ZLOptionKind kind() const;

	virtual const std::string &text(int index) const = 0;
	virtual int choiceNumber() const = 0;
	virtual int initialCheckedIndex() const = 0;
	virtual void onAccept(int index) = 0;
};

class ZLTextOptionEntry : public ZLOptionEntry {

public:
	virtual const std::string &initialValue() const = 0;
	void onAccept(const char *value);
	virtual void onAccept(const std::string &value) = 0;
	virtual bool useOnValueEdited() const;
	void onValueEdited(const char *value);
	virtual void onValueEdited(const std::string &value);
};

class ZLStringOptionEntry : public ZLTextOptionEntry {

protected:
	ZLStringOptionEntry();

public:
	ZLOptionKind kind() const;
};

class ZLMultilineOptionEntry : public ZLTextOptionEntry {

protected:
	ZLMultilineOptionEntry();

public:
	ZLOptionKind kind() const;
};

class ZLBooleanOptionEntry : public ZLOptionEntry {

protected:
	ZLBooleanOptionEntry();

public:
	ZLOptionKind kind() const;

	virtual bool initialState() const = 0;
	virtual void onStateChanged(bool state);
	virtual void onAccept(bool state) = 0;
};

class ZLBoolean3OptionEntry : public ZLOptionEntry {

protected:
	ZLBoolean3OptionEntry();

public:
	ZLOptionKind kind() const;

	virtual ZLBoolean3 initialState() const = 0;
	virtual void onStateChanged(ZLBoolean3 state);
	virtual void onAccept(ZLBoolean3 state) = 0;
};

class ZLSpinOptionEntry : public ZLOptionEntry {

protected:
	ZLSpinOptionEntry();

public:
	ZLOptionKind kind() const;

	virtual int initialValue() const = 0;
	virtual int minValue() const = 0;
	virtual int maxValue() const = 0;
	virtual int step() const = 0;
	virtual void onAccept(int value) = 0;
};

class ZLComboOptionEntry : public ZLOptionEntry {

protected:
	ZLComboOptionEntry(bool editable = false);

public:
	ZLOptionKind kind() const;

	virtual const std::string &initialValue() const = 0;
	virtual const std::vector<std::string> &values() const = 0;
	virtual void onValueSelected(int index);
	void onStringValueSelected(const char *value);
	void onStringValueSelected(const std::string &value);
	virtual bool useOnValueEdited() const;
	void onValueEdited(const char *value);
	virtual void onValueEdited(const std::string &value);
	void onAccept(const char *value);
	virtual void onAccept(const std::string &value) = 0;

	bool isEditable() const;

private:
	const bool myEditable; 
};

class ZLColorOptionEntry : public ZLOptionEntry {

protected:
	ZLColorOptionEntry();

public:
	ZLOptionKind kind() const;

	virtual const ZLColor initialColor() const = 0;
	virtual const ZLColor color() const = 0;
	virtual void onAccept(ZLColor color) = 0;
	virtual void onReset(ZLColor color);
};

class ZLKeyOptionEntry : public ZLOptionEntry {

public:
	ZLKeyOptionEntry();
	void addActionName(const std::string &actionName);
	const std::vector<std::string> &actionNames() const;
	virtual void onAccept() = 0;
	virtual int actionIndex(const std::string &key) = 0;
	virtual void onValueChanged(const std::string &key, int index) = 0;
	virtual void onKeySelected(const std::string &key) = 0;

protected:
	ZLOptionKind kind() const;

private:
	std::vector<std::string> myActionNames;
};

class ZLOrderOptionEntry : public ZLOptionEntry {

public:
	ZLOrderOptionEntry();
	std::vector<std::string> &values();

protected:
	ZLOptionKind kind() const;

private:
	std::vector<std::string> myValues;
};

inline ZLOptionEntry::ZLOptionEntry() : myView(0), myIsVisible(true), myIsActive(true) {}
inline ZLOptionEntry::~ZLOptionEntry() {}
inline void ZLOptionEntry::setView(ZLOptionView *view) { myView = view; }
inline bool ZLOptionEntry::isVisible() const { return myIsVisible; }
inline bool ZLOptionEntry::isActive() const { return myIsActive; }

inline ZLChoiceOptionEntry::ZLChoiceOptionEntry() {}
inline ZLOptionEntry::ZLOptionKind ZLChoiceOptionEntry::kind() const { return CHOICE; }

inline bool ZLTextOptionEntry::useOnValueEdited() const { return false; }
inline void ZLTextOptionEntry::onValueEdited(const std::string&) {}

inline ZLStringOptionEntry::ZLStringOptionEntry() {}
inline ZLOptionEntry::ZLOptionKind ZLStringOptionEntry::kind() const { return STRING; }

inline ZLMultilineOptionEntry::ZLMultilineOptionEntry() {}
inline ZLOptionEntry::ZLOptionKind ZLMultilineOptionEntry::kind() const { return MULTILINE; }

inline ZLBooleanOptionEntry::ZLBooleanOptionEntry() {}
inline ZLOptionEntry::ZLOptionKind ZLBooleanOptionEntry::kind() const { return BOOLEAN; }
inline void ZLBooleanOptionEntry::onStateChanged(bool) {}

inline ZLBoolean3OptionEntry::ZLBoolean3OptionEntry() {}
inline ZLOptionEntry::ZLOptionKind ZLBoolean3OptionEntry::kind() const { return BOOLEAN3; }
inline void ZLBoolean3OptionEntry::onStateChanged(ZLBoolean3) {}

inline ZLSpinOptionEntry::ZLSpinOptionEntry() {}
inline ZLOptionEntry::ZLOptionKind ZLSpinOptionEntry::kind() const { return SPIN; }

inline ZLComboOptionEntry::ZLComboOptionEntry(bool editable) : myEditable(editable) {}
inline ZLOptionEntry::ZLOptionKind ZLComboOptionEntry::kind() const { return COMBO; }
inline void ZLComboOptionEntry::onValueSelected(int) {}
inline bool ZLComboOptionEntry::useOnValueEdited() const { return false; }
inline void ZLComboOptionEntry::onValueEdited(const std::string&) {}
inline bool ZLComboOptionEntry::isEditable() const { return myEditable; }

inline ZLColorOptionEntry::ZLColorOptionEntry() {}
inline ZLOptionEntry::ZLOptionKind ZLColorOptionEntry::kind() const { return COLOR; }
inline void ZLColorOptionEntry::onReset(ZLColor) {}

inline ZLKeyOptionEntry::ZLKeyOptionEntry() {}
inline void ZLKeyOptionEntry::addActionName(const std::string &actionName) { myActionNames.push_back(actionName); }
inline const std::vector<std::string> &ZLKeyOptionEntry::actionNames() const { return myActionNames; }
inline ZLOptionEntry::ZLOptionKind ZLKeyOptionEntry::kind() const { return KEY; }

inline ZLOrderOptionEntry::ZLOrderOptionEntry() {}
inline ZLOptionEntry::ZLOptionKind ZLOrderOptionEntry::kind() const { return ORDER; }
inline std::vector<std::string> &ZLOrderOptionEntry::values() { return myValues; }

#endif /* __ZLOPTIONENTRY_H__ */

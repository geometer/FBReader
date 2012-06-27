/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include "ZLApplicationWindow.h"

#include "../view/ZLViewWidget.h"

ZLApplicationWindow *ZLApplicationWindow::ourInstance = 0;

ZLApplicationWindow &ZLApplicationWindow::Instance() {
	return *ourInstance;
}

ZLApplicationWindow::ZLApplicationWindow(ZLApplication *application) : myApplication(application), myToggleButtonLock(false) {
	ourInstance = this;
	myApplication->myWindow = this;
}

void ZLApplicationWindow::init() {
	myApplication->myViewWidget = createViewWidget();

	initToolbar(WINDOW_TOOLBAR);
	initToolbar(FULLSCREEN_TOOLBAR);
	initMenu();
}

void ZLApplicationWindow::initToolbar(ToolbarType type) {
	const ZLToolbar::ItemVector &toolbarItems = myApplication->toolbar(type).items();
	for (ZLToolbar::ItemVector::const_iterator it = toolbarItems.begin(); it != toolbarItems.end(); ++it) {
		addToolbarItem(*it);
	}
}

void ZLApplicationWindow::refresh() {
	refreshToolbar(WINDOW_TOOLBAR);
	refreshToolbar(FULLSCREEN_TOOLBAR);
	processAllEvents();
}

void ZLApplicationWindow::refreshToolbar(ToolbarType type) {
	const ZLToolbar::ItemVector &items = application().toolbar(type).items();
	bool canAddSeparator = false;
	ZLToolbar::ItemPtr lastSeparator = 0;
	for (ZLToolbar::ItemVector::const_iterator it = items.begin(); it != items.end(); ++it) {
		switch ((*it)->type()) {
			case ZLToolbar::Item::TEXT_FIELD:
			case ZLToolbar::Item::COMBO_BOX:
			case ZLToolbar::Item::SEARCH_FIELD:
			case ZLToolbar::Item::PLAIN_BUTTON:
			case ZLToolbar::Item::MENU_BUTTON:
				{
					ZLToolbar::ActionItem &button = (ZLToolbar::ActionItem&)**it;
					const std::string &id = button.actionId();
        
					const bool visible = application().isActionVisible(id);
					const bool enabled = application().isActionEnabled(id);
        
					if (visible) {
						if (!lastSeparator.isNull()) {
							setToolbarItemState(lastSeparator, true, true);
							lastSeparator = 0;
						}
						canAddSeparator = true;
					}
					setToolbarItemState(*it, visible, enabled);
				}
				break;
			case ZLToolbar::Item::TOGGLE_BUTTON:
				{
					ZLToolbar::ToggleButtonItem &button = (ZLToolbar::ToggleButtonItem&)**it;
					const std::string &id = button.actionId();
        
					const bool visible = application().isActionVisible(id);
					const bool enabled = application().isActionEnabled(id);
        
					if (visible) {
						if (!lastSeparator.isNull()) {
							setToolbarItemState(lastSeparator, true, true);
							lastSeparator = 0;
						}
						canAddSeparator = true;
					}
					/*
					if (!enabled && button.isPressed()) {
						button.buttonGroup().press(0);
						//application().doAction(group->UnselectAllButtonsActionId);
						myToggleButtonLock = true;
						setToggleButtonState(button);
						myToggleButtonLock = false;
					}
					*/
					setToolbarItemState(*it, visible, enabled);
					setToggleButtonState(button);
				}
				break;
			case ZLToolbar::Item::SEPARATOR:
				if (canAddSeparator) {
					lastSeparator = *it;
					canAddSeparator = false;
				} else {
					setToolbarItemState(*it, false, true);
				}
				break;
			case ZLToolbar::Item::FILL_SEPARATOR:
				if (canAddSeparator) {
					lastSeparator = *it;
					canAddSeparator = false;
				} else if (lastSeparator != 0) {
					setToolbarItemState(lastSeparator, false, true);
					lastSeparator = *it;
				} else {
					setToolbarItemState(*it, false, true);
				}
				break;
		}
	}
	if (!lastSeparator.isNull()) {
		setToolbarItemState(lastSeparator, false, true);
	}
}

ZLApplicationWindow::ToolbarType ZLApplicationWindow::type(const ZLToolbar::Item &item) const {
	return
		(&item.toolbar() == &application().toolbar(WINDOW_TOOLBAR)) ?
			WINDOW_TOOLBAR : FULLSCREEN_TOOLBAR;
}

bool ZLApplicationWindow::hasFullscreenToolbar() const {
	return !application().toolbar(FULLSCREEN_TOOLBAR).items().empty();
}

void ZLApplicationWindow::onButtonPress(const ZLToolbar::AbstractButtonItem &button) {
	if (myToggleButtonLock) {
		return;
	}
	if (button.type() == ZLToolbar::Item::TOGGLE_BUTTON) {
		ZLToolbar::ToggleButtonItem &toggleButton = (ZLToolbar::ToggleButtonItem&)button;
		myToggleButtonLock = true;
		if (toggleButton.isPressed()) {
			setToggleButtonState(toggleButton);
			myToggleButtonLock = false;
			return;
		} else {
			toggleButton.press();
			const ZLToolbar::ButtonGroup::ItemSet &items = toggleButton.buttonGroup().myItems;
			for (ZLToolbar::ButtonGroup::ItemSet::const_iterator it = items.begin(); it != items.end(); ++it) {
				setToggleButtonState(**it);
			}
		}
		myToggleButtonLock = false;
	}
	application().doAction(button.actionId());
}

void ZLApplicationWindow::setVisualParameter(const std::string &id, const std::string &value) {
	std::map<std::string,shared_ptr<VisualParameter> >::iterator it = myParameterMap.find(id);
	if (it != myParameterMap.end()) {
		it->second->setValue(value);
	}
}

void ZLApplicationWindow::setParameterValueList(const std::string &id, const std::vector<std::string> &values) {
	std::map<std::string,shared_ptr<VisualParameter> >::iterator it = myParameterMap.find(id);
	if (it != myParameterMap.end()) {
		it->second->setValueList(values);
	}
}

const std::string &ZLApplicationWindow::visualParameter(const std::string &id) {
	std::map<std::string,shared_ptr<VisualParameter> >::const_iterator it = myParameterMap.find(id);
	if (it != myParameterMap.end()) {
		return it->second->value();
	}
	static const std::string EMPTY;
	return EMPTY;
}

void ZLApplicationWindow::addVisualParameter(const std::string &id, shared_ptr<VisualParameter> parameter) {
	if (!parameter.isNull()) {
		myParameterMap[id] = parameter;
	}
}

ZLApplicationWindow::VisualParameter::~VisualParameter() {
}

const std::string &ZLApplicationWindow::VisualParameter::value() const {
	myValue = internalValue();
	return myValue;
}

void ZLApplicationWindow::VisualParameter::setValue(const std::string &value) {
	if (value != myValue) {
		myValue = value;
		internalSetValue(value);
	}
}

void ZLApplicationWindow::VisualParameter::restoreOldValue() {
	internalSetValue(myValue);
}

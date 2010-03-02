/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLibrary.h>
#include <ZLPopupData.h>

#include "ZLGtkApplicationWindow.h"

#include "../util/ZLGtkSignalUtil.h"

static const char *menuId = "gtk-tool-button-menu-id";
static const char *menuKey = "gtk-tool-button-menu-key";

static void runMenuItem(GtkImageMenuItem *menuItem, ZLGtkApplicationWindow *data) {
	data->onGtkButtonPress(GTK_TOOL_ITEM(gtk_object_get_data(GTK_OBJECT(menuItem), menuKey)));
}

static bool createMenuProxy(GtkToolItem *toolItem, ZLGtkApplicationWindow *data) {
	GtkToolButton *button = GTK_TOOL_BUTTON(toolItem);

	GtkWidget *menuItem = gtk_image_menu_item_new_with_label(gtk_tool_button_get_label(button));
	gtk_image_menu_item_set_image(
		GTK_IMAGE_MENU_ITEM(menuItem),
		gtk_image_new_from_pixbuf(
			gtk_image_get_pixbuf(
				GTK_IMAGE(gtk_tool_button_get_icon_widget(button))
			)
		)
	);
	gtk_object_set_data(GTK_OBJECT(menuItem), menuKey, toolItem);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(menuItem), "activate", GTK_SIGNAL_FUNC(runMenuItem), data);
	gtk_tool_item_set_proxy_menu_item(toolItem, menuId, menuItem);

	return true;
}

ZLGtkApplicationWindow::Toolbar::Toolbar(ZLGtkApplicationWindow *window) : myWindow(window) {
	myGtkToolbar = GTK_TOOLBAR(gtk_toolbar_new());
	gtk_toolbar_set_style(myGtkToolbar, GTK_TOOLBAR_ICONS);
	gtk_toolbar_set_tooltips(myGtkToolbar, true);
}

GtkWidget *ZLGtkApplicationWindow::Toolbar::toolbarWidget() const {
	return GTK_WIDGET(myGtkToolbar);
}

ZLToolbar::AbstractButtonItem &ZLGtkApplicationWindow::Toolbar::buttonItemByWidget(GtkToolItem *gtkButton) {
	return (ZLToolbar::AbstractButtonItem&)*myGtkToAbstract[gtkButton];
}

void ZLGtkApplicationWindow::Toolbar::setToggleButtonState(const ZLToolbar::ToggleButtonItem &button) {
	GtkToggleToolButton *gtkButton =
		GTK_TOGGLE_TOOL_BUTTON(myAbstractToGtk[&(ZLToolbar::Item&)button]);
	const bool isPressed = button.isPressed();
	if (gtk_toggle_tool_button_get_active(gtkButton) != isPressed) {
		gtk_toggle_tool_button_set_active(gtkButton, isPressed);
	}
}

static void onButtonClicked(GtkWidget *button, gpointer data) {
	((ZLGtkApplicationWindow*)data)->onGtkButtonPress(GTK_TOOL_ITEM(button));
}

GtkToolItem *ZLGtkApplicationWindow::Toolbar::createGtkToolButton(const ZLToolbar::AbstractButtonItem &button) {
	GtkToolItem *gtkItem = 0;
	static std::string imagePrefix =
		ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter;
	GtkWidget *image = gtk_image_new_from_file(
		(imagePrefix + button.iconName() + ".png").c_str()
	);

	switch (button.type()) {
		case ZLToolbar::Item::PLAIN_BUTTON:
			gtkItem = gtk_tool_button_new(image, button.tooltip().c_str());
			break;
		case ZLToolbar::Item::TOGGLE_BUTTON:
			gtkItem = gtk_toggle_tool_button_new();
			gtk_tool_button_set_label(GTK_TOOL_BUTTON(gtkItem), button.tooltip().c_str());
			gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(gtkItem), image);
			break;
		case ZLToolbar::Item::MENU_BUTTON:
		{
			gtkItem = gtk_menu_tool_button_new(image, button.tooltip().c_str());
			const ZLToolbar::MenuButtonItem &menuButton =
				(const ZLToolbar::MenuButtonItem&)button;
			shared_ptr<ZLPopupData> popupData = menuButton.popupData();
			myPopupIdMap[gtkItem] =
				popupData.isNull() ? (size_t)-1 : (popupData->id() - 1);
			gtk_menu_tool_button_set_menu(GTK_MENU_TOOL_BUTTON(gtkItem), gtk_menu_new());
			gtk_menu_tool_button_set_arrow_tooltip(GTK_MENU_TOOL_BUTTON(gtkItem), myGtkToolbar->tooltips, menuButton.popupTooltip().c_str(), 0);
			break;
		}
		default:
			break;
	}

	gtk_tool_item_set_tooltip(gtkItem, myGtkToolbar->tooltips, button.tooltip().c_str(), 0);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(gtkItem), "create_menu_proxy", GTK_SIGNAL_FUNC(createMenuProxy), myWindow);
	ZLGtkSignalUtil::connectSignal(GTK_OBJECT(gtkItem), "clicked", GTK_SIGNAL_FUNC(onButtonClicked), myWindow);

	return gtkItem;
}

void ZLGtkApplicationWindow::Toolbar::addToolbarItem(ZLToolbar::ItemPtr item) {
	GtkToolItem *gtkItem = 0;

	switch (item->type()) {
		case ZLToolbar::Item::TEXT_FIELD:
		case ZLToolbar::Item::COMBO_BOX:
		case ZLToolbar::Item::SEARCH_FIELD:
			{
				const ZLToolbar::ParameterItem &parameterItem = (const ZLToolbar::ParameterItem&)*item;
				GtkEntryParameter *parameter =
					new GtkEntryParameter(*myWindow, parameterItem);
				myWindow->addVisualParameter(parameterItem.parameterId(), parameter);
				gtkItem = parameter->createToolItem();
				gtk_tool_item_set_tooltip(gtkItem, myGtkToolbar->tooltips, parameterItem.tooltip().c_str(), 0);
			}
			break;
		case ZLToolbar::Item::PLAIN_BUTTON:
		case ZLToolbar::Item::TOGGLE_BUTTON:
		case ZLToolbar::Item::MENU_BUTTON:
			gtkItem = createGtkToolButton((const ZLToolbar::AbstractButtonItem&)*item);
			break;
		case ZLToolbar::Item::SEPARATOR:
			gtkItem = gtk_separator_tool_item_new();
			break;
		case ZLToolbar::Item::FILL_SEPARATOR:
			gtkItem = gtk_separator_tool_item_new();
			gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(gtkItem), false);
			gtk_tool_item_set_expand(gtkItem, true);
			break;
	}
	if (gtkItem != 0) {
		gtk_toolbar_insert(myGtkToolbar, gtkItem, -1);
		myAbstractToGtk[&*item] = gtkItem;
		myGtkToAbstract[gtkItem] = item;
		gtk_widget_show_all(GTK_WIDGET(gtkItem));
	}
}

static bool itemActivated(GtkWidget *menuItem, gpointer data) {
	GtkMenu *menu = GTK_MENU(gtk_widget_get_parent(menuItem));
	GList *children = gtk_container_get_children(GTK_CONTAINER(menu));
	int index = g_list_index(children, (gconstpointer)menuItem);
	((ZLPopupData*)data)->run(index);
	return true;
}

void ZLGtkApplicationWindow::Toolbar::updatePopupData(GtkMenuToolButton *button, shared_ptr<ZLPopupData> data) {
	if (data.isNull()) {
		return;
	}

	const size_t id = data->id();
	if (id == myPopupIdMap[GTK_TOOL_ITEM(button)]) {
		return;
	}
	myPopupIdMap[GTK_TOOL_ITEM(button)] = id;

	GtkMenu *menu = GTK_MENU(gtk_menu_tool_button_get_menu(button));
	GList *children = gtk_container_get_children(GTK_CONTAINER(menu));
	if (children != 0) {
		for (GList *ptr =  g_list_last(children); ; ptr = g_list_previous(ptr)) {
			gtk_container_remove(GTK_CONTAINER(menu), GTK_WIDGET(ptr->data));
			if (ptr == children) {
				break;
			}
		}
	}

	const size_t count = data->count();
	for (size_t i = 0; i < count; ++i) {
		GtkWidget *item = gtk_menu_item_new_with_label(data->text(i).c_str());
		gtk_widget_show_all(item);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		ZLGtkSignalUtil::connectSignal(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(itemActivated), &*data);
	}
}

void ZLGtkApplicationWindow::Toolbar::setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled) {
	std::map<const ZLToolbar::Item*,GtkToolItem*>::const_iterator it = myAbstractToGtk.find(&*item);
	if (it == myAbstractToGtk.end()) {
		return;
	}

	GtkToolItem *gtkItem = it->second;
	if (visible) {
		gtk_widget_show(GTK_WIDGET(gtkItem));
	} else {
		gtk_widget_hide(GTK_WIDGET(gtkItem));
	}
	/*
	 * Not sure, but looks like gtk_widget_set_sensitive(WIDGET, false)
	 * does something strange if WIDGET is already insensitive.
	 */
	bool alreadyEnabled =
		(GTK_WIDGET_STATE(GTK_WIDGET(gtkItem)) & GTK_STATE_INSENSITIVE) == 0;
	if (enabled != alreadyEnabled) {
		gtk_widget_set_sensitive(GTK_WIDGET(gtkItem), enabled);
	}

	if (item->type() == ZLToolbar::Item::MENU_BUTTON) {
		updatePopupData(
			GTK_MENU_TOOL_BUTTON(gtkItem),
			((ZLToolbar::MenuButtonItem&)*item).popupData()
		);
	}
}

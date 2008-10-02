/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#include <string.h>

#include "gtkwidgettoolitem.h"

GtkToolItem *gtk_widget_tool_item_new(GtkWidget *widget) {
	GtkWidgetToolItem *widgetItem;

	widgetItem = g_object_new(GTK_TYPE_WIDGET_TOOL_ITEM, NULL);

	widgetItem->widget = widget;
	gtk_container_set_border_width(GTK_CONTAINER(widgetItem), 8);
	gtk_container_add(GTK_CONTAINER(widgetItem), widgetItem->widget);
	gtk_widget_show(widgetItem->widget);

	return GTK_TOOL_ITEM(widgetItem);
}

GtkWidget *gtk_widget_tool_item_get_widget(GtkWidgetToolItem *widgetItem) {
	return widgetItem->widget;
}

static GObjectClass *parent_class = NULL;

static void gtk_widget_tool_item_finalize(GObject *object) {
	GtkWidgetToolItem *widgetItem = GTK_WIDGET_TOOL_ITEM(object);

	g_object_unref(widgetItem->widget);
  
	parent_class->finalize(object);
}

static void gtk_widget_tool_item_size_allocate(GtkWidget *widget, GtkAllocation *allocation) {
  GtkWidgetToolItem *toolitem = GTK_WIDGET_TOOL_ITEM(widget);
  GtkAllocation child_allocation;
  gint border_width;
  GtkWidget *child = toolitem->widget;

  widget->allocation = *allocation;
  border_width = GTK_CONTAINER (widget)->border_width;
  
  if (child && GTK_WIDGET_VISIBLE(child)) {
    child_allocation.x = allocation->x + border_width;
    child_allocation.y = allocation->y + border_width;
    child_allocation.width = allocation->width - 2 * border_width;
    child_allocation.height = allocation->height - 2 * border_width;
    
		if ((child_allocation.width > 0) && (child_allocation.height > 0)) {
    	gtk_widget_size_allocate (child, &child_allocation);
		}
  }
}

static void gtk_widget_tool_item_class_init(GtkWidgetToolItemClass *klass) {
	GObjectClass *object_class;
	GtkWidgetClass *widget_class;
	//GtkToolItemClass *tool_item_class;

	parent_class = g_type_class_peek_parent(klass);

	object_class = (GObjectClass*)klass;
	widget_class = (GtkWidgetClass*)klass;
	//tool_item_class = (GtkToolItemClass*)klass;

	object_class->finalize = gtk_widget_tool_item_finalize;

  widget_class->size_allocate = gtk_widget_tool_item_size_allocate;

	//tool_item_class->create_menu_proxy = gtk_widget_tool_item_create_menu_proxy;
	//tool_item_class->toolbar_reconfigured = gtk_widget_tool_item_toolbar_reconfigured;
}

static void gtk_widget_tool_item_init(GtkWidgetToolItem *widgetItem, GtkWidgetToolItemClass *klass) {
	GtkToolItem *toolitem = GTK_TOOL_ITEM(widgetItem);
	gtk_tool_item_set_homogeneous(toolitem, FALSE);
}

GType gtk_widget_tool_item_get_type() {
	static GtkType type = 0;

	if (type == 0) {
		static const GTypeInfo type_info = {
			sizeof(GtkWidgetToolItemClass),
			(GBaseInitFunc)NULL,
			(GBaseFinalizeFunc)NULL,
			(GClassInitFunc)gtk_widget_tool_item_class_init,
			(GClassFinalizeFunc)NULL,
			NULL,
			sizeof(GtkWidgetToolItem),
			0, /* n_preallocs */
			(GInstanceInitFunc)gtk_widget_tool_item_init,
			0
		};

		type = g_type_register_static(GTK_TYPE_TOOL_ITEM, "GtkWidgetToolItem", &type_info, 0);
	}
	return type;
}

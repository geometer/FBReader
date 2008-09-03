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

#ifndef __GTKWIDGETTOOLITEM_H__
#define __GTKWIDGETTOOLITEM_H__

#include <gtk/gtktoolitem.h>

G_BEGIN_DECLS

#define GTK_TYPE_WIDGET_TOOL_ITEM            (gtk_widget_tool_item_get_type ())
#define GTK_WIDGET_TOOL_ITEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_WIDGET_TOOL_ITEM, GtkWidgetToolItem))
#define GTK_WIDGET_TOOL_ITEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_WIDGET_TOOL_ITEM, GtkWidgetToolItemClass))
#define GTK_IS_WIDGET_TOOL_ITEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_WIDGET_TOOL_ITEM))
#define GTK_IS_WIDGET_TOOL_ITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_WIDGET_TOOL_ITEM))
#define GTK_WIDGET_TOOL_ITEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_WIDGET_TOOL_ITEM, GtkWidgetToolItemClass))

typedef struct _GtkWidgetToolItem        GtkWidgetToolItem;
typedef struct _GtkWidgetToolItemClass   GtkWidgetToolItemClass;

struct _GtkWidgetToolItem {
	GtkToolItem parent;

	GtkWidget *widget;
};

struct _GtkWidgetToolItemClass {
	GtkToolItemClass parent_class;
};

GType          gtk_widget_tool_item_get_type(void) G_GNUC_CONST;
GtkToolItem   *gtk_widget_tool_item_new(GtkWidget *widget);
GtkWidget     *gtk_widget_tool_item_get_widget(GtkWidgetToolItem *item);

G_END_DECLS

#endif /* __GTKWIDGETTOOLITEM_H__ */

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

/*
 * This code is based on the gtktoolbutton code from Gtk+ library:
 *
 * Copyright (C) 2002 Anders Carlsson <andersca@gnome.org>
 * Copyright (C) 2002 James Henstridge <james@daa.com.au>
 * Copyright (C) 2003 Soeren Sandmann <sandmann@daimi.au.dk>
 */

#include <string.h>

#define GETTEXT_PACKAGE "gtk20"
#include <gtk/gtkhbox.h>
#include <gtk/gtkiconfactory.h>
#include <gtk/gtkimage.h>
#include <gtk/gtkimagemenuitem.h>
#include <gtk/gtkstock.h>
#include <gtk/gtktoolbar.h>
#include <gtk/gtkprivate.h>
#include <glib/gi18n-lib.h>

#ifdef ENABLE_NLS
#define P_(String) dgettext(GETTEXT_PACKAGE "-properties",String)
#else 
#define P_(String) (String)
#endif

#include "gtkwidgettoolitem.h"

#define MENU_ID "gtk-widget-tool-item-menu-id"

enum {
  LAST_SIGNAL
};

enum {
  PROP_0,
  PROP_ICON_WIDGET
};

static void gtk_widget_tool_item_init          (GtkWidgetToolItem      *item,
					   GtkWidgetToolItemClass *klass);
static void gtk_widget_tool_item_class_init    (GtkWidgetToolItemClass *klass);
static void gtk_widget_tool_item_set_property  (GObject            *object,
					   guint               prop_id,
					   const GValue       *value,
					   GParamSpec         *pspec);
static void gtk_widget_tool_item_get_property  (GObject            *object,
					   guint               prop_id,
					   GValue             *value,
					   GParamSpec         *pspec);
static void gtk_widget_tool_item_property_notify (GObject          *object,
					     GParamSpec       *pspec);
static void gtk_widget_tool_item_finalize      (GObject            *object);

static void gtk_widget_tool_item_toolbar_reconfigured (GtkToolItem *tool_item);
static gboolean   gtk_widget_tool_item_create_menu_proxy (GtkToolItem     *item);

static void gtk_widget_tool_item_construct_contents (GtkToolItem *tool_item);
static void gtk_widget_tool_item_set_widget (GtkWidgetToolItem *item, GtkWidget *widget);
static GObjectClass *parent_class = NULL;

#define GTK_WIDGET_TOOL_ITEM_GET_PRIVATE(obj)(G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_WIDGET_TOOL_ITEM, GtkWidgetToolItemPrivate))

struct _GtkWidgetToolItemPrivate
{
  GtkWidget *widget;
};

GType
gtk_widget_tool_item_get_type (void)
{
  static GtkType type = 0;

  if (!type)
    {
      static const GTypeInfo type_info =
	{
	  sizeof (GtkWidgetToolItemClass),
	  (GBaseInitFunc) NULL,
	  (GBaseFinalizeFunc) NULL,
	  (GClassInitFunc) gtk_widget_tool_item_class_init,
	  (GClassFinalizeFunc) NULL,
	  NULL,
	  sizeof (GtkWidgetToolItem),
	  0, /* n_preallocs */
	  (GInstanceInitFunc) gtk_widget_tool_item_init,
		0
	};

      type = g_type_register_static (GTK_TYPE_TOOL_ITEM,
				     "GtkWidgetToolItem",
				     &type_info, 0);
    }
  return type;
}

static void
gtk_widget_tool_item_class_init (GtkWidgetToolItemClass *klass)
{
  GObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkToolItemClass *tool_item_class;
  
  parent_class = g_type_class_peek_parent (klass);
  
  object_class = (GObjectClass *)klass;
  widget_class = (GtkWidgetClass *)klass;
  tool_item_class = (GtkToolItemClass *)klass;
  
  object_class->set_property = gtk_widget_tool_item_set_property;
  object_class->get_property = gtk_widget_tool_item_get_property;
  object_class->notify = gtk_widget_tool_item_property_notify;
  object_class->finalize = gtk_widget_tool_item_finalize;

  tool_item_class->create_menu_proxy = gtk_widget_tool_item_create_menu_proxy;
  tool_item_class->toolbar_reconfigured = gtk_widget_tool_item_toolbar_reconfigured;
  
  /* Properties are interpreted like this:
   *
   *          - if the tool button has an widget, then that widget
   *            will be used as the icon. Otherwise, if the tool button
   *            has a stock id, the corresponding stock icon will be
   *            used. Otherwise, if the tool button has an icon name,
   *            the corresponding icon from the theme will be used.
   *            Otherwise, the tool button will not have an icon.
   *
   */

  /**
   * GtkWidgetToolItem:icon-name:
   * 
   * The name of the themed icon displayed on the item.
   * This property only has an effect if not overridden by "label", 
   * "widget" or "stock_id" properties.
   *
   * Since: 2.8 
   */
  g_object_class_install_property (object_class,
				   PROP_ICON_WIDGET,
				   g_param_spec_object ("widget",
							P_("Icon widget"),
							P_("Icon widget to display in the item"),
							GTK_TYPE_WIDGET,
							GTK_PARAM_READWRITE));

  g_type_class_add_private (object_class, sizeof (GtkWidgetToolItemPrivate));
}

static void
gtk_widget_tool_item_init (GtkWidgetToolItem      *item,
		      GtkWidgetToolItemClass *klass)
{
  GtkToolItem *toolitem = GTK_TOOL_ITEM (item);
  gtk_container_set_border_width(GTK_CONTAINER(item), 5);
  
  item->priv = GTK_WIDGET_TOOL_ITEM_GET_PRIVATE (item);

  gtk_tool_item_set_homogeneous (toolitem, TRUE);
}

static void
gtk_widget_tool_item_construct_contents (GtkToolItem *tool_item)
{
  GtkWidgetToolItem *button = GTK_WIDGET_TOOL_ITEM (tool_item);
  GtkWidget *icon = NULL;
  GtkIconSize icon_size;

  if (button->priv->widget && button->priv->widget->parent)
    {
      gtk_container_remove (GTK_CONTAINER (button->priv->widget->parent),
			    button->priv->widget);
    }

  icon_size = gtk_tool_item_get_icon_size (GTK_TOOL_ITEM (button));
  if (button->priv->widget)
    {
      icon = button->priv->widget;

      if (GTK_IS_IMAGE (icon))
        {
          g_object_set (button->priv->widget,
		      "icon-size", icon_size,
		      NULL);
        }
    }

  if (icon)
    gtk_container_add (GTK_CONTAINER (button), icon);

  gtk_tool_item_rebuild_menu (tool_item);
  
  gtk_widget_queue_resize (GTK_WIDGET (button));
}

static void
gtk_widget_tool_item_set_property (GObject         *object,
			      guint            prop_id,
			      const GValue    *value,
			      GParamSpec      *pspec)
{
  GtkWidgetToolItem *button = GTK_WIDGET_TOOL_ITEM (object);
  
  switch (prop_id)
    {
    case PROP_ICON_WIDGET:
      gtk_widget_tool_item_set_widget (button, g_value_get_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtk_widget_tool_item_property_notify (GObject          *object,
				 GParamSpec       *pspec)
{
  if (strcmp (pspec->name, "is-important") == 0)
    gtk_widget_tool_item_construct_contents (GTK_TOOL_ITEM (object));

  if (parent_class->notify)
    parent_class->notify (object, pspec);
}

static void
gtk_widget_tool_item_get_property (GObject         *object,
			      guint            prop_id,
			      GValue          *value,
			      GParamSpec      *pspec)
{
  GtkWidgetToolItem *button = GTK_WIDGET_TOOL_ITEM (object);

  switch (prop_id)
    {
    case PROP_ICON_WIDGET:
      g_value_set_object (value, button->priv->widget);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtk_widget_tool_item_finalize (GObject *object)
{
  GtkWidgetToolItem *button = GTK_WIDGET_TOOL_ITEM (object);

  if (button->priv->widget)
    g_object_unref (button->priv->widget);
  
  parent_class->finalize (object);
}

static GtkWidget *
clone_image_menu_size (GtkImage *image, GtkSettings *settings)
{
  GtkImageType storage_type = gtk_image_get_storage_type (image);

  if (storage_type == GTK_IMAGE_STOCK)
    {
      gchar *stock_id;
      gtk_image_get_stock (image, &stock_id, NULL);
      return gtk_image_new_from_stock (stock_id, GTK_ICON_SIZE_MENU);
    }
  else if (storage_type == GTK_IMAGE_ICON_SET)
    {
      GtkIconSet *icon_set;
      gtk_image_get_icon_set (image, &icon_set, NULL);
      return gtk_image_new_from_icon_set (icon_set, GTK_ICON_SIZE_MENU);
    }
  else if (storage_type == GTK_IMAGE_PIXBUF)
    {
      gint width, height;
      
      if (settings &&
	  gtk_icon_size_lookup_for_settings (settings, GTK_ICON_SIZE_MENU,
					     &width, &height))
	{
	  GdkPixbuf *src_pixbuf, *dest_pixbuf;
	  GtkWidget *cloned_image;

	  src_pixbuf = gtk_image_get_pixbuf (image);
	  dest_pixbuf = gdk_pixbuf_scale_simple (src_pixbuf, width, height,
						 GDK_INTERP_BILINEAR);

	  cloned_image = gtk_image_new_from_pixbuf (dest_pixbuf);
	  g_object_unref (dest_pixbuf);

	  return cloned_image;
	}
    }

  return NULL;
}
      
static gboolean
gtk_widget_tool_item_create_menu_proxy (GtkToolItem *item)
{
  GtkWidgetToolItem *button = GTK_WIDGET_TOOL_ITEM (item);
  GtkWidget *menu_item = NULL;
  GtkWidget *menu_image = NULL;

  if (button->priv->widget && GTK_IS_IMAGE (button->priv->widget))
    {
      menu_image = clone_image_menu_size (GTK_IMAGE (button->priv->widget),
					  gtk_widget_get_settings (GTK_WIDGET (button)));
    }

  if (menu_image)
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_item), menu_image);

  gtk_tool_item_set_proxy_menu_item (GTK_TOOL_ITEM (button), MENU_ID, menu_item);
  
  return TRUE;
}

static void
gtk_widget_tool_item_toolbar_reconfigured (GtkToolItem *tool_item)
{
  gtk_widget_tool_item_construct_contents (tool_item);
}

/**
 * gtk_widget_tool_item_new:
 * @widget: a widget that will be used as icon widget, or %NULL
 * 
 * Creates a new %GtkWidgetToolItem using @widget as icon.
 * 
 * Return value: A new #GtkWidgetToolItem
 * 
 * Since: 2.4
 **/
GtkToolItem *
gtk_widget_tool_item_new (GtkWidget	 *widget)
{
  GtkWidgetToolItem *button;

  button = g_object_new (GTK_TYPE_WIDGET_TOOL_ITEM,
			 NULL);
  
  if (widget)
    gtk_widget_tool_item_set_widget (button, widget);

  return GTK_TOOL_ITEM (button);  
}

/**
 * gtk_widget_tool_item_set_widget:
 * @button: a #GtkWidgetToolItem
 * @widget: the widget used as icon, or %NULL
 * 
 * Sets @icon as the widget used as icon on @button. If @widget is
 * %NULL the icon is determined by the "stock_id" property. If the
 * "stock_id" property is also %NULL, @button will not have an icon.
 * 
 * Since: 2.4
 **/
void
gtk_widget_tool_item_set_widget (GtkWidgetToolItem *item,
				 GtkWidget     *widget)
{
  g_return_if_fail (GTK_IS_WIDGET_TOOL_ITEM (item));
  g_return_if_fail (widget == NULL || GTK_IS_WIDGET (widget));

  if (widget != item->priv->widget)
    {
      if (item->priv->widget)
	{
	  if (item->priv->widget->parent)
	    {
	      gtk_container_remove (GTK_CONTAINER (item->priv->widget->parent),
				    item->priv->widget);
	    }

	  g_object_unref (item->priv->widget);
	}
      
      if (widget)
	{
	  g_object_ref (widget);
	  gtk_object_sink (GTK_OBJECT (widget));
	}

      item->priv->widget = widget;

      gtk_widget_tool_item_construct_contents (GTK_TOOL_ITEM (item));
      
      g_object_notify (G_OBJECT (item), "widget");
    }
}

/**
 * gtk_widget_tool_item_get_widget:
 * @button: a #GtkWidgetToolItem
 * 
 * Return the widget used as icon widget on @button. See
 * gtk_widget_tool_item_set_widget().
 * 
 * Return value: The widget used as icon on @button, or %NULL.
 * 
 * Since: 2.4
 **/
GtkWidget *
gtk_widget_tool_item_get_widget (GtkWidgetToolItem *button)
{
  g_return_val_if_fail (GTK_IS_WIDGET_TOOL_ITEM (button), NULL);

  return button->priv->widget;
}

#define __GTK_WIDGET_TOOL_ITEM_C__

#ifndef __GDK_PIXBUF_HACK_H__
#define __GDK_PIXBUF_HACK_H__

#include <gtk/gtk.h>

struct _GdkPixbuf {
        GObject parent_instance;

	/* Color space */
	GdkColorspace colorspace;

	/* Number of channels, alpha included */
	int n_channels;

	/* Bits per channel */
	int bits_per_sample;

	/* Size */
	int width, height;

	/* Offset between rows */
	int rowstride;

	/* The pixel array */
	guchar *pixels;

	/* Destroy notification function; it is supposed to free the pixel array */
	GdkPixbufDestroyNotify destroy_fn;

	/* User data for the destroy notification function */
	gpointer destroy_fn_data;

	/* Do we have an alpha channel? */
	guint has_alpha : 1;
};

void rotate(GdkPixbuf *dst, const GdkPixbuf *src);

#endif /* __GDK_PIXBUF_HACK_H__ */

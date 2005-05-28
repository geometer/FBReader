#include "gdk-pixbuf-hack.h"

#include <string.h>

#define OFFSET(pb, x, y) ((x) * (pb)->n_channels + (y) * (pb)->rowstride)

void rotate(GdkPixbuf *dst, const GdkPixbuf *src) {
	for (gint y = 0; y < src->height; y++) {
		for (gint x = 0; x < src->width; x++) {
			const guchar *p = src->pixels + OFFSET (src, x, y);
			guchar *q = dst->pixels + OFFSET (dst, y, src->width - x - 1);
			memcpy (q, p, dst->n_channels);
		}
	}
}

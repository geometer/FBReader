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

#include "ZLGtkPixbufHack.h"

#include <string.h>

//
// pixbuf rotation
// (copied from gqview)
//

static void pixbuf_copy_block_rotate(guchar *src, gint src_row_stride, gint x, gint y,
						 guchar *dest, gint dest_row_stride, gint w, gint h,
						 gint bytes_per_pixel, bool counter_clockwise) {
	gint i, j;
	guchar *sp;
	guchar *dp;

	for (i = 0; i < h; ++i) {
		sp = src + ((i + y) * src_row_stride) + (x * bytes_per_pixel);
		for (j = 0; j < w; ++j) {
			if (counter_clockwise) {
				dp = dest + ((w - j - 1) * dest_row_stride) + (i * bytes_per_pixel);
			} else {
				dp = dest + (j * dest_row_stride) + ((h - i - 1) * bytes_per_pixel);
			}
			*(dp++) = *(sp++);	/* r */
			*(dp++) = *(sp++);	/* g */
			*(dp++) = *(sp++);	/* b */
			if (bytes_per_pixel == 4) *(dp) = *(sp++);	/* a */
		}
	}

}

static void pixbuf_copy_block(guchar *src, gint src_row_stride, gint w, gint h,
						guchar *dest, gint dest_row_stride, gint x, gint y, gint bytes_per_pixel) {
	gint i;
	guchar *sp;
	guchar *dp;

	for (i = 0; i < h; ++i) {
		sp = src + (i * src_row_stride);
		dp = dest + ((y + i) * dest_row_stride) + (x * bytes_per_pixel);
		memcpy(dp, sp, w * bytes_per_pixel);
	}
}

#define ROTATE_BUFFER_WIDTH 24
#define ROTATE_BUFFER_HEIGHT 24

/*
 * Returns a copy of pixbuf src rotated 90 degrees clockwise or 90 counterclockwise
 */
void rotate90(GdkPixbuf *dest, const GdkPixbuf *src, bool counter_clockwise) {
	gint has_alpha;
	gint sw, sh, srs;
	gint drs;
	guchar *s_pix;
	guchar *d_pix;
	gint i, j;
	gint a;
	GdkPixbuf *buffer;
	guchar *b_pix;
	gint brs;
	gint w, h;

	if (src == 0)
		return;

	sw = gdk_pixbuf_get_width(src);
	sh = gdk_pixbuf_get_height(src);
	has_alpha = gdk_pixbuf_get_has_alpha(src);
	srs = gdk_pixbuf_get_rowstride(src);
	s_pix = gdk_pixbuf_get_pixels(src);

	drs = gdk_pixbuf_get_rowstride(dest);
	d_pix = gdk_pixbuf_get_pixels(dest);

	a = (has_alpha ? 4 : 3);

	buffer = gdk_pixbuf_new(GDK_COLORSPACE_RGB, has_alpha, 8, ROTATE_BUFFER_WIDTH, ROTATE_BUFFER_HEIGHT);
	b_pix = gdk_pixbuf_get_pixels(buffer);
	brs = gdk_pixbuf_get_rowstride(buffer);

	for (i = 0; i < sh; i+= ROTATE_BUFFER_WIDTH) {
		w = MIN(ROTATE_BUFFER_WIDTH, sh - i);
		for (j = 0; j < sw; j += ROTATE_BUFFER_HEIGHT) {
			gint x, y;

			h = MIN(ROTATE_BUFFER_HEIGHT, sw - j);
			pixbuf_copy_block_rotate(s_pix, srs, j, i, b_pix, brs, h, w, a, counter_clockwise);

			if (counter_clockwise) {
				x = i;
				y = sw - h - j;
			} else {
				x = sh - w - i;
				y = j;
			}

			pixbuf_copy_block(b_pix, brs, w, h, d_pix, drs, x, y, a);
		}
	}

	gdk_pixbuf_unref(buffer);
}

void rotate180(GdkPixbuf *buffer) {
	if (buffer == 0)
		return;

	const gint width = gdk_pixbuf_get_width(buffer);
	if (width <= 1) {
		return;
	}
	const gint height = gdk_pixbuf_get_height(buffer);
	const gint brs = gdk_pixbuf_get_rowstride(buffer);
	guchar *s_pix = gdk_pixbuf_get_pixels(buffer);
	guchar *d_pix = s_pix + (height - 1) * brs;

	const gint a = gdk_pixbuf_get_has_alpha(buffer) ? 4 : 3;

	guchar * const sbuf = new guchar[width * a];
	guchar * const dbuf = new guchar[width * a];
	guchar * const tmp = new guchar[a];

	while (s_pix < d_pix) {
		memcpy(sbuf, s_pix, width * a);
		memcpy(dbuf, d_pix, width * a);
		guchar *s = sbuf;
		guchar *d = dbuf + (width - 1) * a;
		for (int i = 0; i < width; ++i) {
			memcpy(tmp, s, a);
			memcpy(s, d, a);
			memcpy(d, tmp, a);
			s += a;
			d -= a;
		}
		memcpy(s_pix, sbuf, width * a);
		memcpy(d_pix, dbuf, width * a);
		s_pix += brs;
		d_pix -= brs;
	}
	if (s_pix == d_pix) {
		memcpy(sbuf, s_pix, width * a);
		guchar *s = sbuf;
		guchar *d = sbuf + (width - 1) * a;
		while (s < d) {
			memcpy(tmp, s, a);
			memcpy(s, d, a);
			memcpy(d, tmp, a);
			s += a;
			d -= a;
		}
		memcpy(s_pix, sbuf, width * a);
	}

	delete[] sbuf;
	delete[] dbuf;
	delete[] tmp;
}

// vim:ts=2:sw=2:noet

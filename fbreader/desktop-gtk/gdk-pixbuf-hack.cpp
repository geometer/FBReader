#include "gdk-pixbuf-hack.h"

#include <string.h>

#if 0
//
// pixbuf rotation
// (copied from gqview)
//

static void pixbuf_copy_block_rotate(guchar *src, gint src_row_stride, gint x, gint y,
				     guchar *dest, gint dest_row_stride, gint w, gint h,
				     gint bytes_per_pixel, gint counter_clockwise) {
  gint i, j;
  guchar *sp;
  guchar *dp;

  for (i = 0; i < h; i++) {
    sp = src + ((i + y) * src_row_stride) + (x * bytes_per_pixel);
    for (j = 0; j < w; j++) {
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

	for (i = 0; i < h; i++) {
		sp = src + (i * src_row_stride);
		dp = dest + ((y + i) * dest_row_stride) + (x * bytes_per_pixel);
		memcpy(dp, sp, w * bytes_per_pixel);
	}
}

#define ROTATE_BUFFER_WIDTH 48
#define ROTATE_BUFFER_HEIGHT 48

/*
 * Returns a copy of pixbuf src rotated 90 degrees clockwise or 90 counterclockwise
 *
 */
// GdkPixbuf *pixbuf_copy_rotate_90(GdkPixbuf *src, gint counter_clockwise)
void rotate(GdkPixbuf *dest, const GdkPixbuf *src) {
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
	gint counter_clockwise = true;

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
    w = MIN(ROTATE_BUFFER_WIDTH, (sh - i));
    for (j = 0; j < sw; j += ROTATE_BUFFER_HEIGHT) {
      gint x, y;

      h = MIN(ROTATE_BUFFER_HEIGHT, (sw - j));
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

#else
// old hack is below

//#define OFFSET(pb, x, y) ((x) * (pb)->n_channels + (y) * (pb)->rowstride)
#define OFFSET(pb, x, y) ((x) * 3 + (y) * (pb)->rowstride)

void rotate(GdkPixbuf *dst, const GdkPixbuf *src) {
  bool has_alpha = gdk_pixbuf_get_has_alpha(src);
	const guchar *s = src->pixels;
	const gint rowstride = dst->rowstride;
	const gint h = src->height;
	const gint w = src->width;
	guchar *d1 = dst->pixels + rowstride * (src->width - 1);
	if (has_alpha) {
		for (gint y = 0; y < h; y++) {
			for (gint x = 0; x < w; x++) {
				guchar *d = d1 - rowstride * x;
				*d++ = *s++;
				*d++ = *s++;
				*d++ = *s++;
				*d = *s++;
			}
			d1 += 4;
		}
	} else {
		for (gint y = 0; y < h; y++) {
			for (gint x = 0; x < w; x++) {
				guchar *d = d1 - rowstride * x;
				*d++ = *s++;
				*d++ = *s++;
				*d = *s++;
			}
			d1 += 3;
		}
	}
}
#endif

// vim:ts=2:sw=2:noet

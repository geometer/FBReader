#ifndef __GDK_PIXBUF_HACK_H__
#define __GDK_PIXBUF_HACK_H__

#include <gtk/gtk.h>

void rotate(GdkPixbuf *dst, const GdkPixbuf *src, int sw, int sh);

#endif /* __GDK_PIXBUF_HACK_H__ */

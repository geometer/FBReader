/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __GTKIMAGECONVERTER_H__
#define __GTKIMAGECONVERTER_H__

#include <map>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "../../abstract/image/ImageConverter.h"

class ZLString;
class ZLImage;

class GtkImageConverter : public ImageConverter {

public:
	static GdkPixbuf *gtkImage(const ZLImage &image);
	static void clearCache();

private:
	static GdkPixbuf *gtkImageFromPalmImageFormat(const ZLString &imageData);

private:
	static std::map<const ZLImage*,GdkPixbuf*> ourImageMap;
};

#endif /* __GTKIMAGECONVERTER_H__ */

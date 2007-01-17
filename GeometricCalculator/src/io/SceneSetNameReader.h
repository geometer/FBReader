/*
 * Geometric Calculator -- interactive geometry program
 * Copyright (C) 2003-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __SCENESETNAMEREADER_H__
#define __SCENESETNAMEREADER_H__

#include <ZLXMLReader.h>

class ZLFile;

class SceneSetNameReader : public ZLXMLReader {

public:
	std::string readSetName(const ZLFile &file);

private:	
	void startElementHandler(const char *tag, const char **attributes);

private:
	std::string mySetName;
};

#endif /* __SCENESETNAMEREADER_H__ */

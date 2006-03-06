/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "../common/Files.h"
#include "../common/formats/html/HtmlEntityCollection.h"
#include "../common/collection/BookCollection.h"
#include "../common/fbreader/CollectionView.h"
#include "../common/fbreader/FBFileHandler.h"
#include "GtkFBReader.h"
#include "Paths.h"

const std::string Files::PathPrefix = "/var/lib/install/usr/share/FBReader/";
const std::string Files::PathDelimiter = "/";

const std::string FBFileHandler::ImageDirectory = "FBReader";

const std::string HtmlEntityCollection::CollectionFile = Files::PathPrefix + "formats/html/html.ent";
const std::string GtkFBReader::ImageDirectory = Files::PathPrefix + "icons";
const std::string EncodingDescriptionPath = Files::PathPrefix + "encodings";
const std::string CollectionView::DeleteBookImageFile = GtkFBReader::ImageDirectory + "/FBReader/remove.png";
const std::string BookCollection::DefaultBookPath = "~/MyDocs/Books:/media/mmc1/Books";

/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "../common/hyphenation/TeXHyphenator.h"
//#include "../common/formats/docbook/DocBookReader.h"
#include "../common/formats/html/HtmlEntityExtension.h"
#include "../common/collection/BookCollection.h"
#include "QFBReader.h"
#include "Paths.h"

static const std::string PathPrefix = std::string(INSTALLDIR) + "/share/FBReader/";

const std::string TeXHyphenator::PatternZip = PathPrefix + "hyphenationPatterns.zip";
//const std::string DocBookReader::DTDDirectory = "/usr/share/xml/entities/xml-iso-entities-8879.1986";
const std::string HtmlEntityExtension::CollectionFile = PathPrefix + "formats/html/html.ent";
const std::string QFBReader::ImageDirectory = PathPrefix + "icons";
const std::string FBReader::HelpDirectory = PathPrefix + "help";
const std::string EncodingDescriptionPath = PathPrefix + "encodings";

ZLStringOption BookCollection::PathOption("Options", "BookPath", "~/FBooks:~/Books");

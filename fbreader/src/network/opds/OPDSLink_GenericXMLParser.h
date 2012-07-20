/*
 * Copyright (C) 2008-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __OPDSLINK_GENERICXMLPARSER_H__
#define __OPDSLINK_GENERICXMLPARSER_H__

#include "OPDSXMLParser.h"
#include "OPDSLink_GenericFeedReader.h"

class OPDSLink::GenericXMLParser : public OPDSXMLParser {
public:
	GenericXMLParser(shared_ptr<OPDSFeedReader> feedReader);

protected:
	void startElementHandler(const char *tag, const char **attributes);
	OPDSLink::GenericFeedReader &getFeedReader() const;
};

#endif /* __OPDSLINK_GENERICXMLPARSER_H__ */

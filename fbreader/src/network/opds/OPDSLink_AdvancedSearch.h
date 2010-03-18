/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __OPDSLINK_ADVANCEDSEARCH_H__
#define __OPDSLINK_ADVANCEDSEARCH_H__

#include <string>
/*
#include <algorithm>

#include <ZLStringUtil.h>
#include <ZLNetworkUtil.h>
#include <ZLNetworkManager.h>

#include "OPDSLink.h"
#include "OPDSLinkReader.h"
#include "OPDSCatalogItem.h"
#include "OPDSXMLParser.h"
#include "NetworkOPDSFeedReader.h"

#include "../NetworkOperationData.h"
#include "../authentication/NetworkAuthenticationManager.h"

#include "URLRewritingRule.h"
*/

class OPDSLink::AdvancedSearch {

public:
	AdvancedSearch(
		const std::string &type,
		const std::string &titleParameter,
		const std::string &authorParameter,
		const std::string &tagParameter,
		const std::string &annotationParameter
	);

	std::string query(
		const std::string &titleOrSeries,
		const std::string &author,
		const std::string &tag,
		const std::string &annotation
	) const;

private:
	void addSubQuery(std::string &query, const std::string &name, const std::string &value) const;

private:
	const std::string myType;
	const std::string myTitleParameter;
	const std::string myAuthorParameter;
	const std::string myTagParameter;
	const std::string myAnnotationParameter;
};

#endif /* __OPDSLINK_ADVANCEDSEARCH_H__ */

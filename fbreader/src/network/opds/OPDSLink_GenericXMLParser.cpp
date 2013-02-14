/*
 * Copyright (C) 2008-2013 Geometer Plus <contact@geometerplus.com>
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

#include <ZLXMLNamespace.h>

#include "OPDSLink_GenericXMLParser.h"
#include "URLRewritingRule.h"
#include "OPDSLink_AdvancedSearch.h"

static const std::string TAG_ENTRY = "entry";
static const std::string FBREADER_ADVANCED_SEARCH = "advancedSearch";
static const std::string FBREADER_AUTHENTICATION = "authentication";
static const std::string FBREADER_REWRITING_RULE = "urlRewritingRule";
static const std::string FBREADER_RELATION_ALIAS = "relationAlias";
static const std::string FBREADER_EXTRA = "extra";

OPDSLink::GenericXMLParser::GenericXMLParser(shared_ptr<OPDSFeedReader> feedReader) :
	OPDSXMLParser(feedReader) {
}

OPDSLink::GenericFeedReader &OPDSLink::GenericXMLParser::getFeedReader() const {
	return static_cast<OPDSLink::GenericFeedReader&>(*myFeedReader);
}

void OPDSLink::GenericXMLParser::startElementHandler(const char *tag, const char **attributes) {
	switch (myState) {
		case FEED:
			if (testTag(ZLXMLNamespace::Atom, TAG_ENTRY, tag)) {
				getFeedReader().clear();
			}
			break;
		case F_ENTRY:
			if (testTag(ZLXMLNamespace::FBReaderCatalogMetadata, FBREADER_ADVANCED_SEARCH, tag)) {
				const char *style = attributeValue(attributes, "style");
				const char *author = attributeValue(attributes, "author");
				const char *titleOrSeries = attributeValue(attributes, "titleOrSeries");
				const char *tag = attributeValue(attributes, "tag");
				const char *annotation = attributeValue(attributes, "annotation");
				if (style != 0 && author != 0 && titleOrSeries != 0 && tag != 0 && annotation != 0) {
					getFeedReader().setAdvancedSearch(new OPDSLink::AdvancedSearch(style, titleOrSeries, author, tag, annotation));
				}
				return;
			} else if (testTag(ZLXMLNamespace::FBReaderCatalogMetadata, FBREADER_AUTHENTICATION, tag)) {
				const char *type = attributeValue(attributes, "type");
				if (type != 0) {
					getFeedReader().setAuthenticationType(type);
				}
				return;
			} else if (testTag(ZLXMLNamespace::FBReaderCatalogMetadata, FBREADER_RELATION_ALIAS, tag)) {
				const char *name = attributeValue(attributes, "name");
				const char *type = attributeValue(attributes, "type");
				const char *alias = attributeValue(attributes, "alias");
				if (name != 0 && alias != 0) {
					getFeedReader().addRelationAlias(OPDSLink::RelationAlias(alias, (type != 0) ? type : std::string()), name);
				}
			} else if (testTag(ZLXMLNamespace::FBReaderCatalogMetadata, FBREADER_REWRITING_RULE, tag)) {

				getFeedReader().addUrlRewritingRule(new URLRewritingRule(getAttributesMap(attributes)));

//				const char *type  = attributeValue(attributes, "type");
//				const char *apply = attributeValue(attributes, "apply");
//				const char *name  = attributeValue(attributes, "name");
//				const char *value = attributeValue(attributes, "value");

//				//TODO add rewrite type of 'rewriting rules'
//				URLRewritingRule::RuleApply ruleApply = URLRewritingRule::ALWAYS;
//				if (apply != 0) {
//					const std::string applyStr = apply;
//					if (applyStr == "external") {
//						ruleApply = URLRewritingRule::EXTERNAL;
//					} else if (applyStr == "internal") {
//						ruleApply = URLRewritingRule::INTERNAL;
//					} else if (applyStr != "always") {
//						type = 0;
//					}
//				}

//				if (type != 0 && name != 0 && value != 0) {
//					std::string typeStr = type;
//					if (typeStr == "addUrlParameter") {
//						getFeedReader().addUrlRewritingRule(new URLRewritingRule(URLRewritingRule::ADD_URL_PARAMETER, ruleApply, name, value));
//					}
//				}

				return;
			}
			break;
		default:
			break;
	}
	OPDSXMLParser::startElementHandler(tag, attributes);
}


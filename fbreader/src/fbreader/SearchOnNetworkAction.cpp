/*
 * Copyright (C) 2008-2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLStringUtil.h>
#include <ZLDialogManager.h>
#include <ZLDialog.h>
#include <ZLNetworkManager.h>

#include "../options/FBCategoryKey.h"

#include "FBReaderActions.h"

#include "../network/NetworkLink.h"
#include "../network/NetworkLinkCollection.h"
#include "../network/SearchResult.h"
#include "../network/authentication/NetworkAuthenticationManager.h"
#include "../networkActions/NetworkOperationRunnable.h"

static const std::string SEARCH_PARAMETER_ID = "networkSearchPattern";


ShowNetworkLibraryAction::ShowNetworkLibraryAction() : SetModeAction(FBReader::NETWORK_LIBRARY_MODE, FBReader::BOOK_TEXT_MODE) {
}

bool ShowNetworkLibraryAction::isVisible() const {
	return SetModeAction::isVisible() && NetworkLinkCollection::Instance().numberOfEnabledLinks() > 0;
}

SearchOnNetworkAction::SearchOnNetworkAction() : ModeDependentAction(FBReader::NETWORK_LIBRARY_MODE) {
}

void SearchOnNetworkAction::run() {
	NetworkLinkCollection &collection = NetworkLinkCollection::Instance();
	for (size_t i = 0; i < collection.size(); ++i) {
		NetworkLink &link = collection.link(i);
		if (link.OnOption.value()) {
			shared_ptr<NetworkAuthenticationManager> mgr = link.authenticationManager();
			if (!mgr.isNull()) {
				IsAuthorisedRunnable checker(*mgr);
				checker.executeWithUI();
				if (checker.result() == B3_TRUE && mgr->needsInitialization()) {
					InitializeAuthenticationManagerRunnable initializer(*mgr);
					initializer.executeWithUI();
					if (initializer.hasErrors()) {
						LogOutRunnable logout(*mgr);
						logout.executeWithUI();
					}
				}
			}
		}
	}

	doSearch();
}


void SimpleSearchOnNetworkAction::doSearch() {
	FBReader &fbreader = FBReader::Instance();
	const std::string pattern = fbreader.visualParameter(SEARCH_PARAMETER_ID);
	if (pattern.empty()) {
		return;
	}

	if (!NetworkOperationRunnable::tryConnect()) {
		return;
	}

	SimpleSearchRunnable runnable(pattern);
	runnable.executeWithUI();
	runnable.showErrorMessage();
	shared_ptr<NetworkBookCollection> result = runnable.result();

	if (!result.isNull()) {
		std::string summary = makeSummary(pattern);
		SearchResult::setLastSearchResult(summary, result);
	}

	fbreader.invalidateNetworkView();
	fbreader.refreshWindow();
}

void AdvancedSearchOnNetworkAction::doSearch() {
	shared_ptr<ZLDialog> searchDialog = ZLDialogManager::Instance().createDialog(ZLResourceKey("networkSearchDialog"));

	ZLStringOption titleAndSeriesOption(FBCategoryKey::SEARCH, "network", "title", "");
	searchDialog->addOption(ZLResourceKey("titleAndSeries"), titleAndSeriesOption);
	ZLStringOption authorOption(FBCategoryKey::SEARCH, "network", "author", "");
	searchDialog->addOption(ZLResourceKey("author"), authorOption);
	//ZLStringOption seriesOption(FBCategoryKey::SEARCH, "network", "series", "");
	//searchDialog->addOption(ZLResourceKey("series"), seriesOption);
	ZLStringOption categoryOption(FBCategoryKey::SEARCH, "network", "category", "");
	searchDialog->addOption(ZLResourceKey("category"), categoryOption);
	ZLStringOption descriptionOption(FBCategoryKey::SEARCH, "network", "description", "");
	searchDialog->addOption(ZLResourceKey("description"), descriptionOption);
	searchDialog->addButton(ZLResourceKey("go"), true);
	searchDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);

	if (searchDialog->run()) {
		searchDialog->acceptValues();
		searchDialog.reset();
		std::string titleAndSeriesPattern = titleAndSeriesOption.value();
		ZLStringUtil::stripWhiteSpaces(titleAndSeriesPattern);
		std::string authorPattern = authorOption.value();
		ZLStringUtil::stripWhiteSpaces(authorPattern);
		//std::string seriesPattern = seriesOption.value();
		//ZLStringUtil::stripWhiteSpaces(seriesPattern);
		std::string categoryPattern = categoryOption.value();
		ZLStringUtil::stripWhiteSpaces(categoryPattern);
		std::string descriptionPattern = descriptionOption.value();
		ZLStringUtil::stripWhiteSpaces(descriptionPattern);

		if (!titleAndSeriesPattern.empty() ||
				!authorPattern.empty() ||
				//!seriesPattern.empty() ||
				!categoryPattern.empty() ||
				!descriptionPattern.empty()) {

			if (!NetworkOperationRunnable::tryConnect()) {
				return;
			}

			AdvancedSearchRunnable runnable(titleAndSeriesPattern, authorPattern, categoryPattern, descriptionPattern);
			runnable.executeWithUI();
			runnable.showErrorMessage();
			shared_ptr<NetworkBookCollection> result = runnable.result();

			if (!result.isNull()) {
				std::string summary = makeSummary(titleAndSeriesPattern, authorPattern, categoryPattern, descriptionPattern);
				SearchResult::setLastSearchResult(summary, result);
			}

			FBReader::Instance().invalidateNetworkView();
			FBReader::Instance().refreshWindow();
		}
	}
}

std::string SimpleSearchOnNetworkAction::makeSummary(const std::string &pattern) {
	const ZLResource &resource = ZLResource::resource("dialog")["networkSearchDialog"];
	return ZLStringUtil::printf(resource["annotation"].value(), pattern);
}

std::string AdvancedSearchOnNetworkAction::makeSummary(const std::string &titleAndSeries, const std::string &author, const std::string &category, const std::string &description) {
	const ZLResource &resource = ZLResource::resource("dialog")["networkSearchDialog"];

	std::string query;
	appendQueryValue(query, resource["titleAndSeries"].value(), titleAndSeries);
	appendQueryValue(query, resource["author"].value(), author);
	appendQueryValue(query, resource["category"].value(), category);
	appendQueryValue(query, resource["description"].value(), description);

	return ZLStringUtil::printf(resource["annotation"].value(), query);
}

void AdvancedSearchOnNetworkAction::appendQueryValue(std::string &query, const std::string &name, const std::string &value) {
	if (value.empty()) {
		return;
	}
	if (!query.empty()) {
		query.append(", ");
	}
	query.append(name).append("=\"").append(value).append("\"");
}

/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __LITRESBOOKITEM_H__
#define __LITRESBOOKITEM_H__

#include "../NetworkItems.h"

class LitResBookItem : public NetworkBookItem {
public:
	LitResBookItem(
		const NetworkLink &link,
		const std::string &id,
		unsigned int index,
		const std::string &title,
		const std::string &summary,
		const std::string &language,
		const std::string &date,
		const std::vector<AuthorData> &authors,
		const std::vector<std::string> &tags,
		const std::string &seriesTitle,
		unsigned int indexInSeries,
		const UrlInfoCollection &urlByType,
		const std::vector<shared_ptr<BookReference> > references
	);

	std::vector<shared_ptr<NetworkItem> > getRelatedCatalogsItems() const;

protected:
	const ZLResource &resource(const std::string &resourceKey) const;
};

#endif /* __LITRESBOOKITEM_H__ */

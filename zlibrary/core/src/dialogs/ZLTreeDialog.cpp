/*
 * Copyright (C) 2012 Geometer Plus <contact@geometerplus.com>
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

#include "ZLTreeDialog.h"

ZLTreeDialog::ZLTreeDialog(const ZLResource &resource) : myResource(resource) {
}

void ZLTreeDialog::setSearcher(shared_ptr<ZLTreeSearcher> searcher) {
	mySearcher = searcher;
}

void ZLTreeDialog::onNodeBeginInsert(ZLTreeNode */*parent*/, size_t /*index*/) {
}

void ZLTreeDialog::onNodeEndInsert(){
}

void ZLTreeDialog::onNodeBeginRemove(ZLTreeNode */*parent*/, size_t /*index*/) {
}

void ZLTreeDialog::onNodeEndRemove() {
}

const ZLResource &ZLTreeDialog::resource() const {
	return myResource;
}

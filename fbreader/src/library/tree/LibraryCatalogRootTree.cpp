/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#include "LibraryTreeNodes.h"

#include <iostream> //udmv

const ZLTypeId LibraryCatalogRootTree::TYPE_ID(LibraryCatalogTree::TYPE_ID);

const ZLTypeId &LibraryCatalogRootTree::typeId() const {
    return TYPE_ID;
}

LibraryCatalogRootTree::LibraryCatalogRootTree(RootTree *parent, shared_ptr<LibraryNode> node, std::size_t position) :
    LibraryCatalogTree(parent, node, position) {
    //init(); //at old version, init is called when node should be painted (and if initialized yet)
}

const ZLResource &LibraryCatalogRootTree::resource() const{
    return ZLResource::resource("networkView")["libraryItemRootNode"];
}


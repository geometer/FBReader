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

#include <ZLDialogManager.h>

#include "LocalLibrary.h"

LocalLibrary *LocalLibrary::ourInstance = 0;

LocalLibrary &LocalLibrary::Instance() {
    if (ourInstance == 0) {
        ourInstance = new LocalLibrary();
    }
    return *ourInstance;
}

LocalLibrary::LocalLibrary() {
    myDialog = ZLDialogManager::Instance().createTreeDialog("localLibrary", ZLResource::resource("networkView"));
    myRootTree.setDialog(myDialog);
}

void LocalLibrary::showDialog() {
    synchronize();
    myDialog->run(&myRootTree);
}

void LocalLibrary::hideDialog() {
    myDialog->onCloseRequest();
}

void LocalLibrary::refresh() {
    myDialog->onRefresh();
}

void LocalLibrary::synchronize() {
    makeUpToDate();
}

void LocalLibrary::makeUpToDate() {
    if(myRootTree.children ().size () <= 0){
        new LibraryCatalogRootTree(&myRootTree, new FavoritesBooksNode(), 0);
        new LibraryCatalogRootTree(&myRootTree, new RecentBooksNode(), 1);
        new LibraryCatalogRootTree(&myRootTree, new AuthorsCatalogNode(), 2);
        new LibraryCatalogRootTree(&myRootTree, new BooksByTitleNode(), 3);
        new LibraryCatalogRootTree(&myRootTree, new TagsCatalogNode(), 4);
    }
}

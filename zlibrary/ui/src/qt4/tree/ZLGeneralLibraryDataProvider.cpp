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

#include "ZLGeneralLibraryDataProvider.h"

ZLGeneralLibraryDataProvider::ZLGeneralLibraryDataProvider(){
}

ZLNetworkRequest::Listener *ZLGeneralLibraryDataProvider::getData(ZLTreeNode *node){
    return new ChildrenRequestListener(node, 0, this);
}

ZLNetworkRequest::Listener *ZLGeneralLibraryDataProvider::getMoreData(ZLTreeNode *node){
    return new ChildrenRequestListener(node, 1, this);
}

ZLNetworkRequest::Listener *ZLGeneralLibraryDataProvider::getRefreshedData(ZLTreeNode *node){
    return new ChildrenRequestListener(node, 2, this);
}

ZLGeneralLibraryDataProvider::ChildrenRequestListener::ChildrenRequestListener(ZLTreeNode *node, int mode, ZLGeneralLibraryDataProvider *dataProvider) :
    myNode(node), myMoreMode(mode), myDataProvider(dataProvider) {
}

void ZLGeneralLibraryDataProvider::ChildrenRequestListener::finished(const std::string &error) {
    if (!error.empty()) {
        //TODO show error message?
    }
    std::cout << "@@@ ChildrenRequestListener::finished " << myMoreMode << std::endl;
    if(myMoreMode == 0){
        myDataProvider->getParent()->onChildrenLoaded(myNode, true, error.empty());
    }else if(myMoreMode == 1){
        myDataProvider->getParent()->onMoreChildrenLoaded(error.empty());
    }else{
        myDataProvider->getParent()->onRefreshChildrenLoaded(error.empty());
    }
}


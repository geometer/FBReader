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

#ifndef ZLGENERALLIBRARYDATAPROVIDER_H
#define ZLGENERALLIBRARYDATAPROVIDER_H

#include "ZLTreeDataProvider.h"
#include <iostream> //udmv

class ZLGeneralLibraryDataProvider : public ZLTreeDataProvider
{
public:
    ZLGeneralLibraryDataProvider();
    ZLNetworkRequest::Listener *getData(ZLTreeNode *node);
    ZLNetworkRequest::Listener *getMoreData(ZLTreeNode *node);
    ZLNetworkRequest::Listener *getRefreshedData(ZLTreeNode *node);

private:
    ZLQtTreeDialog *getParent() {return myDialog;}
    class ChildrenRequestListener : public ZLNetworkRequest::Listener {
        public:
            ChildrenRequestListener(ZLTreeNode *node, int mode, ZLGeneralLibraryDataProvider *dataProvider);
            void finished(const std::string &error);

        private:
            ZLTreeNode *myNode;
            int myMoreMode;
            ZLGeneralLibraryDataProvider *myDataProvider;
    };

friend class ChildrenRequestListener;
};


#endif // ZLGENERALLIBRARYDATAPROVIDER_H

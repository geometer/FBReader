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

#ifndef ZLTREEDATAPROVIDER_H
#define ZLTREEDATAPROVIDER_H

#include <ZLTreeNode.h>
#include <ZLNetworkRequest.h>
#include "../dialogs/ZLQtTreeDialog.h"

class ZLQtTreeDialog;

class ZLTreeDataProvider{
public:
    ZLTreeDataProvider();
    virtual ZLNetworkRequest::Listener *getData(ZLTreeNode *node) = 0;
    virtual ZLNetworkRequest::Listener *getMoreData(ZLTreeNode *node) = 0;
    virtual ZLNetworkRequest::Listener *getRefreshedData(ZLTreeNode *node) = 0;
    void setParent(ZLQtTreeDialog *dialog);

protected:
    ZLQtTreeDialog *myDialog;
};

#endif // ZLTREEDATAPROVIDER_H

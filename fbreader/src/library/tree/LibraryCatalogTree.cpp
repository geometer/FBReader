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

#include <algorithm>

#include <ZLDialogManager.h>
#include "LibraryTreeNodes.h"

#include <iostream> //udmv

const ZLTypeId LibraryCatalogTree::TYPE_ID(LibraryTree::TYPE_ID);

const ZLTypeId &LibraryCatalogTree::typeId() const {
    return TYPE_ID;
}

LibraryCatalogTree::LibraryCatalogTree(RootTree *parent, shared_ptr<LibraryNode> node, std::size_t position) :
    LibraryTree(parent, position), myNode(node){
}

LibraryCatalogTree::LibraryCatalogTree(LibraryTree *parent, shared_ptr<LibraryNode> node, std::size_t position) :
    LibraryTree(parent, position), myNode(node){
    //init();
}

const ZLResource &LibraryCatalogTree::resource() const{
    return ZLResource::resource("networkView")["libraryItemRootNode"];
}

std::string LibraryCatalogTree::title() const{
    if(!myNode.isNull()){
        return myNode->getTitle();
    }else{
        return "";
    }
}
std::string LibraryCatalogTree::subtitle() const{
    if(!myNode.isNull()){
        return myNode->getSubTitle();
    }else{
        return "";
    }
}

shared_ptr<LibraryNode> LibraryCatalogTree::getNode() const{
    return myNode;
}

shared_ptr<const ZLImage> LibraryCatalogTree::image() const {
    if(!myNode.isNull ()){
        return FBTree::defaultCoverImage(myNode->getImageName ());
    }else{
        return FBTree::defaultCoverImage("booktree-catalog.png");
    }
}

void LibraryCatalogTree::requestChildren(shared_ptr<ZLNetworkRequest::Listener> listener){
    myListeners.push_back(listener);
    if (myListeners.size() > 1) {
        return;
    }
    if(!myNode.isNull()){
        myNode->getChildren(this);
    }
}

void LibraryCatalogTree::requestMoreChildren(shared_ptr<ZLNetworkRequest::Listener> listener){
    myListeners.push_back(listener);
    if (myListeners.size() > 1) {
        return;
    }
    if(!myNode.isNull()){
        myNode->getMoreChildren(this);
    }
}

void LibraryCatalogTree::onChildrenReceived(const BookList &childrens, const std::string &error, const std::size_t startIndex) {
    if(startIndex == 0){
        this->clear ();
    }
    for (std::size_t i = 0; i < childrens.size(); ++i) {
        new LibraryBookTree(this, childrens.at(i));
    }
    notifyListeners(error);
}

void LibraryCatalogTree::onChildrenReceived(const AuthorList &childrens, const std::string &error, const std::size_t startIndex) {
    if(startIndex == 0){
        this->clear ();
    }
    std::size_t index = startIndex;
    for (std::size_t i = 0; i < childrens.size(); ++i) {
        new LibraryCatalogTree(this, new BooksByAuthorNode(childrens.at(i)), index++);
    }
    notifyListeners(error);
}

void LibraryCatalogTree::onChildrenReceived(const TagList &childrens, const std::string &error, const std::size_t startIndex) {
    if(startIndex == 0){
        this->clear ();
    }
    std::size_t index = startIndex;
    for (std::size_t i = 0; i < childrens.size(); ++i) {
        new LibraryCatalogTree(this, new BooksByTagNode(childrens.at(i)), index++);
    }
    notifyListeners(error);
}

void LibraryCatalogTree::onChildrenReceived(std::vector<shared_ptr<LibraryNode> > &childrens, const std::string &error, const std::size_t startIndex){
    if(startIndex == 0){
        this->clear ();
    }
    std::size_t index = startIndex;
    for (std::size_t i = 0; i < childrens.size(); ++i) {
        new LibraryCatalogTree(this, childrens.at(i), index++);
    }
    notifyListeners(error);
}

void LibraryCatalogTree::notifyListeners(const std::string &error) {
    for (std::size_t i = 0; i < myListeners.size(); ++i) {
        if (!myListeners.at(i).isNull())
            myListeners.at(i)->finished(error);
    }
    myListeners.clear();
}

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

#include "LibraryNodes.h"

#include <iostream> //udmv

LibraryNode::LibraryNode(){
}
std::string LibraryNode::getTitle(){
    return "";
}
std::string LibraryNode::getSubTitle(){
     return "";
}

std::string LibraryNode::getImageName(){
    return "booktree-catalog.png";
}

FavoritesBooksNode::FavoritesBooksNode() : LibraryNode(){
}
void FavoritesBooksNode::getChildren(LibraryCatalogTree *tree){
    const BookList &bl =  Library::Instance().recentBooks();
    tree->onChildrenReceived(bl, "");
}
std::string FavoritesBooksNode::getTitle(){
    return "Favorites";
}
std::string FavoritesBooksNode::getSubTitle(){
     return "Favorite books";
}

RecentBooksNode::RecentBooksNode() : LibraryNode(){
}
void RecentBooksNode::getChildren(LibraryCatalogTree *tree){
    const BookList &bl =  Library::Instance().recentBooks();
    tree->onChildrenReceived(bl, "");
}
std::string RecentBooksNode::getTitle(){
    return "Recent";
}
std::string RecentBooksNode::getSubTitle(){
     return "Recent books";
}

AuthorsCatalogNode::AuthorsCatalogNode() : LibraryNode(){
}
void AuthorsCatalogNode::getChildren(LibraryCatalogTree *tree){
    const AuthorList &bl =  Library::Instance().authors();
    tree->onChildrenReceived(bl, "");
}
std::string AuthorsCatalogNode::getTitle(){
    return "By Author";
}
std::string AuthorsCatalogNode::getSubTitle(){
     return "Books sorted by author";
}
std::string AuthorsCatalogNode::getImageName(){
    return "booktree-author.png";
}

BooksByAuthorNode::BooksByAuthorNode(shared_ptr<Author> author) : LibraryNode(), myAuthor(author){
}
void BooksByAuthorNode::getChildren(LibraryCatalogTree *tree){
    const BookList &bl =  Library::Instance().books(myAuthor);
    tree->onChildrenReceived(bl, "");
}
std::string BooksByAuthorNode::getTitle(){
    return myAuthor->name ();
}
std::string BooksByAuthorNode::getSubTitle(){
    return myAuthor->name ()+"'s books";
}
std::string BooksByAuthorNode::getImageName(){
    return "booktree-author.png";
}

TagsCatalogNode::TagsCatalogNode() : LibraryNode(){
}
void TagsCatalogNode::getChildren(LibraryCatalogTree *tree){
    const TagList &list = Library::Instance().tags ();
    tree->onChildrenReceived(list, "");
}
std::string TagsCatalogNode::getTitle(){
    return "By Tag";
}
std::string TagsCatalogNode::getSubTitle(){
     return "Books sorted by tag";
}
std::string TagsCatalogNode::getImageName(){
    return "booktree-tag.png";
}

BooksByTagNode::BooksByTagNode(shared_ptr<Tag> tag) : LibraryNode(), myTag(tag){
}
void BooksByTagNode::getChildren(LibraryCatalogTree *tree){
    const BookList &list =  Library::Instance().books(myTag);
    tree->onChildrenReceived(list, "");
}
std::string BooksByTagNode::getTitle(){
    return myTag->name ();
}
std::string BooksByTagNode::getSubTitle(){
    return myTag->name ()+"'s books";
}
std::string BooksByTagNode::getImageName(){
    return "booktree-tag.png";
}

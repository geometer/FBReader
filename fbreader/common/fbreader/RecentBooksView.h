/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#ifndef __RECENTBOOKSVIEW_H__
#define __RECENTBOOKSVIEW_H__

#include <map>

#include "../textview/TextView.h"
#include "../description/BookDescription.h"
#include "../collection/BookCollection.h"

class FBReader;
class PlainTextModel;
class Paragraph;

class RecentBooksView : public TextView {

public:
  RecentBooksView(FBReader &reader, ZLPaintContext &context);
  ~RecentBooksView();
  const std::string &caption() const;

  void rebuild();
  bool onStylusPress(int x, int y);

  void paint();

  LastOpenedBooks &lastBooks();

private:
  FBReader &myReader;
  LastOpenedBooks myLastBooks;
  PlainTextModel *myLastBooksModel;
};

inline LastOpenedBooks &RecentBooksView::lastBooks() {
  return myLastBooks;
}

#endif /* __RECENTBOOKSVIEW_H__ */

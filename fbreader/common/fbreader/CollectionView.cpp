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

#include <abstract/ZLFileImage.h>
#include <abstract/ZLDialogManager.h>

#include "CollectionView.h"
#include "FBReader.h"

#include "../model/TextModel.h"
#include "../model/Paragraph.h"

#include "../textview/ParagraphCursor.h"

#include "../collection/BookCollection.h"
#include "../collection/BookList.h"
#include "../description/BookDescription.h"
#include "../description/Author.h"

static const std::string LIBRARY = "Library";
static const std::string deleteImageId = "deleteImage";

CollectionView::CollectionView(FBReader &reader, ZLPaintContext &context) : FBView(reader, context) {
  myTreeModel = 0;
  myTreeStateIsFrozen = false;
  myImageMap[deleteImageId] = new ZLFileImage("image/png", DeleteBookImageFile, 0);
}

CollectionView::~CollectionView() {
  if (myTreeModel != 0) {
    delete myTreeModel;
  }

  for (ImageMap::iterator image = myImageMap.begin(); image != myImageMap.end(); ++image) {
    if (image->second != 0) {
      delete image->second;
    }
  }
}

const std::string &CollectionView::caption() const {
  return LIBRARY;
}

void CollectionView::gotoParagraph(int num, bool last) {
  if ((num >= 0) && (num < (int)myTreeModel->paragraphsNumber())) {
    TreeParagraph *tp = (TreeParagraph*)(*myTreeModel)[num];
    if (myTreeStateIsFrozen) {
      TreeParagraph *parent = tp->parent();
      while ((num > 0) && (parent != 0) && !parent->isOpen()) {
        for (--num; ((num > 0) && parent != (*myTreeModel)[num]); --num);
        parent = parent->parent();
      }
    } else {
      tp->openTree();
      rebuildPaintInfo(true);
    }
  }
  
  TextView::gotoParagraph(num, last);
}

void CollectionView::paint() {
  if (!myCollection.isSynchronized()) {
    rebuild();
  }
  if (myTreeModel == 0) {
    myTreeModel = new TreeModel();
    const std::vector<AuthorPtr> &authors = myCollection.authors();
    for (std::vector<AuthorPtr>::const_iterator it = authors.begin(); it != authors.end(); ++it) {
      const Books &books = myCollection.books(*it);
      if (!books.empty()) {
        TreeParagraph *authorParagraph = myTreeModel->createParagraph();
        myTreeModel->addControl(LIBRARY_AUTHOR_ENTRY, true);
        myTreeModel->addText((*it)->displayName());
        for (Books::const_iterator jt = books.begin(); jt != books.end(); ++jt) {
          TreeParagraph *bookParagraph = myTreeModel->createParagraph(authorParagraph);
          myTreeModel->addControl(LIBRARY_BOOK_ENTRY, true);
          myTreeModel->addText((*jt)->title());
          if (myCollection.isBookExternal(*jt)) {
            myTreeModel->addText(" ");
            myTreeModel->addImage(deleteImageId, myImageMap);
          }
          myBooksMap[bookParagraph] = *jt;
        }
      }
    }
    setModel(myTreeModel, LIBRARY);
  }
  TextView::paint();
}

void CollectionView::rebuild() {
  setModel(0, LIBRARY);
  myBooksMap.clear();
  if (myTreeModel != 0) {
    delete myTreeModel;
    myTreeModel = 0;
  }
  myCollection.rebuild();
}

bool CollectionView::onStylusPress(int x, int y) {
  myTreeStateIsFrozen = true;
  bool processedByParent = TextView::onStylusPress(x, y);
  myTreeStateIsFrozen = false;
  if (processedByParent) {
    return true;
  }

  const TextElementPosition *imagePosition = elementByCoordinates(x, y);
  if ((imagePosition != 0) && (imagePosition->Kind == TextElement::IMAGE_ELEMENT)) {
    int paragraphNumber = imagePosition->ParagraphNumber;
    if ((paragraphNumber < 0) || ((int)model()->paragraphsNumber() <= paragraphNumber)) {
      return false;
    }
    TreeParagraph *paragraph = (TreeParagraph*)(*myTreeModel)[paragraphNumber];
    std::map<Paragraph*,BookDescriptionPtr>::iterator it = myBooksMap.find(paragraph);
    if (it != myBooksMap.end()) {
      BookDescription &description = *it->second;
      const std::string question = "Remove Book\n\"" + description.title() + "\"\nfrom library?";
      if (ZLDialogManager::instance().questionBox("Remove Book", question, "Yes", "No") == 0) {
        BookList().removeFileName(description.fileName());
        myTreeModel->removeParagraph(paragraphNumber);
        rebuildPaintInfo(true);
        repaintView();
      }
    }
    return true;
  }

  const ParagraphPosition *position = paragraphByCoordinate(y);
  if (position == 0) {
    return false;
  }

  int paragraphNumber = position->ParagraphNumber;
  if ((paragraphNumber < 0) || ((int)model()->paragraphsNumber() <= paragraphNumber)) {
    return false;
  }

  TreeParagraph *paragraph = (TreeParagraph*)(*myTreeModel)[paragraphNumber];
  if (!paragraph->children().empty()) {
    const TextElementPosition *elementPosition = elementByCoordinates(x, y);
    if ((elementPosition == 0) || (elementPosition->Kind != TextElement::TREE_ELEMENT)) {
      return false;
    }

    paragraph->open(!paragraph->isOpen());
    rebuildPaintInfo(true);
    preparePaintInfo();
    if (paragraph->isOpen()) {
      // TODO: correct next paragraph number calculation for multi-level trees
      int nextParagraphNumber = paragraphNumber + paragraph->children().size() + 1;
      int lastParagraphNumber = endCursor().paragraphCursor().index();
      if (endCursor().isEndOfParagraph()) {
        ++lastParagraphNumber;
      }
      if (lastParagraphNumber < nextParagraphNumber) {
        gotoParagraph(nextParagraphNumber, true);
        preparePaintInfo();
      }
    }
    int firstParagraphNumber = startCursor().paragraphCursor().index();
    if (startCursor().isStartOfParagraph()) {
      --firstParagraphNumber;
    }
    if (firstParagraphNumber >= paragraphNumber) {
      gotoParagraph(paragraphNumber);
      preparePaintInfo();
    }
    repaintView();
  } else {
    std::map<Paragraph*,BookDescriptionPtr>::iterator it = myBooksMap.find(paragraph);
    if (it != myBooksMap.end()) {
      myReader.openBook(it->second);
      myReader.showBookTextView();
    }
  }

  return true;
}

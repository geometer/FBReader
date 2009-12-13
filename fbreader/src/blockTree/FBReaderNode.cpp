/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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

#include <ZLResource.h>
#include <ZLibrary.h>
#include <ZLFileImage.h>

#include "FBReaderNode.h"

#include "../fbreader/FBReader.h"
#include "../options/FBOptions.h"
#include "../options/FBTextStyle.h"

class FBReaderNode::ExpandTreeAction : public ZLRunnable {

public:
	ExpandTreeAction(FBReaderNode &node);
	void run();

private:
	FBReaderNode &myNode;
};

std::map<std::string,shared_ptr<ZLImage> > FBReaderNode::ourDefaultCovers;

FBReaderNode::FBReaderNode(ZLBlockTreeNode *parent, size_t atPosition) : ZLBlockTreeNode(parent, atPosition), myCoverImageIsStored(false) {
}

FBReaderNode::~FBReaderNode() {
}

shared_ptr<ZLImage> FBReaderNode::coverImage() const {
	if (!myCoverImageIsStored) {
		myCoverImageIsStored = true;
		myStoredCoverImage = extractCoverImage();
	}
	return myStoredCoverImage;
}

void FBReaderNode::drawCover(ZLPaintContext &context, int vOffset) {
	shared_ptr<ZLImage> cover = coverImage();
	if (cover.isNull()) {
		return;
	}

	shared_ptr<ZLImageData> coverData = ZLImageManager::Instance().imageData(*cover);
	if (coverData.isNull()) {
		return;
	}

	const FBTextStyle &style = FBTextStyle::Instance();
	const int unit = context.fontSizeToPixels(style.fontSize());
	const int h = unit * 9 / 2, w = h * 3 / 4;
	vOffset += unit / 2;
	const int hOffset = level() * unit * 3 - unit * 2;

	const int origWidth = context.imageWidth(*coverData);
	const int origHeight = context.imageHeight(*coverData);
	if (origWidth == 0 || origHeight == 0) {
		return;
	}

	int coeff = std::min(w / origWidth, h / origHeight);
	if (coeff == 0) {
		coeff = 1;
	}
	int width = coeff * origWidth;
	int height = coeff * origHeight;
	if (width > w || height > h) {
		width = context.imageWidth(*coverData, w, h, ZLPaintContext::SCALE_REDUCE_SIZE);
		height = context.imageHeight(*coverData, w, h, ZLPaintContext::SCALE_REDUCE_SIZE);
	}
	context.drawImage(hOffset + (w - width) / 2, vOffset + (h + height) / 2, *coverData, width, height, ZLPaintContext::SCALE_FIT_TO_SIZE);
}

void FBReaderNode::drawTitle(ZLPaintContext &context, int vOffset, const std::string &text, bool highlighted) {
	const FBTextStyle &style = FBTextStyle::Instance();
	const int unit = context.fontSizeToPixels(style.fontSize());
	const int hOffset = level() * unit * 3 + unit * 2;

	context.setColor(highlighted ?
		FBOptions::Instance().colorOption(ZLTextStyle::HIGHLIGHTED_TEXT).value() :
		FBOptions::Instance().RegularTextColorOption.value());
	context.setFont(style.fontFamily(), style.fontSize(), style.bold(), style.italic());

	context.drawString(hOffset, vOffset + 2 * unit, text.data(), text.size(), false);
}

void FBReaderNode::drawSummary(ZLPaintContext &context, int vOffset, const std::string &text, bool highlighted) {
	const FBTextStyle &style = FBTextStyle::Instance();
	const int unit = context.fontSizeToPixels(style.fontSize());
	const int hOffset = level() * unit * 3 + unit * 2;

	context.setColor(highlighted ?
		FBOptions::Instance().colorOption(ZLTextStyle::HIGHLIGHTED_TEXT).value() :
		FBOptions::Instance().RegularTextColorOption.value());
	context.setFont(style.fontFamily(), style.fontSize() * 2 / 3, style.bold(), style.italic());

	context.drawString(hOffset, vOffset + 13 * unit / 4, text.data(), text.size(), false);
}

void FBReaderNode::internalDrawHyperlink(ZLPaintContext &context, int &hOffset, int &vOffset, const std::string &text, shared_ptr<ZLRunnable> action, bool aux) {
	if (action.isNull()) {
		return;
	}

	// aux makes font size and hOffset to be 80% of their normal sizes

	const FBTextStyle &style = FBTextStyle::Instance();
	const int unit = context.fontSizeToPixels(style.fontSize());
	const int h = aux ? (unit * 11 / 2) : (unit * 9 / 2);
	const int left = hOffset + level() * unit * 3 + unit * 2;

	context.setColor(FBOptions::Instance().colorOption("internal").value());
	context.setFont(
		style.fontFamily(), 
		aux ? (16 * style.fontSize() / 30) : (style.fontSize() * 2 / 3), 
		style.bold(), 
		style.italic()
	);

	const int stringW = context.stringWidth(text.data(), text.size(), false);
	const int stringH = context.stringHeight();
	context.drawString(left, vOffset + h, text.data(), text.size(), false);
	addHyperlink(left, h - stringH, left + stringW, h, action);
	hOffset += stringW + (aux ? (16 * context.spaceWidth() / 5) : (4 * context.spaceWidth()));
}

void FBReaderNode::drawHyperlink(ZLPaintContext &context, int &hOffset, int &vOffset, const std::string &text, shared_ptr<ZLRunnable> action) {
	internalDrawHyperlink(context, hOffset, vOffset, text, action, false);
}

void FBReaderNode::drawAuxHyperlink(ZLPaintContext &context, int &hOffset, int &vOffset, const std::string &text, shared_ptr<ZLRunnable> action) {
	internalDrawHyperlink(context, hOffset, vOffset, text, action, true);
}

FBReaderNode::ExpandTreeAction::ExpandTreeAction(FBReaderNode &node) : myNode(node) {
}

void FBReaderNode::ExpandTreeAction::run() {
	myNode.expandOrCollapseSubtree();
	FBReader::Instance().refreshWindow();
}

void FBReaderNode::expandOrCollapseSubtree() {
	if (isOpen()) {
		open(false);
	} else if (!children().empty()) {
		open(true);
		if (view().visibilityMode(this) != ZLBlockTreeView::INVISIBLE) {
			ZLBlockTreeNode *lastChild = children().back();
			while (view().visibilityMode(lastChild) != ZLBlockTreeView::VISIBLE &&
						 this != view().firstVisibleNode()) {
				view().setFirstVisibleNode(view().firstVisibleNode()->next());
			}
		}
	}
}

shared_ptr<ZLRunnable> FBReaderNode::expandTreeAction() {
	if (myExpandTreeAction.isNull()) {
		myExpandTreeAction = new ExpandTreeAction(*this);
	}
	return myExpandTreeAction;
}

shared_ptr<ZLImage> FBReaderNode::defaultCoverImage(const std::string &id) {
	shared_ptr<ZLImage> cover = ourDefaultCovers[id];
	if (cover.isNull()) {
		cover = new ZLFileImage("image/auto",
			ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + id,
			0);
		ourDefaultCovers[id] = cover;
	}
	return cover;
}

bool FBReaderNode::hasAuxHyperlink() const {
	return false;
}

int FBReaderNode::height(ZLPaintContext &context) const {
	return
		context.fontSizeToPixels(FBTextStyle::Instance().fontSize()) *
		(hasAuxHyperlink() ? 13 : 11) / 2;
}

/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

const ZLTypeId FBReaderNode::TYPE_ID(ZLBlockTreeNode::TYPE_ID);

class FBReaderNode::ExpandTreeAction : public ZLRunnableWithKey {

public:
	ExpandTreeAction(FBReaderNode &node);
	void run();
	ZLResourceKey key() const;

private:
	FBReaderNode &myNode;
};

std::map<std::string,shared_ptr<ZLImage> > FBReaderNode::ourDefaultCovers;

FBReaderNode::FBReaderNode(ZLBlockTreeNode *parent, size_t atPosition) : ZLBlockTreeNode(parent, atPosition), myCoverImageIsStored(false), myIsInitialized(false) {
}

void FBReaderNode::init() {
}

bool FBReaderNode::highlighted() const {
	return false;
}

FBReaderNode::~FBReaderNode() {
}

const ZLTypeId &FBReaderNode::typeId() const {
	return TYPE_ID;
}

shared_ptr<ZLImage> FBReaderNode::coverImage() const {
	if (!myCoverImageIsStored) {
		myCoverImageIsStored = true;
		myStoredCoverImage = extractCoverImage();
	}
	return myStoredCoverImage;
}

void FBReaderNode::drawCover(ZLPaintContext &context, int vOffset) {
	drawCoverReal(context, vOffset);
}

void FBReaderNode::drawCoverReal(ZLPaintContext &context, int vOffset) {
	shared_ptr<ZLImage> cover = coverImage();
	if (cover.isNull()) {
		return;
	}

	shared_ptr<ZLImageData> coverData = ZLImageManager::Instance().imageData(*cover);
	if (coverData.isNull()) {
		return;
	}

	const FBTextStyle &style = FBTextStyle::Instance();
	const int unit = unitSize(context, style);
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

void FBReaderNode::drawTitle(ZLPaintContext &context, int vOffset) {
	const FBTextStyle &style = FBTextStyle::Instance();
	const int unit = unitSize(context, style);
	const int hOffset = level() * unit * 3 + unit * 2;

	context.setColor(highlighted() ?
		FBOptions::Instance().colorOption(ZLTextStyle::HIGHLIGHTED_TEXT).value() :
		FBOptions::Instance().RegularTextColorOption.value());
	context.setFont(style.fontFamily(), style.fontSize(), style.bold(), style.italic());

	const std::string text = title();
	context.drawString(hOffset, vOffset + 2 * unit, text.data(), text.size(), false);
}

void FBReaderNode::drawSummary(ZLPaintContext &context, int vOffset) {
	const std::string text = summary();
	if (text.empty()) {
		return;
	}

	const FBTextStyle &style = FBTextStyle::Instance();
	const int unit = unitSize(context, style);
	const int hOffset = level() * unit * 3 + unit * 2;

	context.setColor(highlighted() ?
		FBOptions::Instance().colorOption(ZLTextStyle::HIGHLIGHTED_TEXT).value() :
		FBOptions::Instance().RegularTextColorOption.value());
	context.setFont(style.fontFamily(), style.fontSize() * 2 / 3, style.bold(), style.italic());

	context.drawString(hOffset, vOffset + 13 * unit / 4, text.data(), text.size(), false);
}

void FBReaderNode::drawHyperlink(ZLPaintContext &context, int &hOffset, int &vOffset, shared_ptr<ZLRunnableWithKey> action, bool auxiliary) {
	// auxiliary makes font size and hSkip to be 70% of their normal sizes
	if (action.isNull() || !action->makesSense()) {
		return;
	}

	const FBTextStyle &style = FBTextStyle::Instance();
	const int unit = unitSize(context, style);
	const int h = auxiliary ? (unit * 11 / 2) : (unit * 9 / 2);
	const int left = hOffset + level() * unit * 3 + unit * 2;

	context.setColor(FBOptions::Instance().colorOption("internal").value());
	context.setFont(
		style.fontFamily(), 
		auxiliary ? (7 * style.fontSize() / 15) : (style.fontSize() * 2 / 3), 
		style.bold(), 
		style.italic()
	);

	const std::string text = action->text(resource());
	const int stringW = context.stringWidth(text.data(), text.size(), false);
	const int stringH = context.stringHeight();
	context.drawString(left, vOffset + h, text.data(), text.size(), false);
	addHyperlink(left, h - stringH, left + stringW, h, action);
	hOffset += stringW + 4 * context.spaceWidth();
}

FBReaderNode::ExpandTreeAction::ExpandTreeAction(FBReaderNode &node) : myNode(node) {
}

void FBReaderNode::ExpandTreeAction::run() {
	myNode.expandOrCollapseSubtree();
	FBReader::Instance().refreshWindow();
}

ZLResourceKey FBReaderNode::ExpandTreeAction::key() const {
	return ZLResourceKey(myNode.isOpen() ? "collapseTree" : "expandTree");
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

void FBReaderNode::registerAction(shared_ptr<ZLRunnableWithKey> action, bool auxiliary) {
	if (!action.isNull()) {
		myActions.push_back(std::make_pair(action, auxiliary));
	}
}

void FBReaderNode::registerExpandTreeAction() {
	registerAction(new ExpandTreeAction(*this));
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

int FBReaderNode::height(ZLPaintContext &context) const {
	bool hasAuxHyperlink = false;
	for (std::vector<std::pair<shared_ptr<ZLRunnableWithKey>,bool> >::const_iterator it = myActions.begin(); it != myActions.end(); ++it) {
		if (it->second && it->first->makesSense()) {
			hasAuxHyperlink = true;
			break;
		}
	}
	return
		unitSize(context, FBTextStyle::Instance()) *
			(hasAuxHyperlink ? 13 : 11) / 2;
}

int FBReaderNode::unitSize(ZLPaintContext &context, const FBTextStyle &style) const {
	context.setFont(style.fontFamily(), style.fontSize(), style.bold(), style.italic());
	return (context.stringHeight() * 2 + 2) / 3;
}

std::string FBReaderNode::summary() const {
	std::string result;
	int count = 0;
	const ZLBlockTreeNode::List &subNodes = children();
	ZLBlockTreeNode::List::const_iterator it = subNodes.begin();
	for (; it != subNodes.end() && count < 3; ++it, ++count) {
		if (count > 0) {
			result += ", ";
		}
		result += ((const FBReaderNode*)*it)->title();
	}
	if (it != subNodes.end()) {
		result += ", ...";
	}
	return result;
}

void FBReaderNode::paint(ZLPaintContext &context, int vOffset) {
	if (!myIsInitialized) {
		init();
		myIsInitialized = true;
	}

	removeAllHyperlinks();

	drawCover(context, vOffset);
	drawTitle(context, vOffset);
	drawSummary(context, vOffset);

	int left = 0;
	int auxLeft = 0;
	for (std::vector<std::pair<shared_ptr<ZLRunnableWithKey>,bool> >::const_iterator it = myActions.begin(); it != myActions.end(); ++it) {
		if (it->first->makesSense()) {
			if (it->second) {
				drawHyperlink(context, auxLeft, vOffset, it->first, true);
			} else {
				drawHyperlink(context, left, vOffset, it->first);
			}
		}
	}
}

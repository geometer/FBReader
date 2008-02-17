/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLUnicodeUtil.h>

#include <ZLTextHyphenator.h>

#include "ZLTextView.h"
#include "ZLTextLineInfo.h"
#include "ZLTextParagraphCursor.h"
#include "ZLTextStyle.h"
#include "ZLTextWord.h"

struct ZLTextPartialInfo {
	//ZLTextWordCursor Start;
	//ZLTextWordCursor RealStart;
	ZLTextWordCursor End;
	bool IsVisible;
	//int LeftIndent;
	int Width;
	int Height;
	int Descent;
	//int VSpaceAfter;
	int SpaceCounter;
	//ZLTextStylePtr StartStyle;
	//shared_ptr<ZLTextTreeNodeInfo> NodeInfo;

	ZLTextPartialInfo(const ZLTextLineInfo &lineInfo, const ZLTextWordCursor &end);
	void setTo(ZLTextLineInfo &lineInfo) const;
};

ZLTextPartialInfo::ZLTextPartialInfo(const ZLTextLineInfo &lineInfo, const ZLTextWordCursor &end) : End(end), IsVisible(false), Width(lineInfo.Width), Height(lineInfo.Height), Descent(lineInfo.Descent), SpaceCounter(0) {
}

void ZLTextPartialInfo::setTo(ZLTextLineInfo &lineInfo) const {
	lineInfo.End = End;
	lineInfo.IsVisible = IsVisible;
	lineInfo.Width = Width;
	lineInfo.Height = std::max(lineInfo.Height, Height);
	lineInfo.Descent = std::max(lineInfo.Descent, Descent);
	lineInfo.SpaceCounter = SpaceCounter;
}

ZLTextLineInfoPtr ZLTextView::processTextLine(const ZLTextWordCursor &start, const ZLTextWordCursor &end) {
	const bool useHyphenator =
		ZLTextStyleCollection::instance().baseStyle().AutoHyphenationOption.value();

	ZLTextLineInfoPtr infoPtr = new ZLTextLineInfo(start, myStyle.textStyle());

	std::set<ZLTextLineInfoPtr>::const_iterator it = myLineInfoCache.find(infoPtr);
	if (it != myLineInfoCache.end()) {
		const ZLTextLineInfoPtr &storedInfo = *it;
		myStyle.applyControls(storedInfo->Start, storedInfo->End);
		return storedInfo;
	}

	ZLTextLineInfo &info = *infoPtr;
	ZLTextWordCursor current = start;
	const ZLTextParagraphCursor &paragraphCursor = current.paragraphCursor();
	const bool isFirstLine = current.isStartOfParagraph();

	if (paragraphCursor.paragraph().kind() == ZLTextParagraph::TREE_PARAGRAPH) {
		info.NodeInfo = new ZLTextTreeNodeInfo();
		ZLTextTreeNodeInfo &nodeInfo = *info.NodeInfo;
		const ZLTextTreeParagraph &treeParagraph = (const ZLTextTreeParagraph&)paragraphCursor.paragraph();
		nodeInfo.IsLeaf = treeParagraph.children().empty();
		nodeInfo.IsOpen = treeParagraph.isOpen();
		nodeInfo.IsFirstLine = isFirstLine;
		nodeInfo.ParagraphNumber = paragraphCursor.index();

		nodeInfo.VerticalLinesStack.reserve(treeParagraph.depth() - 1);
		if (treeParagraph.depth() > 1) {
			const ZLTextTreeParagraph *ctp = treeParagraph.parent();
			nodeInfo.VerticalLinesStack.push_back(ctp->children().back() != &treeParagraph);
			for (int i = 1; i < treeParagraph.depth() - 1; ++i) {
				const ZLTextTreeParagraph *parent = ctp->parent();
				nodeInfo.VerticalLinesStack.push_back(ctp != parent->children().back());
				ctp = parent;
			}
		}
	}

	if (isFirstLine) {
		ZLTextElement::Kind elementKind = paragraphCursor[current.wordNumber()].kind();
		while ((elementKind == ZLTextElement::CONTROL_ELEMENT) ||
					 (elementKind == ZLTextElement::FORCED_CONTROL_ELEMENT)) {
			const ZLTextElement &element = paragraphCursor[current.wordNumber()];
			switch (elementKind) {
				case ZLTextElement::CONTROL_ELEMENT:
					myStyle.applyControl((const ZLTextControlElement&)element);
					break;
				case ZLTextElement::FORCED_CONTROL_ELEMENT:
					myStyle.applyControl((const ZLTextForcedControlElement&)element);
					break;
				default:
					break;
			}
			current.nextWord();
			if (current.equalWordNumber(end)) {
				break;
			}
			elementKind = paragraphCursor[current.wordNumber()].kind();
		}
		info.StartStyle = myStyle.textStyle();
		info.RealStart = current;
	}

	ZLTextStylePtr storedStyle = myStyle.textStyle();

	info.LeftIndent = myStyle.textStyle()->leftIndent();
	if (isFirstLine) {
		info.LeftIndent += myStyle.textStyle()->firstLineIndentDelta();
	}
	if (!info.NodeInfo.isNull()) {
		info.LeftIndent += (myStyle.context().stringHeight() + 2) / 3 * 4 * (info.NodeInfo->VerticalLinesStack.size() + 1);
	}
	info.Width = info.LeftIndent;

	if (info.RealStart.equalWordNumber(end)) {
	  info.End = info.RealStart;
		return infoPtr;
	}

	ZLTextPartialInfo newInfo(info, current);
	bool allowBreakAtNBSpace = true;
	const int maxWidth = viewWidth() - myStyle.textStyle()->rightIndent();
	bool wordOccured = false;
	int lastSpaceWidth = 0;
	int removeLastSpace = false;

	ZLTextElement::Kind elementKind = paragraphCursor[newInfo.End.wordNumber()].kind();

	bool breakedAtFirstWord = false;
	do {
		const ZLTextElement &element = paragraphCursor[newInfo.End.wordNumber()];
		newInfo.Width += myStyle.elementWidth(element, newInfo.End.charNumber());
		newInfo.Height = std::max(newInfo.Height, myStyle.elementHeight(element));
		newInfo.Descent = std::max(newInfo.Descent, myStyle.elementDescent(element));
		switch (elementKind) {
			case ZLTextElement::CONTROL_ELEMENT:
				myStyle.applyControl((const ZLTextControlElement&)element);
				break;
			case ZLTextElement::FORCED_CONTROL_ELEMENT:
				myStyle.applyControl((const ZLTextForcedControlElement&)element);
				break;
			case ZLTextElement::WORD_ELEMENT:
			case ZLTextElement::IMAGE_ELEMENT:
				wordOccured = true;
				newInfo.IsVisible = true;
				break;
			case ZLTextElement::HSPACE_ELEMENT:
			case ZLTextElement::NB_HSPACE_ELEMENT:
				if (wordOccured) {
					wordOccured = false;
					++newInfo.SpaceCounter;
					lastSpaceWidth = myStyle.context().spaceWidth();
					newInfo.Width += lastSpaceWidth;
				}
				break;
			case ZLTextElement::EMPTY_LINE_ELEMENT:
				newInfo.IsVisible = true;
			default:
				break;
		}

		if (newInfo.Width > maxWidth) {
			if (!info.End.equalWordNumber(start)) {
				break;
			}
			if (useHyphenator && myStyle.textStyle()->allowHyphenations() &&
					(elementKind == ZLTextElement::WORD_ELEMENT)) {
				breakedAtFirstWord = true;
				break;
			}
		}

		ZLTextElement::Kind previousKind = elementKind;
		newInfo.End.nextWord();
		bool allowBreak = newInfo.End.equalWordNumber(end);
		bool nbspaceBreak = false;
		if (!allowBreak) {
			elementKind = paragraphCursor[newInfo.End.wordNumber()].kind();
			if (elementKind == ZLTextElement::NB_HSPACE_ELEMENT) {
				if (allowBreakAtNBSpace) {
					allowBreak = true;
					nbspaceBreak = true;
				}
			} else {
				allowBreak =
					((elementKind != ZLTextElement::WORD_ELEMENT) || (previousKind == ZLTextElement::WORD_ELEMENT)) &&
					(elementKind != ZLTextElement::IMAGE_ELEMENT) &&
					(elementKind != ZLTextElement::CONTROL_ELEMENT);
			}
		}
		if (allowBreak) {
			newInfo.setTo(info);
			allowBreakAtNBSpace = nbspaceBreak;
			storedStyle = myStyle.textStyle();
			removeLastSpace = !wordOccured && (info.SpaceCounter > 0);
		}
	} while (!newInfo.End.equalWordNumber(end));

	if (!newInfo.End.equalWordNumber(end) && useHyphenator &&
		 myStyle.textStyle()->allowHyphenations()) {
		const ZLTextElement &element = paragraphCursor[newInfo.End.wordNumber()];
		if (element.kind() == ZLTextElement::WORD_ELEMENT) {
			const int startCharNumber = newInfo.End.charNumber();
			newInfo.Width -= myStyle.elementWidth(element, startCharNumber);
			const ZLTextWord &word = (ZLTextWord&)element;
			int spaceLeft = maxWidth - newInfo.Width;
			if (breakedAtFirstWord ||
					((word.Length > 3) && (spaceLeft > 2 * myStyle.context().spaceWidth()))) {
				ZLUnicodeUtil::Ucs2String ucs2string;
				ZLUnicodeUtil::utf8ToUcs2(ucs2string, word.Data, word.Size);
				ZLTextHyphenationInfo hyphenationInfo = ZLTextHyphenator::instance().info(word);
				int hyphenationPosition = word.Length - 1;
				int subwordWidth = 0;
				for (; hyphenationPosition > startCharNumber; --hyphenationPosition) {
					if (hyphenationInfo.isHyphenationPossible(hyphenationPosition)) {
						subwordWidth = myStyle.wordWidth(word, startCharNumber, hyphenationPosition - startCharNumber, ucs2string[hyphenationPosition - 1] != '-');
						if (subwordWidth <= spaceLeft) {
							break;
						}
					}
				}
				if ((hyphenationPosition == startCharNumber) &&
						(info.End.wordNumber() <= info.RealStart.wordNumber())) {
					hyphenationPosition = word.Length;
					subwordWidth = myStyle.elementWidth(element, startCharNumber);
				}
				if (hyphenationPosition > startCharNumber) {
					newInfo.Width += subwordWidth;
					newInfo.setTo(info);
					storedStyle = myStyle.textStyle();
					removeLastSpace = false;
					info.End.setCharNumber(hyphenationPosition);
				}
			}
		}
	}

	if (removeLastSpace) {
		info.Width -= lastSpaceWidth;
		--info.SpaceCounter;
	}

	myStyle.setTextStyle(storedStyle);

	if (isFirstLine) {
		info.Height += info.StartStyle->spaceBefore();
	}
	if (info.End.isEndOfParagraph()) {
		info.VSpaceAfter = myStyle.textStyle()->spaceAfter();
	}

	if (!info.End.equalWordNumber(end) || end.isEndOfParagraph()) {
		myLineInfoCache.insert(infoPtr);
	}

	return infoPtr;
}

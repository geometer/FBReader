/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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
#include <ZLTextStyle.h>
#include <ZLTextStyleCollection.h>

#include "ZLTextArea.h"
#include "ZLTextLineInfo.h"
#include "ZLTextAreaStyle.h"
#include "ZLTextParagraphCursor.h"
#include "ZLTextWord.h"

struct ZLTextPartialInfo {
	//ZLTextWordCursor Start;
	//ZLTextWordCursor RealStart;
	ZLTextWordCursor End;
	bool IsVisible;
	//int StartIndent;
	int Width;
	int Height;
	int Descent;
	//int VSpaceAfter;
	int SpaceCounter;
	//shared_ptr<ZLTextStyle> StartStyle;
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

ZLTextLineInfoPtr ZLTextArea::processTextLine(Style &style, const ZLTextWordCursor &start, const ZLTextWordCursor &end) {
	const bool useHyphenator =
		ZLTextStyleCollection::Instance().AutoHyphenationOption.value();

	ZLTextLineInfoPtr infoPtr = new ZLTextLineInfo(start, style.textStyle(), style.bidiLevel());

	std::set<ZLTextLineInfoPtr>::const_iterator it = myLineInfoCache.find(infoPtr);
	if (it != myLineInfoCache.end()) {
		const ZLTextLineInfoPtr &storedInfo = *it;
		style.applyControls(storedInfo->Start, storedInfo->End);
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
		nodeInfo.ParagraphIndex = paragraphCursor.index();

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
		ZLTextElement::Kind elementKind = paragraphCursor[current.elementIndex()].kind();
		while ((elementKind == ZLTextElement::CONTROL_ELEMENT) ||
					 (elementKind == ZLTextElement::FORCED_CONTROL_ELEMENT)) {
			style.applySingleControl(paragraphCursor[current.elementIndex()]);
			current.nextWord();
			if (current.equalElementIndex(end)) {
				break;
			}
			elementKind = paragraphCursor[current.elementIndex()].kind();
		}
		info.StartStyle = style.textStyle();
		info.StartBidiLevel = style.bidiLevel();
		info.RealStart = current;
	}

	shared_ptr<ZLTextStyle> storedStyle = style.textStyle();
	unsigned char storedBidiLevel = style.bidiLevel();

	const int fontSize = style.textStyle()->fontSize();
	// TODO: change metrics at font change
	const ZLTextStyleEntry::Metrics metrics(fontSize, fontSize / 2, width(), height());
	info.StartIndent = style.textStyle()->lineStartIndent(metrics, isRtl());
	if (isFirstLine) {
		info.StartIndent += style.textStyle()->firstLineIndentDelta(metrics);
	}
	if (!info.NodeInfo.isNull()) {
		info.StartIndent += (context().stringHeight() + 2) / 3 * 4 * (info.NodeInfo->VerticalLinesStack.size() + 1);
	}
	info.Width = info.StartIndent;

	if (info.RealStart.equalElementIndex(end)) {
	  info.End = info.RealStart;
		return infoPtr;
	}

	ZLTextPartialInfo newInfo(info, current);
	bool allowBreakAtNBSpace = true;
	const int maxWidth = metrics.FullWidth - style.textStyle()->lineEndIndent(metrics, isRtl());
	bool wordOccured = false;
	int lastSpaceWidth = 0;
	int removeLastSpace = false;

	ZLTextElement::Kind elementKind = paragraphCursor[newInfo.End.elementIndex()].kind();

	bool breakedAtFirstWord = false;
	do {
		const ZLTextElement &element = paragraphCursor[newInfo.End.elementIndex()];
		newInfo.Width += style.elementWidth(element, newInfo.End.charIndex(), metrics);
		newInfo.Height = std::max(newInfo.Height, style.elementHeight(element, metrics));
		newInfo.Descent = std::max(newInfo.Descent, style.elementDescent(element));
		style.applySingleControl(element);
		switch (elementKind) {
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
					lastSpaceWidth = context().spaceWidth();
					newInfo.Width += lastSpaceWidth;
				}
				break;
			case ZLTextElement::EMPTY_LINE_ELEMENT:
				newInfo.IsVisible = true;
				break;
			default:
				break;
		}

		if (newInfo.Width > maxWidth) {
			if (!info.End.equalElementIndex(start)) {
				break;
			}
			if (useHyphenator && style.textStyle()->allowHyphenations() &&
					(elementKind == ZLTextElement::WORD_ELEMENT)) {
				breakedAtFirstWord = true;
				break;
			}
		}

		ZLTextElement::Kind previousKind = elementKind;
		newInfo.End.nextWord();
		bool allowBreak = newInfo.End.equalElementIndex(end);
		bool nbspaceBreak = false;
		if (!allowBreak) {
			elementKind = paragraphCursor[newInfo.End.elementIndex()].kind();
			if (elementKind == ZLTextElement::NB_HSPACE_ELEMENT) {
				if (allowBreakAtNBSpace) {
					allowBreak = true;
					nbspaceBreak = true;
				}
			} else if (elementKind == ZLTextElement::WORD_ELEMENT) {
				allowBreak = previousKind == ZLTextElement::WORD_ELEMENT;
			} else if ((elementKind == ZLTextElement::START_REVERSED_SEQUENCE_ELEMENT) &&
								 (previousKind == ZLTextElement::WORD_ELEMENT)) {
				allowBreak = false;
			} else if ((elementKind == ZLTextElement::END_REVERSED_SEQUENCE_ELEMENT) &&
								 (previousKind == ZLTextElement::WORD_ELEMENT)) {
				allowBreak = false;
			} else {
				allowBreak =
					(elementKind != ZLTextElement::IMAGE_ELEMENT) &&
					(elementKind != ZLTextElement::CONTROL_ELEMENT);
			}
		}
		if (allowBreak) {
			newInfo.setTo(info);
			allowBreakAtNBSpace = nbspaceBreak;
			storedStyle = style.textStyle();
			storedBidiLevel = style.bidiLevel();
			removeLastSpace = !wordOccured && (info.SpaceCounter > 0);
		}
	} while (!newInfo.End.equalElementIndex(end));

	if (!newInfo.End.equalElementIndex(end) && useHyphenator &&
		 style.textStyle()->allowHyphenations()) {
		const ZLTextElement &element = paragraphCursor[newInfo.End.elementIndex()];
		if (element.kind() == ZLTextElement::WORD_ELEMENT) {
			const int startCharIndex = newInfo.End.charIndex();
			newInfo.Width -= style.elementWidth(element, startCharIndex, metrics);
			const ZLTextWord &word = (ZLTextWord&)element;
			int spaceLeft = maxWidth - newInfo.Width;
			if (breakedAtFirstWord ||
					((word.Length > 3) && (spaceLeft > 2 * context().spaceWidth()))) {
				ZLUnicodeUtil::Ucs4String ucs4string;
				ZLUnicodeUtil::utf8ToUcs4(ucs4string, word.Data, word.Size);
				ZLTextHyphenationInfo hyphenationInfo = ZLTextHyphenator::Instance().info(word);
				int hyphenationPosition = word.Length - 1;
				int subwordWidth = 0;
				for (; hyphenationPosition > startCharIndex; --hyphenationPosition) {
					if (hyphenationInfo.isHyphenationPossible(hyphenationPosition)) {
						subwordWidth = style.wordWidth(word, startCharIndex, hyphenationPosition - startCharIndex, ucs4string[hyphenationPosition - 1] != '-');
						if (subwordWidth <= spaceLeft) {
							break;
						}
					}
				}
				if ((hyphenationPosition == startCharIndex) &&
						(info.End.elementIndex() <= info.RealStart.elementIndex())) {
					hyphenationPosition = word.Length;
					subwordWidth = style.elementWidth(element, startCharIndex, metrics);
				}
				if (hyphenationPosition > startCharIndex) {
					newInfo.Width += subwordWidth;
					newInfo.setTo(info);
					storedStyle = style.textStyle();
					storedBidiLevel = style.bidiLevel();
					removeLastSpace = false;
					info.End.setCharIndex(hyphenationPosition);
				}
			}
		}
	}

	if (removeLastSpace) {
		info.Width -= lastSpaceWidth;
		--info.SpaceCounter;
	}

	style.setTextStyle(storedStyle, storedBidiLevel);

	if (isFirstLine) {
		info.Height += info.StartStyle->spaceBefore(metrics);
	}
	if (info.End.isEndOfParagraph()) {
		info.VSpaceAfter = style.textStyle()->spaceAfter(metrics);
	}

	if (!info.End.equalElementIndex(end) || end.isEndOfParagraph()) {
		myLineInfoCache.insert(infoPtr);
	}

	return infoPtr;
}

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

#ifndef __FBREADERNODE_H__
#define __FBREADERNODE_H__

#include <map>
#include <vector>

#include <ZLBlockTreeView.h>

class ZLImage;
class ZLResource;
class FBTextStyle;

class FBReaderNode : public ZLBlockTreeNode {

protected:
	static shared_ptr<ZLImage> defaultCoverImage(const std::string &id);

private:
	static std::map<std::string,shared_ptr<ZLImage> > ourDefaultCovers;

private:
	class ExpandTreeAction;

public:
	static const ZLTypeId TYPE_ID;

protected:
	FBReaderNode(ZLBlockTreeNode *parent, size_t atPosition = (size_t)-1);
	virtual void init();
	virtual const ZLResource &resource() const = 0;
	virtual bool highlighted() const;

public:
	~FBReaderNode();

	void drawCoverReal(ZLPaintContext &context, int vOffset);

protected:
	virtual void drawCover(ZLPaintContext &context, int vOffset);
	void drawTitle(ZLPaintContext &context, int vOffset);
	void drawSummary(ZLPaintContext &context, int vOffset);
	void drawHyperlink(ZLPaintContext &context, int &hOffset, int &vOffset, shared_ptr<ZLRunnableWithKey> action, bool auxiliary = false);

private:
	int unitSize(ZLPaintContext &context, const FBTextStyle &style) const;

protected:
	void paint(ZLPaintContext &context, int vOffset);
	void registerAction(shared_ptr<ZLRunnableWithKey> action, bool auxiliary = false);
	void registerExpandTreeAction();
	virtual shared_ptr<ZLImage> extractCoverImage() const = 0;

private:
	const ZLTypeId &typeId() const;

public:
	shared_ptr<ZLImage> coverImage() const;
	virtual std::string title() const = 0;
	virtual std::string summary() const;

	void expandOrCollapseSubtree();

protected:
	int height(ZLPaintContext &context) const;

private:
	mutable bool myCoverImageIsStored;
	mutable shared_ptr<ZLImage> myStoredCoverImage;
	std::vector<std::pair<shared_ptr<ZLRunnableWithKey>,bool> > myActions;
	bool myIsInitialized;
};

#endif /* __FBREADERNODE_H__ */

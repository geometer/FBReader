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

#ifndef __TOCTREENODES_H__
#define __TOCTREENODES_H__

#include <ZLTextParagraph.h>

#include <ZLTreeTitledNode.h>
#include <ZLTreeActionNode.h>

class ReferenceTreeNode : public ZLTreeTitledNode {

public:
	static const ZLTypeId TYPE_ID;
	const ZLTypeId &typeId() const;

public:
	ReferenceTreeNode(const ZLTextTreeParagraph& paragraph);

public:
	std::string title() const;

private:
	void createChildren();

private:
	const ZLTextTreeParagraph& myParagraph;
};

class ReferenceNode : public ZLTreeActionNode {

public:
	static const ZLTypeId TYPE_ID;
	const ZLTypeId &typeId() const;

public:
	ReferenceNode(const ZLTextTreeParagraph& paragraph);

public:
	std::string title() const;
	bool activate();

private:
	const ZLTextTreeParagraph& myParagraph;
};

#endif /* __TOCTREENODES_H__ */

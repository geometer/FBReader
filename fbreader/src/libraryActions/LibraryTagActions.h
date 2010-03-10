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

#ifndef __LIBRARYTAGACTIONS_H__
#define __LIBRARYTAGACTIONS_H__

#include <shared_ptr.h>

#include <ZLRunnable.h>

class Tag;

class TagEditOrCloneAction : public ZLRunnableWithKey {

public:
	TagEditOrCloneAction(shared_ptr<Tag> tag);

private:
	void run();

protected:
	virtual void onAccept(const std::string &name, bool includeSubTags) = 0;
	virtual std::string resourceKeyName() const = 0;

protected:
	const shared_ptr<Tag> myTag;
};

class TagEditAction : public TagEditOrCloneAction {

public:
	TagEditAction(shared_ptr<Tag> tag);

private:
	ZLResourceKey key() const;
	void onAccept(const std::string &name, bool includeSubTags);
	std::string resourceKeyName() const;
};

class TagCloneAction : public TagEditOrCloneAction {

public:
	TagCloneAction(shared_ptr<Tag> tag);

private:
	ZLResourceKey key() const;
	void onAccept(const std::string &name, bool includeSubTags);
	std::string resourceKeyName() const;
};

class TagRemoveAction : public ZLRunnableWithKey {

public:
	TagRemoveAction(shared_ptr<Tag> tag);

private:
	void run();
	ZLResourceKey key() const;

private:
	const shared_ptr<Tag> myTag;
};

#endif /* __LIBRARYTAGACTIONS_H__ */

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

#include "ZLGtkOptionViewHolder.h"
#include "ZLGtkOptionView.h"

ZLOptionView *ZLGtkOptionViewHolder::createViewByEntry(const std::string &name, const std::string &tooltip, shared_ptr<ZLOptionEntry> option) {
	if (option.isNull()) {
		return 0;
	}

	switch (option->kind()) {
		case ZLOptionEntry::BOOLEAN:
			return new BooleanOptionView(name, tooltip, option, *this);
		case ZLOptionEntry::BOOLEAN3:
			return new Boolean3OptionView(name, tooltip, option, *this);
		case ZLOptionEntry::STRING:
			return new StringOptionView(name, tooltip, option, *this);
		case ZLOptionEntry::CHOICE:
			return new ChoiceOptionView(name, tooltip, option, *this);
		case ZLOptionEntry::SPIN:
			return new SpinOptionView(name, tooltip, option, *this);
		case ZLOptionEntry::COMBO:
			return new ComboOptionView(name, tooltip, option, *this);
		case ZLOptionEntry::COLOR:
			return new ColorOptionView(name, tooltip, option, *this);
		case ZLOptionEntry::KEY:
			return new KeyOptionView(name, tooltip, option, *this);
	}

	return 0;
}

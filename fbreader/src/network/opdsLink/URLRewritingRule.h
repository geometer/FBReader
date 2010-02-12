/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __URLREWRITINGRULE_H__
#define __URLREWRITINGRULE_H__

#include <string>

struct URLRewritingRule {

	enum RuleType {
		ADD_URL_PARAMETER
	};

	enum RuleApply {
		ALWAYS, EXTERNAL, INTERNAL
	};

	RuleType Type;
	RuleApply Apply;
	std::string Name;
	std::string Value;

	URLRewritingRule(RuleType type, RuleApply apply, const std::string &name, const std::string &value);
};

inline URLRewritingRule::URLRewritingRule(RuleType type, RuleApply apply, const std::string &name, const std::string &value) : Type(type), Apply(apply), Name(name), Value(value) {}

#endif /* __URLREWRITINGRULE_H__ */

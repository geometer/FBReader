/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "FBReader.h"

std::string FBReader::actionName(ActionCode code) {
	switch (code) {
		case ACTION_SHOW_COLLECTION:
			return "Show Library";
		case ACTION_SHOW_OPTIONS:
			return "Show Options Dialog";
		case ACTION_UNDO:
			return "Undo";
		case ACTION_REDO:
			return "Redo";
		case ACTION_SHOW_CONTENTS:
			return "Show Table Of Contents";
		case ACTION_SEARCH:
			return "Search";
		case ACTION_FIND_PREVIOUS:
			return "Find Previous";
		case ACTION_FIND_NEXT:
			return "Find Next";
		case ACTION_LARGE_SCROLL_FORWARD:
			return "Large Scroll Forward";
		case ACTION_LARGE_SCROLL_BACKWARD:
			return "Large Scroll Backward";
		case ACTION_SMALL_SCROLL_FORWARD:
			return "Small Scroll Forward";
		case ACTION_SMALL_SCROLL_BACKWARD:
			return "Small Scroll Backward";
		case ACTION_SCROLL_TO_HOME:
			return "Go To Home";
		case ACTION_SCROLL_TO_START_OF_TEXT:
			return "Go To Start Of Section";
		case ACTION_SCROLL_TO_END_OF_TEXT:
			return "Go To End Of Section";
		case ACTION_INCREASE_FONT:
			return "Increase Font Size";
		case ACTION_DECREASE_FONT:
			return "Decrease Font Size";
		case ACTION_SHOW_HIDE_POSITION_INDICATOR:
			return "Toggle Position Indicator";
		case ACTION_TOGGLE_FULLSCREEN:
			return "Toggle Fullscreen Mode";
		case ACTION_FULLSCREEN_ON:
			return "Switch To Fullscreen Mode";
		case ACTION_ADD_BOOK:
			return "Add Book";
		case ACTION_SHOW_BOOK_INFO:
			return "Show Book Info Dialog";
		/*
		case ACTION_SHOW_HELP:
			return "";
		*/
		case ACTION_ROTATE_SCREEN:
			return "Rotate Screen";
		case ACTION_SHOW_LAST_BOOKS:
			return "Show Recent Books";
		case ACTION_QUIT:
			return "Quit";
		default:
			return "";
	}
}

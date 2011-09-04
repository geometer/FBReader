/*
 * Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

import QtQuick 1.0
import com.nokia.meego 1.0

Item {
	id: root
	property variant handler
	width: parent.width
	height: switchItem.height
	visible: handler.visible
	enabled: handler.enabled
	
	Label {
		anchors { left: root.left; right: switchItem.left; verticalCenter: switchItem.verticalCenter }
		anchors.rightMargin: 15
		text: handler.name
	}

	Switch {
		id: switchItem
		anchors { top: root.top; right: root.right }
		visible: handler.visible
		onCheckedChanged: if (handler.checked != switchItem.checked) handler.checked = switchItem.checked
		Component.onCompleted: switchItem.checked = root.handler.checked
	}
	
	Connections {
		target: root.handler
		onCheckedChanged: switchItem.checked = root.handler.checked
	}
}

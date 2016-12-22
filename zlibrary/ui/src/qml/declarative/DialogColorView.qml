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
	height: rect.height
	visible: handler.visible
	enabled: handler.enabled
	
	Label {
		anchors { left: root.left; right: rect.left; verticalCenter: rect.verticalCenter }
		anchors.rightMargin: 15
		text: handler.name
		visible: text != ""
	}

	Rectangle {
		id: rect
		anchors { top: root.top; right: root.right }
		height: column.height
		width: redSlider.width
		color: Qt.rgba(redSlider.value, greenSlider.value, blueSlider.value)
		property bool __isUpdate: false
		Column {
			id: column
			Slider {
				id: redSlider
				onValueChanged: if (!rect.__isUpdate) root.handler.color = rect.color
			}
			Slider {
				id: greenSlider
				width: redSlider.width
				onValueChanged: if (!rect.__isUpdate) root.handler.color = rect.color
			}
			Slider {
				id: blueSlider
				width: redSlider.width
				onValueChanged: if (!rect.__isUpdate) root.handler.color = rect.color
			}
		}
		
		function __update() {
			var color = root.handler.parsedColor();
			rect.__isUpdate = true;
			redSlider.value = color.red;
			greenSlider.value = color.green;
			blueSlider.value = color.blue;
			rect.__isUpdate = false;
		}
		
		Component.onCompleted: rect.__update()
	}
	Connections {
		target: root.handler
		onColorChanged: rect.__update()
	}
}

/*
 * Copyright (C) 2004-2011 Geometer Plus <contact@geometerplus.com>
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
import Qt.labs.components.native 1.0
import com.nokia.extras 1.0

Item {
	id: root
	property variant handler
	width: parent.width
	height: textField.height
	visible: handler.visible
	enabled: handler.enabled
	
	Label {
		anchors { left: root.left; right: textField.left; verticalCenter: textField.verticalCenter }
		anchors.rightMargin: 15
		text: handler.name
	}

	TextField {
		id: textField
		anchors { top: root.top; right: root.right }
		text: handler.value
		placeholderText: "Value between " + handler.minimumValue + " and " + handler.maximumValue
		validator: IntValidator { bottom: handler.minimumValue; top: handler.maximumValue }
		errorHighlight: !acceptableInput
		inputMethodHints: Qt.ImhDigitsOnly | Qt.ImhNoPredictiveText
		onTextChanged: if (root.handler.value != text) root.handler.value = text
	}
	
	Connections {
		target: root.handler
		onValueChanged: textField.text = root.handler.value
	}
}

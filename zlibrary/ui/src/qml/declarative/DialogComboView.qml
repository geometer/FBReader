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
	height: combobox.height
	visible: handler.visible
	enabled: handler.enabled
	
	Label {
		anchors { left: root.left; right: combobox.left; verticalCenter: combobox.verticalCenter }
		anchors.rightMargin: 15
		text: handler.name
	}
	
	Button {
		id: combobox
		anchors { top: root.top; right: root.right }
		text: handler.currentText
		onClicked: {
			var values = handler.values;
			dialog.selectedIndex = -1;
			for (var i = 0; i < values.length; ++i) {
				dialog.model.append({ "name": values[i] });
				if (values[i] == text)
					dialog.selectedIndex = i;
			}
			dialog.open();
		}
	}
	Image {
		id: image
		anchors { right: combobox.right; verticalCenter: combobox.verticalCenter }
		property variant __invertedString: theme.inverted ? "-inverted" : ""
		property variant __enabled: combobox.enabled ? "" : "-disabled"
		property variant __pressed: combobox.pressed ? "-pressed" : ""
		source: "image://theme/meegotouch-combobox-indicator" + __invertedString + __enabled + __pressed
	}

//	TextField {
//		id: combobox
//		anchors.fill: combobox
//		visible: false
//		text: handler.currentText
//	}
	
	SelectionDialog {
		id: dialog
		titleText: handler.name
		onAccepted: {
			handler.currentIndex = dialog.selectedIndex;
			dialog.model.clear();
		}
		onRejected: dialog.model.clear()
	}
	
	Connections {
		target: root.handler
		onCurrentTextChanged: combobox.text = root.handler.currentText
	}
}

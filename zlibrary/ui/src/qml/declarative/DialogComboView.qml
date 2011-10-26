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
import com.nokia.meego 1.0

Item {
	id: root
	property variant handler
	property variant widget: handler.editable ? textField : combobox
	width: parent.width
	height: widget.height
	visible: handler.visible
	enabled: handler.enabled
	
	Label {
		id: label
		anchors { left: root.left; rightMargin: 15; verticalCenter: root.widget.verticalCenter }
		text: handler.name
	}
	
	Button {
		id: combobox
		anchors {
			left: label.text == "" ? root.left : label.right
			leftMargin: label.text == "" ? 0 : 15
			top: root.top
			right: root.right
		}
		visible: !root.handler.editable
		text: handler.currentText
		onClicked: dialog.chooseValue()
	}
	
	TextField {
		id: textField
		anchors.fill: combobox
//		anchors {
//			left: label.text == "" ? root.left : label.right
//			leftMargin: label.text == "" ? 0 : 15
//			top: root.top
//			right: root.right
//		}
		visible: root.handler.editable
		onTextChanged: if (root.handler.currentText != text) root.handler.currentText = text
		Component.onCompleted: text = handler.currentText
	}
	Image {
		id: image
		anchors { right: root.widget.right; rightMargin: 5; verticalCenter: root.widget.verticalCenter }
		property variant __invertedString: theme.inverted ? "-inverted" : ""
		property variant __enabled: root.widget.enabled ? "" : "-disabled"
		property variant __pressed: combobox.pressed ? "-pressed" : ""
		source: "image://theme/meegotouch-combobox-indicator" + __invertedString + __enabled + __pressed
		MouseArea {
			enabled: root.handler.editable
			onClicked: dialog.chooseValue()
		}
	}
	
	SelectionDialog {
		id: dialog
		titleText: handler.name
		model: ListModel{}
		onAccepted: {
			handler.currentIndex = dialog.selectedIndex;
			dialog.model.clear();
		}
		onRejected: dialog.model.clear()
		
		function chooseValue() {
			var values = root.handler.values;
			var text = root.handler.currentText;
			dialog.selectedIndex = -1;
			for (var i = 0; i < values.length; ++i) {
				dialog.model.append({ "name": values[i] });
				if (values[i] == text)
					dialog.selectedIndex = i;
			}
			dialog.open();
		}
	}
	
	Connections {
		target: root.handler
		onCurrentTextChanged: {
			if (root.widget.text != root.handler.currentText)
				root.widget.text = root.handler.currentText;
		}
	}
}

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
import org.fbreader 0.14
import "DialogUtils.js" as Utils

Flickable {
	id: root
	
	property variant handler

	anchors.fill: parent
	anchors.margins: 15
    contentWidth: column.width
    contentHeight: column.height
    flickableDirection: Flickable.VerticalFlick

    Column {
        id: column
        anchors.top: parent.top
        spacing: 10
		width: root.width
//		height: childrenRect.height
		
		Repeater {
			model: handler.items
			Item {
				id: contentArea
				width: column.width
				height: visible ? childrenRect.height : 0
				visible: modelData.visible
				enabled: modelData.enabled
				property variant child
				Component.onCompleted: child = root.createChild(contentArea, modelData)
			}
		}
    }
	
	function createChild(item, object) {
		var componentName;
		switch (object.type) {
		case OptionView.Choice:
			componentName = "DialogChoiceView.qml"
			break;
		case OptionView.Boolean:
			componentName = "DialogBooleanView.qml"
			break;
		case OptionView.Boolean3:
			// TODO
			componentName = "DialogBoolean3View.qml"
			break;
		case OptionView.String:
			componentName = "DialogStringView.qml"
			break;
		case OptionView.Password:
			componentName = "DialogStringView.qml"
			break;
		case OptionView.Spin:
			componentName = "DialogSpinView.qml"
			break;
		case OptionView.Combo:
			// TODO: Implement editing of text
			componentName = "DialogComboView.qml"
			break;
		case OptionView.Color:
			componentName = "DialogColorView.qml"
			break;
		case OptionView.Key:
			// TODO
			componentName = "DialogKeyView.qml"
			break;
		case OptionView.Order:
			// TODO
			componentName = "DialogOrderView.qml"
			break;
		case OptionView.Multiline:
			componentName = "DialogMultilineView.qml"
			break;
		case OptionView.Static:
			componentName = "DialogStaticView.qml"
			break;
		default:
			break;
		}
		if (componentName !== undefined) {
			var component = Qt.createComponent(componentName);
			if (component !== null)
				component.createObject(item, { handler: object });
		}
	}
}

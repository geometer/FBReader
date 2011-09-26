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
import org.fbreader 0.14

ListView {
	id: root
	
	property variant handler
	property bool invertedTheme: false

	anchors.fill: parent
	anchors { leftMargin: 15; topMargin: 15; rightMargin: 15 }
	model: VisualDataModel {
		id: itemModel
		model: handler ? handler.items : null
		delegate: Item {
			id: contentArea
			width: root.width
			height: modelData.visible ? childrenRect.height + 15 : 0
			visible: modelData.visible
			enabled: modelData.enabled
			property variant child: __ensureChild(modelData, modelData.visible)
			function __ensureChild(modelData, visible) {
				if (!child && visible)
					child = root.createChild(contentArea, modelData)
				return child;
			}
		}
	}
	
	property variant __componentCache: {}
	
	onInvertedThemeChanged: fixChildren(root)

	function fixChildren(child) {
		if (child.platformStyle !== undefined
				&& child.platformStyle.inverted !== undefined) {
			child.platformStyle.inverted = root.invertedTheme;
		}
		var children = child.children;
		for (var i = 0; i < children.length; ++i)
			fixChildren(children[i]);
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
//			componentName = "DialogBoolean3View.qml"
			break;
		case OptionView.Path:
			componentName = "DialogPathView.qml"
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
//			componentName = "DialogKeyView.qml"
			break;
		case OptionView.Order:
			// TODO
//			componentName = "DialogOrderView.qml"
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
		var child;
		if (componentName !== undefined) {
			var component = __componentCache[componentName];
			if (!component) {
				component = Qt.createComponent(componentName);
				__componentCache[componentName] = component;
			}
			if (component.status == Component.Error) {
				// Error Handling
				console.log("Error loading component:", component.errorString());
				return null;
			}
			if (component !== null) {
				child = component.createObject(item, { handler: object });
				if (root.invertedTheme)
					fixChildren(child)
			}
		}
		return child;
	}
}

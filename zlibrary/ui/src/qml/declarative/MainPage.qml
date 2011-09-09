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
import Qt.labs.gestures 1.0
import org.fbreader 0.14

Page {
	id: root
    property bool showToolBar: true
	tools: ToolBarLayout {
		id: toolBarLayout
		Repeater {
			model: applicationInfo.actions
			ToolIcon {
				id: item
				property variant menu;
				iconSource: modelData.platformIconId === "" ? modelData.iconSource : ""
				platformIconId: modelData.platformIconId
				onClicked: {
					switch (modelData.type) {
					case ToolBarItem.PlainButton:
					case ToolBarItem.ToggleButton:
						modelData.activate();
						break;
					case ToolBarItem.MenuButton:
						if (menu === undefined) {
							console.log("menu", JSON.stringify(modelData.items));
							var component = Qt.createComponent("DynamicMenu.qml");
							var args = { item: modelData, visualParent: root.pageStack };
							menu = component.createObject(root, args);
							menu.open()
						} else if (menu.status == DialogStatus.Closed) {
							menu.open()
						} else {
							menu.close();
						}
						break;
					default:
						break;
					}
					
				}
				visible: modelData.visible && modelData.enabled
			}
		}
		ToolIcon {
			platformIconId: "toolbar-view-menu"
			onClicked: {
				if (mainMenu.status == DialogStatus.Closed) {
					mainMenu.item.recheckItems();
					mainMenu.open()
				} else {
					mainMenu.close();
				}
			}
		}
	}
	DynamicMenu {
		id: mainMenu
		visualParent: root.pageStack
		item: applicationInfo.menuBar
	}
	BookView {
		id: bookView
		anchors.fill: parent
		holder: objectHolder
		Component.onCompleted: objectHolder.bookView = bookView
		MouseArea {
			anchors.fill: parent
			onPressed: objectHolder.handlePress(mouse.x, mouse.y)
			onReleased: objectHolder.handleRelease(mouse.x, mouse.y)
			onPositionChanged: {
				if (pressed)
					objectHolder.handleMovePressed(mouse.x, mouse.y)
				else
					objectHolder.handleMove(mouse.x, mouse.y)
			}
		}
	}
	states: [
		State {
			name: ""
			when: toolBarArea.pressed || mainMenu.status != DialogStatus.Closed
			PropertyChanges {
				target: root
				showToolBar: true
			}
		},
		State {
			name: "hidden"
			PropertyChanges {
				target: root
				showToolBar: false
			}
		}
	]
	Timer {
		id: toolBarTimer
		running: root.state == "" && mainMenu.status == DialogStatus.Closed
		interval: 3000
		property bool triggered: false
		onTriggered: root.state = "hidden"
	}
	MouseArea {
		id: toolBarArea
		enabled: root.state == "hidden"
		anchors.bottom: parent.bottom
		width: parent.width
		// FIXME: Find the way to find actual ToolBar's height without hardcoding
		height: 72
		z: bookView.z + 1
	}
}

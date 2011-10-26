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
import com.nokia.extras 1.0
import org.fbreader 0.14

BaseTreeDialogPage {
	id: root
	
	VisualDataModel {
		id: visualModel
		model: root.handler
		rootIndex: root.rootIndex
		delegate: ListDelegate {
			signal tapAndHold
			property bool hasTapAndHold: false
			onClicked: {
				if (hasTapAndHold)
					return;
				console.log(model.title, model.activatable, model.page)
				if (model.activatable) {
					if (root.handler.activate(visualModel.modelIndex(index))) {
//						root.__close();
						root.handler.finish();
					}
				} else {
					var rootIndex = visualModel.modelIndex(index);
					var component = model.page ? itemPageComponent : root.component;
					var args = {
						"handler": root.handler,
					    "rootWindow": root.rootWindow,
					    "component": root.component,
					    "rootIndex": rootIndex
					};
					if (model.page)
						args["imageSource"] = model.iconSource;
					var page = root.pageStack.push(component, args);
				}
			}
			onPressedChanged: if (pressed) hasTapAndHold = false
			onTapAndHold: {
				console.log("Tap on", model.title, model.iconSource)
				var menu = menuComponent.createObject(listView.parent, { "modelIndex": visualModel.modelIndex(index) });
				if (menu.hasChildren) {
					hasTapAndHold = true;
					menu.show();
				} else {
					menu.destroy();
				}
			}
			
			MoreIndicator {
				id: indicator
				anchors { verticalCenter: parent.verticalCenter; right: parent.right }
				visible: !model.activatable && !itemBusyIndicator.visible
			}
			BusyIndicator {
				id: itemBusyIndicator
				anchors { verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: 5 }
				visible: root.revision >= 0 && root.handler.progressData(visualModel.modelIndex(index)) !== undefined
				running: visible
			}
			Timer {
				interval: 800
				running: parent.pressed
				onTriggered: parent.tapAndHold()
			}
		}
	}
	
	ListView {
		id: listView
		anchors.fill: parent
		model: visualModel
	}
	
	Component {
		id: itemPageComponent
		TreeDialogItemPage {
		}
	}
	
	Component {
		id: menuComponent
		TreeDialogMenu {
			id: menu
			visualParent: root.pageStack
			function show() {
				open();
				menu.statusChanged.connect(
							function() {
								if (menu.status == DialogStatus.Closed)
									menu.destroy();
							});
			}
		}
	}

	Component.onCompleted: root.handler.fetchChildren(rootIndex)
}

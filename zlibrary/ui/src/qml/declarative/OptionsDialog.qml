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

Sheet {
	id: root
	property variant handler
	property variant sections: handler ? handler.sections : null
	acceptButtonText: handler ? handler.okButtonText : ""
	rejectButtonText: handler ? handler.cancelButtonText : ""
	
	onSectionsChanged: pagesModel.update()
	
	content: Item {
		id: contentArea
		objectName: "contentArea"
		anchors.fill: parent
		anchors { top: parent.top; left: parent.left; right: parent.right; bottom: parent.bottom; }
		anchors.bottomMargin: toolBar.visible || (toolBar.opacity==1)? toolBar.height : 0
		PageStack {
			id: pageStack
			anchors { top: parent.top; left: parent.left; right: parent.right; bottom: parent.bottom; }
			anchors.bottomMargin: toolBar.visible || (toolBar.opacity==1)? toolBar.height : 0
			toolBar: toolBar
		}
		ToolBar {
			objectName: "toolBar"
			anchors.top: contentArea.bottom
			id: toolBar
			// Don't know why I have to do it manually
			onVisibleChanged: if (__currentContainer) __currentContainer.visible = visible;
		}
	}
	Page {
		id: mainPage
		tools: ToolBarLayout {
			id: toolsLayout
		}
		ListModel {
			id: pagesModel
			
			function update() {
				pagesModel.clear();
				if (!root.sections)
					return;
				var sections = root.sections;
				for (var i = 0; i < sections.length; ++i) {
					var section = sections[i];
					pagesModel.append({ "title": section.title, "subtitle": "", "section": section });
				}
			}
		}
		
		ListView {
			id: pagesView
			anchors.fill: parent
			model: pagesModel
			delegate: ListDelegate {
				MoreIndicator {
					anchors.right: parent.right;
					anchors.verticalCenter: parent.verticalCenter
				}
				onClicked: {
					pageStack.push(contentComponent, { handler: model.section });
					toolBar.__currentContainer.visible = true;
				}
			}
		}
	}
	Component {
		id: contentComponent
		Page {
			id: contentPage
			property alias handler: content.handler
			DialogContent {
				id: content
			}
			tools: ToolBarLayout {
				ToolIcon {
					visible: true
					platformIconId: "toolbar-previous"
					onClicked: pageStack.pop()
				}
			}
		}
	}
	onAccepted: handler.accept()
	onRejected: handler.reject()
	onStatusChanged: {
		if (status == PageStatus.Inactive && pageStack.currentPage != mainPage) {
			pageStack.clear();
			pageStack.push(mainPage);
		}
	}
	Component.onCompleted: {
		__owner = parent;
		pageStack.push(mainPage);
	}
}

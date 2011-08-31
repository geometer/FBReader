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
import com.nokia.extras 1.0

Sheet {
	id: root
	property variant handler
	acceptButtonText: handler ? handler.okButtonText : ""
	rejectButtonText: handler ? handler.cancelButtonText : ""
	
	onHandlerChanged: pagesModel.update()
	
	content: MouseArea {
		id: contentArea
		anchors.fill: parent
		anchors { top: parent.top; left: parent.left; right: parent.right; bottom: parent.bottom; }
		anchors.bottomMargin: toolBar.visible || (toolBar.opacity==1)? toolBar.height : 0
		PageStack {
			id: pageStack
			anchors { top: parent.top; left: parent.left; right: parent.right; bottom: parent.bottom; }
			anchors.bottomMargin: toolBar.visible || (toolBar.opacity==1)? toolBar.height : 0
			toolBar: toolBar
			Page {
				id: mainPage
//				tools: ToolBarLayout {
//				}
				
				ListModel {
					id: pagesModel
					
					function update() {
						pagesModel.clear()
						if (!root.handler)
							return;
						var sections = root.handler.sections;
						for (var i = 0; i < sections.length; ++i) {
							var section = sections[i];
							console.log("append", section.title, section);
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
						onClicked: pageStack.push(contentComponent, { handler: model.section })
					}
				}
			}
			Component.onCompleted: pageStack.push(mainPage)
		}
		ToolBar {
			anchors.top: contentArea.bottom
			id: toolBar
		}
	}
	Component {
		id: contentComponent
		Page {
			property alias handler: content.handler
			DialogContent {
				id: content
			}
			tools: ToolBarLayout {
				ToolIcon {
					platformIconId: "toolbar-previous"
					onClicked: pageStack.pop()
				}
			}
		}
	}
	MouseArea { anchors.fill: pageStack }
	onAccepted: { handler.accept(); while (pageStack.depth > 1) pageStack.pop().visible = false; }
	onRejected: { handler.reject(); while (pageStack.depth > 1) pageStack.pop().visible = false; }
}

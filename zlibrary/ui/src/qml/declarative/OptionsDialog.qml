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

Page {
	id: root
	property variant handler
	property variant sections: handler ? handler.sections : null
	
	onSectionsChanged: pagesModel.update()
	tools: ToolBarLayout {
		id: toolsLayout
		ToolIcon {
			platformIconId: "toolbar-previous"
			onClicked: pageStack.pop()
		}
	}
	
	ListModel {
		id: pagesModel
		
		function update() {
			console.log(root, root.sections, root.sections.length)
			pagesModel.clear();
			if (!root.sections)
				return;
			console.log(root, root.sections[0])
			var sections = root.sections;
			for (var i = 0; i < sections.length; ++i) {
				var section = sections[i];
				pagesModel.append({ "title": section.title, "subtitle": "", "section": section });
			}
		}
	}
	ListView {
		id: pagesView
		anchors { leftMargin: 14; fill: parent; rightMargin: 14 }
		model: pagesModel
		delegate: ListDelegate {
			MoreIndicator {
				anchors.right: parent.right;
				anchors.verticalCenter: parent.verticalCenter
			}
			onClicked: pageStack.push(contentComponent, { handler: model.section });
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
					platformIconId: "toolbar-previous"
					onClicked: pageStack.pop()
				}
			}
		}
	}
	onStatusChanged: {
		if (status == PageStatus.Deactivating && pageStack.depth == 1)
			handler.accept();
	}
}

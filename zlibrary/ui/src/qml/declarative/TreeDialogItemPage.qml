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
	property alias imageSource: content.imageSource
	
//	VisualItemModel {
//		id: visualModel
		DialogContent {
			id: content
			handler: root.handler.createPageContent(root.rootIndex)
//			height: childrenRect.height
//			width: listView.parent.width
//			height: childrenRect.height
//			anchors.top: image.bottom
//			anchors.bottom: parent.bottom
//			width: parent.width
//			anchors { leftMargin: 0; topMargin: 0; rightMargin: 0 }
			anchors { fill: parent; topMargin: 14 }
			
			Component.onCompleted: console.log("Yahoo!", content.handler, content.height, content.childrenRect.height, content.implicitHeight, content.contentHeight)
		}
//	}
	
//	ListView {
//		id: listView
//		model: visualModel
//		anchors.fill: parent
//		anchors { topMargin: 14 }
//	}
}

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

Page {
	id: root
	orientationLock: rootWindow.fixedOrientation ? PageOrientation.LockPrevious : PageOrientation.Automatic
	property variant rootWindow
	property variant handler
	property variant component
	property variant progressData: handler.progressData(root.rootIndex)
	property bool infiniteProgress: progressData === undefined ? false : progressData.infinite
	property bool finiteProgress: progressData === undefined ? false : !progressData.infinite
	property real progressValue: progressData === undefined ? 0.0 : (progressData.value * 1.0 / progressData.maximumValue)
	property variant rootIndex
	property int revision: 0
    default property alias contentChildren: content.data
	
	Connections {
		target: root.handler
		onProgressChanged: {
			++root.revision;
			root.progressData = handler.progressData(root.rootIndex);
		}
		onFinished: root.__close()
	}
	
	function __close() {
		if (root.pageStack.depth == 1)
			return;
		var i = root.pageStack.depth;
		var item = root.pageStack.find(function() { return (--i) === 0; })
		root.pageStack.pop(item);
	}
	
	Item {
		id: content
		anchors {
			leftMargin: 14
			top: root.top
			bottom: progressBar.top
			rightMargin: 14
			left: root.left
			right: root.right
		}
	}
	
	ProgressBar {
		id: progressBar
		anchors { margins: 5; bottom: parent.bottom }
		height: visible ? implicitHeight : 0
		width: parent.width
		visible: root.finiteProgress
		value: root.progressValue
	}
	
	tools: ToolBarLayout {
		id: toolBarLayout
		ToolIcon {
			visible: true
			platformIconId: "toolbar-previous"
			onClicked: { pageStack.pop(); if (pageStack.depth == 1) root.handler.finish(); }
		}
		
		BusyIndicator {
			id: busyIndicator
			anchors.centerIn: parent
			visible: root.infiniteProgress
			running: root.infiniteProgress
		}
		
		ToolIcon {
			visible: mainMenu.hasChildren
			platformIconId: "toolbar-view-menu"
			onClicked: (mainMenu.status == DialogStatus.Closed)
					   ? mainMenu.open()
					   : mainMenu.close()
		}
	}
	TreeDialogMenu {
		id: mainMenu
		modelIndex: root.rootIndex
		visualParent: root.pageStack
	}
}

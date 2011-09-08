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
import org.fbreader 0.14

Rectangle {
	id: root
	property variant handler
	color: Qt.rgba(0.0, 0.0, 0.0, 0.9)
	anchors.margins: -1

	anchors.fill: parent
	Label {
		id: label
		anchors { horizontalCenter: parent.horizontalCenter; bottom: dummySpace.top }
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.Wrap
		width: parent.width
		text: root.handler.text
	}
	Item {
		id: dummySpace
		anchors { bottom: indicator.top }
		height: ((parent.height - indicator.height) / 2.0 - label.height) / 2.0
	}
	BusyIndicator {
		id: indicator
		anchors.centerIn: parent
		platformStyle: BusyIndicatorStyle { size: "large" }
		running: root.visible
	}
	Component.onCompleted: {
		label.style.inverted = true;
		indicator.style.inverted = true;
	}
		
	// eat mouse events
	MouseArea {
		id: mouseEventEater
		anchors.fill: parent
		enabled: parent.visible
	}
	
	Connections {
		target: root.handler
		onFinished: root.destroy()
	}
}

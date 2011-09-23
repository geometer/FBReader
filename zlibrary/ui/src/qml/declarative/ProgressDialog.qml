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

Sheet {
	id: root
	property variant handler

	content: Item {
		anchors.fill: parent
		Label {
			anchors { verticalCenter: parent.verticalCenter; bottom: indicator.top }
			width: parent.width
			text: root.handler.text
		}
		BusyIndicator {
			id: indicator
			anchors.centerIn: parent
			platformStyle: BusyIndicatorStyle { size: "large" }
			running: root.status == DialogStatus.Open
		}
	}
	
	Connections {
		target: root.handler
		onFinished: root.close()
	}
}

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

Dialog {
	id: root
	property variant handler;
    property Style platformStyle: QueryDialogStyle {}
	
//	title: handler.title
	content: Item {
		property int upperBound: visualParent ? visualParent.height - buttonsRow.height - 64
												: root.parent.height - buttonsRow.height - 64
		property int __sizeHint: Math.min(Math.max(root.platformStyle.contentFieldMinSize,
												   dialogContent.contentHeight),
										  upperBound)
		height: __sizeHint + root.platformStyle.contentTopMargin
		width: root.width
		DialogContent {
			id: dialogContent
			invertedTheme: true
			handler: root.handler.content
		}
		Component.onCompleted: {
			console.log(__sizeHint, upperBound, root.platformStyle.contentTopMargin, dialogContent.contentHeight)
		}
	}

	buttons: Column {
        id: buttonsRow
        anchors.top: parent.top
        width: parent.width
        height: childrenRect.height
//		anchors.horizontalCenter: parent.horizontalCenter
		Repeater {
			model: root.handler.buttonNames
			Button {
				text: modelData
				onClicked: {
					var names = root.handler.acceptButtons;
					for (var i = 0; i < names.length; ++i) {
						if (names[i] == text) {
							accept();
							return;
						}
					}
					reject();
				}
				platformStyle: ButtonStyle { inverted: true }
			}
		}
	}
//	MouseArea { anchors.fill: parent }
	onAccepted: if (root.handler) root.handler.accept()
	onRejected: if (root.handler) root.handler.reject()
}

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
import Qt.labs.components.native 1.0
import com.nokia.extras 1.0
import org.fbreader 0.14

Rectangle {
	id: root
	property variant handler
	property real alpha: 0
	color: Qt.rgba(0.0, 0.0, 0.0, alpha)
	anchors.margins: -1
	property int status: DialogStatus.Closed
	visible: status != DialogStatus.Closed;
	state: "closed"

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
	
	function open() {
		root.state = "";
	}
	
	function close() {
		root.state = "closed";
	}
	
	function transitionStarted() {
        status = (state == "closed") ? DialogStatus.Closing : DialogStatus.Opening;
    }
    
    function transitionEnded() {
        status = (state == "closed") ? DialogStatus.Closed : DialogStatus.Open;
    }
	
	states: [
        State {
            name: ""
            PropertyChanges {
				target: root
				alpha: 0.9
			}
        },
        State {
            name: "closed"
            PropertyChanges {
				target: root
				alpha: 0
			}
        }
    ]

    transitions: [
        Transition {
            from: ""
			to: "closed"
			reversible: true
            SequentialAnimation {
                ScriptAction {
					script: {
						if (root.state == "closed")
							root.transitionStarted();
						else
							root.transitionEnded();
						console.log("transition start to", root.state);
					}
				}
                NumberAnimation {
					target: root
					properties: "alpha"
					easing.type: Easing.InOutCubic
					duration: 250
				}
                ScriptAction {
					script: {
						if (root.state == "closed")
							root.transitionEnded();
						else
							root.transitionStarted();
						console.log("transition end to", root.state);
					}
				}
            }
        }
    ]
		
	// eat mouse events
	MouseArea {
		id: mouseEventEater
		anchors.fill: parent
		enabled: parent.visible
	}
	
	Component.onCompleted: {
		label.style.inverted = true;
		indicator.style.inverted = true;
		root.handler.finished.connect(root.close);
	}
	
//	Connections {
//		target: root.handler
//		onFinished: root.close() // root.destroy()
//	}
}

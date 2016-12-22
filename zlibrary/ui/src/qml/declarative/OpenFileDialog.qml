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

Page {
	id: root
	property variant handler
	
	tools: ToolBarLayout {
		ToolIcon {
			platformIconId: "toolbar-previous"
			onClicked: {
				handler.directoryPath = dirModel.rootPath;
				handler.fileName = "";
				handler.finish();
				root.pageStack.pop();
			}
		}
		ToolIcon {
			platformIconId: "toolbar-up"
			onClicked: {
				visualModel.rootIndex = visualModel.parentModelIndex()
			}
		}
	}

	ListView {
        id: view
		anchors.fill: parent
        model: VisualDataModel {
			id: visualModel
            model: FileSystemModel {
				id: dirModel
			}
   
            delegate: Item {
				height: Math.max(textLabel.height, indicator.height)
				width: parent.width
				anchors.bottomMargin: 2
                Text {
					anchors.verticalCenter: parent.verticalCenter
					id: textLabel
					text: fileName
				}
//				visible: model.hasModelChildren || handler.check(model.filePath)
   
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (model.hasModelChildren) {
                            view.model.rootIndex = view.model.modelIndex(index)
						} else {
							handler.directoryPath = dirModel.rootPath;
							handler.fileName = model.fileName;
							handler.finish();
						}
                    }
                }
				MoreIndicator {
					id: indicator
					anchors { verticalCenter: parent.verticalCenter; right: parent.right }
					visible: model.hasModelChildren
				}
            }
        }
    }
//	MouseArea { anchors.fill: parent }
	Component.onCompleted: {
		console.log(handler, dirModel, handler.directoryPath)
		visualModel.rootIndex = dirModel.setRootPath(handler.directoryPath)
		console.log(visualModel.count)
	}
	Connections {
		target: handler
		onFinished: root.pageStack.pop()
	}
}

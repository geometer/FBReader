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
	rejectButtonText: qsTr("Cancel")

	content: ListView {
        id: view
		anchors.fill: parent
        model: VisualDataModel {
			id: visualModel
            model: FileSystemModel {
				id: dirModel
			}
            delegate: ListDelegate {
				id: itemDelegate
				// TODO: Find why dirModel ignores filter QDir::NoDot
				visible: model.directory
						 ? model.fileName != "."
						 : (root.handler !== null && root.handler.check(model.filePath))
				height: visible ? 88 : 0 // UI.LIST_ITEM_HEIGHT
				onClicked: {
					console.log("clicked", model.filePath)
					if (model.directory) {
						if (model.fileName == "..")
							view.model.rootIndex = view.model.parentModelIndex();
						else
							view.model.rootIndex = view.model.modelIndex(index);
						dirModel.rootPath = model.filePath
					} else {
						console.log("finish", dirModel.rootPath, model.fileName)
						handler.directoryPath = dirModel.rootPath;
						handler.fileName = model.filePath;
						handler.finish();
						accept();
					}
				}
				MoreIndicator {
					id: indicator
					anchors { verticalCenter: parent.verticalCenter; right: parent.right }
					visible: model.directory
				}
			}
        }
    }
	Component.onCompleted: {
		console.log(handler, dirModel, handler.directoryPath)
		visualModel.rootIndex = dirModel.setRootPath(handler.directoryPath)
		console.log(visualModel.count)
	}
}

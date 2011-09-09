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

PageStackWindow {
	id: root
	showToolBar: pageStack.currentPage === null
				 || pageStack.currentPage.showToolBar === undefined
				 || pageStack.currentPage.showToolBar
	
	initialPage: MainPage {
		id: mainPage
		OptionsDialog {
			id: optionsDialog
		}
	}

	Connections {
		target: dialogManager

		onDialogRequested: {
			console.log("simple dialog", object, object.title)
			var component = Qt.createComponent("SimpleDialog.qml");
			var dialog = component.createObject(mainPage, { handler: object });
			dialog.open();
		}
		
		onOptionsDialogRequested: {
			console.log("options dialog", object, object.title);
			optionsDialog.handler = object;
			optionsDialog.open();
		}

        onFileDialogRequested: {
			console.log("file dialog", object, object.directoryPath)
			var component = Qt.createComponent("OpenFileDialog.qml");
			var dialog = component.createObject(mainPage, { handler: object });
			dialog.open();
		}
		
		onProgressDialogRequested: {
			console.log("progress dialog", object)
			var component = Qt.createComponent("ProgressDialog.qml");
			var dialog = component.createObject(root.pageStack.parent.parent, { handler: object });
			dialog.open();
		}
		
		onInformationBoxRequested: {
			// var title, message, button
			var args = { "titleText": title, "message": message, "acceptButtonText": button };
			var dialog = queryDialog.createObject(mainPage, args);
			dialog.open();
		}
		onErrorBoxRequested: {
			// var title, message, button
			var args = { "titleText": title, "message": message, "acceptButtonText": button };
			var dialog = queryDialog.createObject(mainPage, args);
			dialog.open();
		}
	}
	
	Component {
		id: queryDialog
		QueryDialog {
		}
	}
}

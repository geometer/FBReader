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

PageStackWindow {
	id: root
	property bool fixedOrientation: false
	property bool invertedTheme: pageStack.currentPage !== null
								 && pageStack.currentPage.invertedTheme !== undefined
								 && pageStack.currentPage.invertedTheme
	showToolBar: pageStack.currentPage === null
				 || pageStack.currentPage.showToolBar === undefined
				 || pageStack.currentPage.showToolBar
	
	onInvertedThemeChanged: theme.inverted = root.invertedTheme
	
	initialPage: MainPage {
		id: mainPage
		rootWindow: root
	}
	
	Connections {
		target: screen
		onCurrentOrientationChanged: root.fixedOrientation = false
	}

	Connections {
		target: dialogManager

		onDialogRequested: {
			var component = Qt.createComponent("SimpleDialog.qml");
			root.openDialog(component.createObject(mainPage, { handler: object }));
		}
		
		onOptionsDialogRequested: {
			var component = Qt.createComponent("OptionsDialog.qml");
			root.pageStack.push(component, { handler: object, rootWindow: root, component: component });
		}
		
        onFileDialogRequested: {
			var component = Qt.createComponent("OpenFileDialog.qml");
			root.openDialog(component.createObject(mainPage, { handler: object }));
		}
		
        onTreeDialogRequested: {
			console.log("bla-bla", object)
			var component = Qt.createComponent("TreeDialogPage.qml");
			root.pageStack.push(component, { handler: object, rootWindow: root, component: component });
		}
		
		onProgressDialogRequested: {
			root.openDialog(progressDialog.createObject(root.pageStack.parent.parent, { handler: object }));
		}
		
		onQuestionDialogRequested: {
			var component = Qt.createComponent("QuestionDialog.qml");
			root.openDialog(component.createObject(root.pageStack.parent.parent, { handler: object }));
		}
		
		onInformationBoxRequested: {
			// var title, message, button
			var args = { "titleText": title, "message": message, "acceptButtonText": button };
			root.openDialog(queryDialog.createObject(mainPage, args));
		}
		onErrorBoxRequested: {
			// var title, message, button
			var args = { "titleText": title, "message": message, "acceptButtonText": button };
			root.openDialog(queryDialog.createObject(mainPage, args));
		}
	}
	
	function openDialog(dialog) {
		if (dialog.open !== undefined)
			dialog.open();
		if (dialog.statusChanged !== undefined) {
			dialog.statusChanged.connect(
						function() {
							if (dialog.status == DialogStatus.Closed) {
								dialog.destroy();
								// hook for toolbar activity
								if (root.pageStack.currentPage == mainPage)
									mainPage.state = ""
							}
						});
		}
	}
	
	Component {
		id: progressDialog
		ProgressDialog {
		}
	}
	
	Component {
		id: queryDialog
		QueryDialog {
		}
	}

//	Component.onCompleted: {
//		theme.inverted = true
//	}

// TODO: Check why it doesn't dissapear sometimes	
//	Rectangle {
//		id: overlayRect
//		anchors.fill: parent
//		color: "#60000000"
//		visible: !platformWindow.active
//		Label {
//			anchors.centerIn: parent
//			width: parent.width
//			text: applicationInfo.bookTitle
//			font.pixelSize: 70
//			font.bold: true
//			color: "white"
//			horizontalAlignment: Text.AlignHCenter
//		}
//	}
}

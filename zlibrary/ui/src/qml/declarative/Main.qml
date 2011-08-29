import QtQuick 1.0
import com.nokia.meego 1.0

PageStackWindow {
	id: root
	
	initialPage: MainPage {
		id: mainPage
	}
	Connections {
		target: dialogManager

        onFileDialogRequested: {
			console.log(object, object.directoryPath)
			var component = Qt.createComponent("OpenFileDialog.qml");
			var dialog = component.createObject(root, { handler: object });
//			dialog.handler = object;
			root.pageStack.push(dialog);
		}
	}
//	Component {
//		id: fileDialogComponent
//		OpenFileDialog {
//		}
//	}
}
//MenuItem {
//	text: modelData.text
//}

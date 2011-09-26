import QtQuick 1.0
import Qt.labs.components.native 1.0

Menu {
	id: menu
	property variant modelIndex
	property bool hasChildren: false
	MenuLayout {
		id: menuLayout
		Repeater {
			id: menuRepeater
			model: root.handler.actions(menu.modelIndex)
			MenuItem {
				text: modelData
				onClicked: root.handler.run(menu.modelIndex, index)
			}
		}
		Component.onCompleted: {
			menu.hasChildren = menuRepeater.count > 0;
			menuRepeater.destroy();
		}
	}
}

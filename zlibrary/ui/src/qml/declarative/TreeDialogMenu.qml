import QtQuick 1.0
import com.nokia.meego 1.0
import "TreeDialogMenu.js" as Engine

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
				id: menuItem
				text: ""
				height: visible ? platformStyle.height : 0
				onClicked: root.handler.run(menu.modelIndex, index)
				Component.onCompleted: Engine.pushItem(menuItem)
			}
		}
		Component.onCompleted: {
			menu.hasChildren = menuRepeater.count > 0;
			menuRepeater.destroy();
		}
	}
	onStatusChanged: {
		console.log("statusChanged", status, DialogStatus.Opening, Engine.menuItems)
		if (status == DialogStatus.Opening) {
			Engine.prepareItems(root.handler, menu.modelIndex);
		}
	}
}

import QtQuick 1.0
import com.nokia.meego 1.0

Menu {
	id: menu
	property variant modelIndex
	property bool hasChildren: false
	property variant menuItems: []
	MenuLayout {
		id: menuLayout
		Repeater {
			id: menuRepeater
			model: root.handler.actions(menu.modelIndex)
			MenuItem {
				id: menuItem
				text: modelData
				height: visible ? platformStyle.height : 0
				onClicked: root.handler.run(menu.modelIndex, index)
				Component.onCompleted: menu.menuItems.push(parent)
			}
		}
		Component.onCompleted: {
			menu.hasChildren = menuRepeater.count > 0;
			menuRepeater.destroy();
		}
	}
	onStatusChanged: {
		if (status == DialogStatus.Opening) {
			for (var i = 0; i < menu.menuItems.length; ++i)
				menu.menuItems[i].visible = root.handler.isVisibleAction(menu.modelIndex, i);
		}
	}
}

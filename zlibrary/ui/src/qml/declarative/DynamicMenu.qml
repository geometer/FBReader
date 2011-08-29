import QtQuick 1.0
import com.nokia.meego 1.0

Menu {
	id: root
	property variant item
	MenuLayout {
		Repeater {
			id: repeater
			model: root.item.items
			MenuItem {
				text: modelData
				onClicked: root.item.activate(index);
			}
		}
	}
}

import QtQuick 1.0
import com.nokia.meego 1.0
import Qt.labs.gestures 1.0
import org.fbreader 0.14

Page {
	id: root
	tools: ToolBarLayout {
		Repeater {
			model: applicationInfo.actions
			ToolIcon {
				id: item
				property variant menu;
				iconSource: modelData.platformIconId === "" ? modelData.iconSource : ""
				platformIconId: modelData.platformIconId
				onClicked: {
					switch (modelData.type) {
					case ToolBarItem.PlainButton:
					case ToolBarItem.ToggleButton:
						modelData.activate();
						break;
					case ToolBarItem.MenuButton:
						if (menu === undefined) {
							var component = Qt.createComponent("DynamicMenu.qml");
							var args = { item: modelData, visualParent: root.pageStack };
							menu = component.createObject(root, args);
						}

						if (menu.status == DialogStatus.Closed)
							menu.open()
						else
							menu.close();
						break;
					default:
						break;
					}

				}
				visible: modelData.visible && modelData.enabled
			}
		}
		ToolIcon {
			platformIconId: "toolbar-view-menu"
			onClicked: (mainMenu.status == DialogStatus.Closed)
					   ? mainMenu.open()
					   : mainMenu.close();
		}
	}
	DynamicMenu {
		id: mainMenu
		visualParent: root.pageStack
		item: applicationInfo.menuBar
	}
	BookView {
		id: bookView
		anchors.fill: parent
		holder: objectHolder
//		onTap: {
//			console.log("tap pos = (", gesture.position.x, "," ,gesture.position.y, ")");
//			objectHolder.handleFingerTap(gesture.position.x, gesture.position.y);
//		}
//		onSwipe: {
//			console.log("swipe angle=", gesture.swipeAngle);
//			if (gesture.swipeAngle > 180)
//				objectHolder.handleScrollBarPageStep(-1);
//			else
//				objectHolder.handleScrollBarPageStep(+1);
//		}
		Component.onCompleted: objectHolder.bookView = bookView
		MouseArea {
			anchors.fill: parent
			onClicked: {
				console.log("tap pos = (", mouse.x, "," ,mouse.y, ")");
				objectHolder.handleFingerTap(mouse.x, mouse.y);
			}
		}
	}
//	GestureArea {
//		anchors.fill: parent
//		focus: true
//		onTap: {
//			console.log("tap pos = (", gesture.position.x, "," ,gesture.position.y, ")");
//			objectHolder.handleFingerTap(gesture.position.x, gesture.position.y);
//		}
//		onSwipe: {
//			console.log("swipe angle=", gesture.swipeAngle);
//			if (gesture.swipeAngle > 180)
//				objectHolder.handleScrollBarPageStep(-1);
//			else
//				objectHolder.handleScrollBarPageStep(+1);
//		}
//	}
}

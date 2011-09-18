import QtQuick 1.0
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.fbreader 0.14

Page {
	id: root
	property variant handler
	property variant component
	property alias rootIndex: visualModel.rootIndex
	
	VisualDataModel {
		id: visualModel
		model: root.handler
		delegate: ListDelegate {
			signal tapAndHold
			onClicked: {
				console.log(model.title, model.activatable, model.page)
				if (model.activatable) {
					if (root.handler.activate(visualModel.modelIndex(index))) {
						var i = root.pageStack.depth;
						var item = root.pageStack.find(function() { return (--i) === 0; })
						root.pageStack.pop(item);
						root.handler.finish();
					}
				} else {
					var rootIndex = visualModel.modelIndex(index);
					root.pageStack.push(root.component, {
										"handler": root.handler,
										"component": root.component,
										"rootIndex": rootIndex
					});
				}
			}
			onTapAndHold: {
				var index = visualModel.modelIndex(index);
				var menu = menuComponent.createObject(listView.parent, { "modelIndex": index });
				menu.open();
				menu.statusChanged.connect(
							function() {
								if (menu.status == DialogStatus.Closed)
									menu.destroy();
							});
				console.log("tap and hold on " + model.title, ":",
							root.handler.actions(visualModel.modelIndex(index)))
			}

			Timer {
				interval: 800
				running: parent.pressed
				onTriggered: parent.tapAndHold()
			}
		}
	}
	
	ListView {
		id: listView
		anchors.fill: parent
		model: visualModel
	}
	
	tools: ToolBarLayout {
		id: toolBarLayout
		ToolIcon {
			visible: true
			platformIconId: "toolbar-previous"
			onClicked: { pageStack.pop(); if (pageStack.depth == 1) root.handler.finish(); }
		}
		
		ToolIcon {
			// TODO: Add menu
			platformIconId: "toolbar-view-menu"
//			onClicked: {
//				if (mainMenu.status == DialogStatus.Closed) {
//					mainMenu.item.recheckItems();
//					mainMenu.open()
//				} else {
//					mainMenu.close();
//				}
//			}
		}
	}
	Component {
		id: menuComponent
		Menu {
			id: menu
			property variant modelIndex
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
				Component.onCompleted: menuRepeater.destroy()
			}
//			Component.onCompleted: {
//				var actions = root.handler.actions(index);
//				console.log(menuLayout.menuChildren);
//				console.log(JSON.stringify(menuLayout.menuChildren));
//				for (var prop in menuLayout.menuChildren)
//					console.log(prop, menuLayout.menuChildren[prop]);
//				var items = [];
//				for (var i = 0; i < actions.length; ++i) {
//					var args = { "index": menu.index, "actionId": i };
//					var item = menuItemComponent.createObject(null, args);
//					menuLayout.menuChildren.push(item);
//				}
////				menuLayout.menuChildren = items;
//			}
		}
	}
	Component {
		id: menuItemComponent
		MenuItem {
			property variant index
			property int actionId
			onClicked: root.handler.run(index, actionId)
		}
	}

	Component.onCompleted: root.handler.fetchChildren(rootIndex)
}

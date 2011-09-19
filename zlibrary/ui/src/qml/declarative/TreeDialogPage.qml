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
			property bool hasTapAndHold: false
			onClicked: {
				if (hasTapAndHold)
					return;
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
			onPressedChanged: if (pressed) hasTapAndHold = false
			onTapAndHold: {
				var index = visualModel.modelIndex(index);
				var menu = menuComponent.createObject(listView.parent, { "modelIndex": index });
				if (menu.hasChildren) {
					hasTapAndHold = true;
					menu.show();
				} else {
					menu.destroy();
				}

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
			visible: mainMenu.hasChildren
			platformIconId: "toolbar-view-menu"
			onClicked: (mainMenu.status == DialogStatus.Closed)
					   ? mainMenu.open()
					   : mainMenu.close()
		}
	}
	TreeDialogMenu {
		id: mainMenu
		modelIndex: root.rootIndex
		visualParent: root.pageStack
	}

	Component {
		id: menuComponent
		TreeDialogMenu {
			id: menu
			visualParent: root.pageStack
			function show() {
				open();
				menu.statusChanged.connect(
							function() {
								if (menu.status == DialogStatus.Closed)
									menu.destroy();
							});
			}
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

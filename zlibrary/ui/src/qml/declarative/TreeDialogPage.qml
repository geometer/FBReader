import QtQuick 1.0
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import org.fbreader 0.14

Page {
	id: root
	orientationLock: rootWindow.fixedOrientation ? PageOrientation.LockPrevious : PageOrientation.Automatic
	property variant rootWindow
	property variant handler
	property variant component
	property variant progressData: handler.progressData(root.rootIndex)
	property bool infiniteProgress: progressData === undefined ? false : progressData.infinite
	property bool finiteProgress: progressData === undefined ? false : !progressData.infinite
	property real progressValue: progressData === undefined ? 0.0 : (progressData.value * 1.0 / progressData.maximumValue)
	property alias rootIndex: visualModel.rootIndex
	property int revision: 0
	
	Connections {
		target: root.handler
		onProgressChanged: {
			++root.revision;
			root.progressData = handler.progressData(root.rootIndex);
		}
		onFinished: root.__close()
	}
	
	function __close() {
		if (root.pageStack.depth == 1)
			return;
		var i = root.pageStack.depth;
		var item = root.pageStack.find(function() { return (--i) === 0; })
		root.pageStack.pop(item);
	}
	
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
//						root.__close();
						root.handler.finish();
					}
				} else {
					var rootIndex = visualModel.modelIndex(index);
					var page = root.pageStack.push(root.component, {
												   "handler": root.handler,
												   "rootWindow": root.rootWindow,
												   "component": root.component,
												   "rootIndex": rootIndex
												   });
				}
			}
			onPressedChanged: if (pressed) hasTapAndHold = false
			onTapAndHold: {
				console.log("Tap on", model.title, model.iconSource)
				var menu = menuComponent.createObject(listView.parent, { "modelIndex": visualModel.modelIndex(index) });
				if (menu.hasChildren) {
					hasTapAndHold = true;
					menu.show();
				} else {
					menu.destroy();
				}
			}
			
			MoreIndicator {
				id: indicator
				anchors { verticalCenter: parent.verticalCenter; right: parent.right }
				visible: !model.activatable && !itemBusyIndicator.visible
			}
			BusyIndicator {
				id: itemBusyIndicator
				anchors { verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: 5 }
				visible: root.revision >= 0 && root.handler.progressData(visualModel.modelIndex(index)) !== undefined
				running: visible
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
		anchors { leftMargin: 14; top: parent.top; bottom: progressBar.top; rightMargin: 14 }
		width: parent.width
		model: visualModel
	}
	
	ProgressBar {
		id: progressBar
		anchors { margins: 5; bottom: parent.bottom }
		height: visible ? implicitHeight : 0
		width: parent.width
		visible: root.finiteProgress
		value: root.progressValue
	}
	
	tools: ToolBarLayout {
		id: toolBarLayout
		ToolIcon {
			visible: true
			platformIconId: "toolbar-previous"
			onClicked: { pageStack.pop(); if (pageStack.depth == 1) root.handler.finish(); }
		}
		
		BusyIndicator {
			id: busyIndicator
			anchors.centerIn: parent
			visible: root.infiniteProgress
			running: root.infiniteProgress
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

	Component.onCompleted: {
		root.handler.fetchChildren(rootIndex)
		console.log("Component.onCompleted", root.rootModel, root.fetchingChildren)
	}
}

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
			onClicked: {
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
		}
	}
	
	ListView {
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
	
	Component.onCompleted: root.handler.fetchChildren(rootIndex)
}

var menuItems = []

function pushItem(item) {
	menuItems.push(item)
}

function prepareItems(handler, index) {
	var names = handler.actions(index);
	for (var i = 0; i < menuItems.length; ++i) {
		console.log("status", menuItems[i], names[i])
		menuItems[i].text = names[i];
		menuItems[i].visible = handler.isVisibleAction(index, i);
	}
}

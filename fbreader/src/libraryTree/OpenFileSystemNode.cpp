#include <ZLDialogManager.h>

#include "../fbreader/FBReader.h"
#include "../library/Library.h"
#include "../formats/FormatPlugin.h"
#include "LibraryNodes.h"

#include "LibraryTreeNodes.h"



const ZLTypeId OpenFileSystemNode::TYPE_ID(ZLTreeActionNode::TYPE_ID);

const ZLTypeId &OpenFileSystemNode::typeId() const {
	return TYPE_ID;
}

std::string OpenFileSystemNode::title() const {
        //TODO add not-toolbar resource for this node
        return ZLResource::resource("toolbar")["addBook"]["label"].value();
}

std::string OpenFileSystemNode::subtitle() const {
        //TODO add not-toolbar resource for this node
        return ZLResource::resource("toolbar")["addBook"]["tooltip"].value();
}

static const std::string GROUP_NAME = "OpenFileDialog";

OpenFileSystemNode::OpenFileSystemNode() :
DirectoryOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP_NAME, "Directory", ZLFile("~").path()),
FileOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP_NAME, "File", std::string()) {
}

bool OpenFileSystemNode::activate() {
	//TODO call AddBookAction here
	const ZLResourceKey dialogKey("addFileDialog");
	FileFilter filter;
	shared_ptr<ZLOpenFileDialog> dialog = ZLDialogManager::Instance().createOpenFileDialog(dialogKey, DirectoryOption.value(), FileOption.value(), filter);
	bool code = dialog->run();
	DirectoryOption.setValue(dialog->directoryPath());
	FileOption.setValue(dialog->filePath());
	if (code) {
		shared_ptr<Book> book;
		ZLFile file(dialog->filePath());
		FBReader::Instance().createBook(file, book);
		if (!book.isNull()) {
			FBReader::Instance().openBook(book);
			return true;
		}
	}
	return false;
}

bool OpenFileSystemNode::FileFilter::accepts(const ZLFile &file) const {
	return file.isArchive() || !PluginCollection::Instance().plugin(file, false).isNull();
}

std::string OpenFileSystemNode::imageUrl() const {
	return FBNode::defaultImageUrl("booktree-folder.png");
}

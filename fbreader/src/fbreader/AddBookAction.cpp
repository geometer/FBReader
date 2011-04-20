/*
 * Copyright (C) 2004-2011 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <ZLDialogManager.h>
#include <ZLOptionsDialog.h>
#include <ZLFile.h>

#include "FBReaderActions.h"
#include "FBReader.h"
#include "../library/Book.h"
#include "../formats/FormatPlugin.h"

class AddBookAction::FileFilter : public ZLOpenFileDialog::Filter {

private:
	bool accepts(const ZLFile &file) const;
};

bool AddBookAction::FileFilter::accepts(const ZLFile &file) const {
	return file.isArchive() || !PluginCollection::Instance().plugin(file, false).isNull();
}

static const std::string GROUP_NAME = "OpenFileDialog";

AddBookAction::AddBookAction(int visibleInModes) :
	ModeDependentAction(visibleInModes),
	DirectoryOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP_NAME, "Directory", ZLFile("~").path()),
	FileOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP_NAME, "File", std::string()) {
}

void AddBookAction::run() {
	const ZLResourceKey dialogKey("addFileDialog");

	FileFilter filter;
	shared_ptr<ZLOpenFileDialog> dialog = ZLDialogManager::Instance().createOpenFileDialog(dialogKey, DirectoryOption.value(), FileOption.value(), filter);
	bool code = dialog->run();
	DirectoryOption.setValue(dialog->directoryPath());
	FileOption.setValue(dialog->filePath());
	if (code) {
		FBReader::Instance().openFile(ZLFile(dialog->filePath()));
	}
}

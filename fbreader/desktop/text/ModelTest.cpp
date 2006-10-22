#include <iostream>

#include <abstract/ZLEncodingConverter.h>
#include <abstract/XMLOptions.h>
#include <unix/ZLUnixFSManager.h>

#include "../../common/model/Paragraph.h"
#include "../../common/bookmodel/BookModel.h"
#include "../../common/bookmodel/BookReader.h"

#include "../../common/collection/BookCollection.h"
#include "../../common/fbreader/CollectionView.h"
#include "../../common/fbreader/FBFileHandler.h"

const std::string FBFileHandler::ImageDirectory = "FBReader";

const std::string CollectionView::DeleteBookImageFile = Files::PathPrefix + "icons/FBReader/remove.png";
const std::string BookCollection::DefaultBookPath = "~/FBooks:~/Books";

void dumpParagraph(const Paragraph &paragraph) {
	std::cout << "PARAGRAPH: ";
	for (Paragraph::Iterator it(paragraph); !it.isEnd(); it.next()) {
		ParagraphEntry::Kind kind = it.entryKind();
		std::cout << kind << " ";
		shared_ptr<ParagraphEntry> entry = it.entry();
		if (entry.isNull()) {
			std::cout << "NULL";
		} else {
			switch (kind) {
				case ParagraphEntry::TEXT_ENTRY:
					{
						TextEntry &text = (TextEntry&)*entry;
						std::string s;
						s.append(text.data(), text.dataLength());
						std::cout << s;
					}
					break;
				case ParagraphEntry::IMAGE_ENTRY:
					break;
				case ParagraphEntry::CONTROL_ENTRY:
				case ParagraphEntry::HYPERLINK_CONTROL_ENTRY:
					{
						ControlEntry &control = (ControlEntry&)*entry;
						std::cout << "CTRL" << control.kind();
						std::cout << (control.isStart() ? "+" : "-");
						std::cout << (control.isHyperlink() ? "+" : "-");
					}
					break;
				case ParagraphEntry::FORCED_CONTROL_ENTRY:
					break;
			}
		}
		std::cout << ";";
	}
	std::cout << "\n";
}

void dumpModel(const TextModel &model) {
	const int N = model.paragraphsNumber();
	for (int i = 0; i < N; ++i) {
		dumpParagraph(*model[i]);
	}
}

void dumpModel(const BookModel &model) {
	std::cout << "Book text:\n";
	dumpModel(model.bookTextModel());
	std::cout << "Contents table:\n";
	dumpModel(model.contentsModel());
	// TODO: dump all footnotes models
}

int main(int argc, const char **argv) {
	if (argc != 2) {
		std::cerr << "usage:\n	" << argv[0] << " <book file name>\n";
		return 1;
	}

	ZLUnixFSManager::createInstance();
	ZLEncodingConverter::setEncodingDescriptionPath(Files::PathPrefix + "encodings");
	XMLOptions::createInstance("FBReader");

	BookDescriptionPtr description = BookDescription::create(argv[1]);
	if (description.isNull()) {
		std::cerr << "couldn't read file " << argv[1] << "\n";
		return 1;
	}

	dumpModel(BookModel(description));

	return 0;
}

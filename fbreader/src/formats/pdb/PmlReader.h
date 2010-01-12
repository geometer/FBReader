/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

/* 
 * Information about Palm Markup Language was taken from next sources:
 * http://www.m.ereader.com/ereader/help/dropbook/pml.htm
 * http://ccit205.wikispaces.com/Palm+Markup+Language+(PML)
 */

#ifndef __PMLREADER_H__
#define __PMLREADER_H__

#include <string>

#include <ZLEncodingConverter.h>
#include <ZLTextAlignmentType.h>

#include "../EncodedTextReader.h"
#include "../../bookmodel/FBTextKind.h"

class ZLInputStream;

class PmlReader : public EncodedTextReader {

public:
	virtual bool readDocument(ZLInputStream &stream);

protected:
	PmlReader(const std::string &encoding);
	virtual ~PmlReader();

protected:	
 	enum FontProperty {
        FONT_BOLD,
        FONT_ITALIC,
        FONT_UNDERLINED,
		FONT_SUBSCRIPT,
		FONT_SUPERSCRIPT
    };
	
	enum FontSizeType {
		NORMAL,
		SMALLER,
		LARGER
	};


	virtual void addCharData(const char *data, size_t len, bool convert) = 0;
    virtual void addLink(FBTextKind kind, const std::string &id, bool on) = 0;
	virtual void addLinkLabel(const std::string &label) = 0;
	virtual void addImageReference(const std::string &id) = 0;
    virtual void setFontSize() = 0;
    virtual void switchFontProperty(FontProperty property) = 0;
    virtual void newLine() = 0;
    virtual void newPage() = 0;
    virtual void newParagraph() = 0;

	void interrupt();

private:
	bool parseDocument(ZLInputStream &stream);
	void processTag(std::string &tagName, const std::string &parameter = ourDefaultParameter);
	void processCharData(const char* data, size_t len, bool convert = true);
    void processFontProperty(FontProperty property);
    void processAlignment(ZLTextAlignmentType alignment);
    void processFontSize(FontSizeType sizeType);
    void processIndent(const std::string &parameter =ourDefaultParameter);
    void processLink(FBTextKind kind, const std::string &parameter);
	
	static size_t findTagLength(const char* ptr);

protected:
    struct PmlReaderState {
        bool Bold;
        bool Italic;
        bool Underlined;
		bool SmallCaps;
		bool Subscript;
		bool Superscript;
        
		ZLTextAlignmentType Alignment;
		FontSizeType FontSize;
		
		unsigned short Indent;
		bool IndentBlockOn;
		bool BoldBlockOn;
		bool FootnoteLinkOn;	
		bool InternalLinkOn;
		bool InvisibleText;	
    };

    PmlReaderState myState;
	
private:
	char* myStreamBuffer;

	bool myIsInterrupted;
	const static std::string ourDefaultParameter;
};

#endif /* __PMLREADER_H__ */

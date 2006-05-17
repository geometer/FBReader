/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
 * Copyright (C) 2006 Vladimir Sorokin
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

#ifndef __RTFREADER_H__
#define __RTFREADER_H__

#include <string>
#include <map>
#include <stack>

#include <abstract/ZLFSManager.h>
#include <abstract/ZLEncodingConverter.h>

#include "../../model/AlignmentType.h"

class ZLInputStream;

class RtfReader {

private:
  static void RtfReader::fillKeywordMap();

private:
	class RtfCommand;
  static std::map<std::string, RtfCommand*> ourKeywordMap;

protected:
  RtfReader(const std::string &encoding);
  virtual ~RtfReader();

public:
  virtual bool readDocument(const std::string &fileName);

protected:
  enum DestinationType {
    DESTINATION_NONE,
    DESTINATION_SKIP,
    DESTINATION_INFO,
    DESTINATION_TITLE,
    DESTINATION_AUTHOR,
    DESTINATION_PICTURE,
    DESTINATION_STYLESHEET,
    DESTINATION_FOOTNOTE,
  };

  enum FontProperty {
    FONT_BOLD,
    FONT_ITALIC,
    FONT_UNDERLINED
  };
    
  virtual void addCharData(const char *data, size_t len, bool convert) = 0;
  virtual void insertImage(const std::string &mimeType, const std::string &fileName, size_t startOffset, size_t size) = 0;
	virtual void setEncoding(int code) = 0;
  virtual void switchDestination(DestinationType destination, bool on) = 0;
  virtual void setAlignment(AlignmentType alignment) = 0;
  virtual void setFontProperty(FontProperty property) = 0;
  virtual void newParagraph() = 0;

  void interrupt();

private:
  class RtfCommand {
  protected:
    virtual ~RtfCommand();

  public:
    virtual void run(RtfReader &reader, int *parameter) const = 0;
  };

  friend class RtfNewParagraphCommand : public RtfCommand {
  public:
    void run(RtfReader &reader, int *parameter) const;
  };

  friend class RtfFontPropertyCommand : public RtfCommand {
  public:
    RtfFontPropertyCommand(FontProperty property);
    void run(RtfReader &reader, int *parameter) const;

  private:
    RtfReader::FontProperty myProperty;
  };

  friend class RtfAlignmentCommand : public RtfCommand {
  public:
    RtfAlignmentCommand(AlignmentType alignment);
    void run(RtfReader &reader, int *parameter) const;
  
  private:
    AlignmentType myAlignment;
  };

  friend class RtfCharCommand : public RtfCommand {
  public:
    RtfCharCommand(const std::string &chr);
    void run(RtfReader &reader, int *parameter) const;

  private:
    std::string myChar;
  };

  friend class RtfDestinationCommand : public RtfCommand {
  public:
    RtfDestinationCommand(DestinationType dest);
    void run(RtfReader &reader, int *parameter) const;

  private:
    DestinationType myDestination;
  };

  friend class RtfStyleCommand : public RtfCommand {
  public:
    void run(RtfReader &reader, int *parameter) const;
  };

  friend class RtfSpecialCommand : public RtfCommand {
    void run(RtfReader &reader, int *parameter) const;
  };

  friend class RtfPictureCommand : public RtfCommand {
  public:
    RtfPictureCommand(const std::string &mimeType);
    void run(RtfReader &reader, int *parameter) const;

  private:
    const std::string myMimeType;
  };

  friend class RtfFontResetCommand : public RtfCommand {
  public:
    void run(RtfReader &reader, int *parameter) const;
  };
  
  friend class RtfCodepageCommand : public RtfCommand {
  public:
    void run(RtfReader &reader, int *parameter) const;
  };
  
  bool parseDocument();
  void processKeyword(const std::string &keyword, int *parameter = 0);
  void processCharData(const char *data, size_t len, bool convert = true);

protected:
  struct RtfReaderState {
    bool Bold;
    bool Italic;
    bool Underlined;
    AlignmentType Alignment;
    DestinationType Destination;
  
    bool ReadDataAsHex;
  };

  RtfReaderState myState;
  shared_ptr<ZLEncodingConverter> myConverter;

private:
  bool mySpecialMode;

  std::string myFileName;
  shared_ptr<ZLInputStream> myStream;
  char *myStreamBuffer;

  std::stack<RtfReaderState> myStateStack;

  int myBinaryDataSize;
  std::string myNextImageMimeType;

  int myIsInterrupted;  
};

#endif /* __RTFREADER_H__ */

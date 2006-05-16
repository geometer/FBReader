/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

enum Destination {
  DESTINATION_NONE,
  DESTINATION_SKIP,
  DESTINATION_INFO,
  DESTINATION_TITLE,
  DESTINATION_AUTHOR,
  DESTINATION_PICTURE,
  DESTINATION_STYLESHEET,
  DESTINATION_FOOTNOTE,
};

// property save structure
struct RtfReaderState
{
    bool Bold;
    bool Italic;
    bool Underlined;
    AlignmentType alignment;
    Destination rds;

    bool ReadDataAsHex;
};


class ZLInputStream;

class RtfReader {

private:
  static void RtfReader::fillKeywordMap();

  class RtfCommand;
  static std::map<std::string, RtfCommand*> ourKeywordMap;

public:
  bool readDocument(const std::string &fileName);

protected:
  RtfReader(const std::string &encoding);
  virtual ~RtfReader();

protected:
  std::string encoding;
  
  virtual void startDocumentHandler() = 0;
  virtual void endDocumentHandler() = 0;

  virtual void startElementHandler(int tag) = 0;

  virtual void addCharData(const char *data, size_t len, bool convert) = 0;

  virtual void insertImage(const std::string &mimeType, const std::string &fileName, size_t startOffset, size_t size) = 0;

  void interrupt(void);
  shared_ptr<ZLEncodingConverter> myConverter;

protected:
  enum FontProperty {
    FONT_BOLD,
    FONT_ITALIC,
    FONT_UNDERLINED
  };
    
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
    RtfDestinationCommand(Destination dest);
    void run(RtfReader &reader, int *parameter) const;

  private:
    Destination myDestination;
  };

  friend class RtfStyleCommand : public RtfCommand {
  public:
    void run(RtfReader &reader, int *parameter) const;
  };

  friend class RtfSpecCommand : public RtfCommand {
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
  
private:
  void ecParseCharData(const char *data, size_t len, bool convert = true);
  // TODO: change to pure virtual
  virtual void switchDestination(Destination destiantion, bool on) = 0;
  virtual void setAlignment(AlignmentType) {}
  virtual void setFontProperty(FontProperty property, bool start) = 0;
  virtual void newParagraph() = 0;

  void ecTranslateKeyword(const std::string &keyword, int param, bool fParam);

  int parseDocument();

  bool fSkipDestIfUnk;

  RtfReaderState state;

private:
  std::string myFileName;
  shared_ptr<ZLInputStream> myStream;
  char *myStreamBuffer;

  std::stack<RtfReaderState> myStateStack;

  int myBinaryDataSize;
  std::string myNextImageMimeType;

  int myIsInterrupted;  
};

#endif /* __RTFREADER_H__ */

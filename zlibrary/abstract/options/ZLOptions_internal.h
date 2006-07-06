/*
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

#ifndef __ZLOPTIONS_INTERNAL_H__
#define __ZLOPTIONS_INTERNAL_H__

#include <string>

class ZLOptions {

protected:
  static ZLOptions *ourInstance;

protected:
  ZLOptions();
  virtual ~ZLOptions();

public:
  static ZLOptions &instance();
  static void deleteInstance();

  virtual void setGroup(const std::string &name) = 0;
  virtual void clearGroup() = 0;

  virtual void unsetValue(const std::string &name) = 0;

  virtual bool booleanValue(const std::string &name, bool defaultValue) = 0;
  virtual void setValue(const std::string &name, bool value, const std::string &category) = 0;

  virtual long integerValue(const std::string &name, long defaultValue) = 0;
  virtual void setValue(const std::string &name, long value, const std::string &category) = 0;

  virtual double doubleValue(const std::string &name, double defaultValue) = 0;
  virtual void setValue(const std::string &name, double value, const std::string &category) = 0;

  virtual std::string stringValue(const std::string &name, const std::string &defaultValue) = 0;
  virtual void setValue(const std::string &name, const std::string &value, const std::string &category) = 0;

  virtual void setCategory(const std::string &name, const std::string &category) = 0;

	virtual void startAutoSave(int seconds) = 0;
};

#endif /* __ZLOPTIONS_INTERNAL_H__ */

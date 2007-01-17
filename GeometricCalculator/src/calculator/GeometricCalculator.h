/*
 * Geometric Calculator -- interactive geometry program
 * Copyright (C) 2003-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __GEOMETRICCALCULATOR_H__
#define __GEOMETRICCALCULATOR_H__

#include <ZLApplication.h>
#include <ZLKeyBindings.h>

class DiagramView;
class Document;

class GeometricCalculator : public ZLApplication { 

public:
	static std::string ConfigDirectory();

public:
	GeometricCalculator(const std::string &fileName);
	~GeometricCalculator();

	void initWindow();
	ZLKeyBindings &keyBindings();

private:
	const std::string defaultSceneFileName() const;

private:
	bool saveIfChanged();
	void newScene();
	void open();
	void open(const std::string &fileName);
	bool save();
	void save(const std::string &fileName, const std::string &sceneName);

	Document &document() const;

	//void help();

private:
	ZLKeyBindings myBindings;

friend class ShowOptionsDialogAction;
friend class ShowInfoDialogAction;
friend class OpenSceneAction;
friend class SaveSceneAction;
friend class NewSceneAction;
friend class QuitAction;
};

#endif /* __GEOMETRICCALCULATOR_H__ */

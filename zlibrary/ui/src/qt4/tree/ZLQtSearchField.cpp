/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <QtCore/QDebug>
#include <QtGui/QStyle>
#include <QtGui/QCompleter>
#include <QtGui/QStringListModel>

#include <ZLibrary.h>
#include <ZLFile.h>
#include <ZLResource.h>
#include <ZLOptions.h>
#include <ZLStringUtil.h>

#include "ZLQtSearchField.h"

ZLQtSearchField::ZLQtSearchField(QWidget *parent) : QLineEdit(parent) {
	//TODO somehow make it feathered
	//TODO set active and non-active color by default of system

	setObjectName("search-field");

	myWaitingIcon = new QtWaitingSpinner(12, 3, 2, 3, this);
	myWaitingIcon->setSpeed(2);

	mySearchIcon = new QLabel(this);
	static std::string iconPath = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter + "search_icon.png";
	QPixmap searchIcon = QPixmap(ZLFile(iconPath).path().c_str());
	mySearchIcon->setPixmap(searchIcon);
	mySearchIcon->setFixedSize(searchIcon.size());

	setFixedSize(155,25);

	setPlaceholderText(QString::fromStdString(ZLResource::resource("networkView")["searchResultNode"]["searchfield"].value()));

	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

	setStyleSheet(QString("QLineEdit { padding-left: %1px; } ").arg(mySearchIcon->sizeHint().width() + frameWidth));

	QCompleter *completer = new QCompleter(this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	this->setCompleter(completer);

	loadSuggestions();
	connect(this, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
}

QtWaitingSpinner *ZLQtSearchField::getWaitingIcon() {
	return myWaitingIcon;
}

void ZLQtSearchField::onReturnPressed() {
	if (text().isEmpty()) {
		return;
	}
	if (mySuggestions.contains(text())) {
		return;
	}
	mySuggestions.insert(text());
	saveSuggestions();
}

void ZLQtSearchField::updateSuggestions() {
	QStringListModel *model = new QStringListModel(mySuggestions.toList(), this);
	this->completer()->setModel(model);
}

static const std::string SUGGESTION = "suggestion";
static const std::string SUGGESTIONS = "suggestions";

void ZLQtSearchField::loadSuggestions() {
	mySuggestions.clear();
	bool finished = false;
	for (unsigned int i = 0; !finished; ++i) {
		std::string suggestOptionName(SUGGESTION);
		ZLStringUtil::appendNumber(suggestOptionName, i);
		std::string suggestion = ZLStringOption(ZLCategoryKey::NETWORK, SUGGESTIONS, suggestOptionName, "").value();
		if (suggestion.empty()) {
			finished = true;
		} else {
			mySuggestions.insert(QString::fromStdString(suggestion));
		}
	}
	updateSuggestions();
}

void ZLQtSearchField::saveSuggestions() {
	QList<QString> suggestions = mySuggestions.values();
	for (int i = 0; i < suggestions.size(); ++i) {
		if (suggestions.at(i).isEmpty()) {
			continue;
		}
		std::string suggestOptionName(SUGGESTION);
		ZLStringUtil::appendNumber(suggestOptionName, (unsigned int)i);
		ZLStringOption suggestion(ZLCategoryKey::NETWORK, SUGGESTIONS, suggestOptionName, "");
		suggestion.setValue(suggestions.at(i).toStdString());
	}
	updateSuggestions();
}

void ZLQtSearchField::resizeEvent(QResizeEvent *ev) {
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	mySearchIcon->move(rect().left() + frameWidth + 4, (rect().bottom() - mySearchIcon->sizeHint().height())/2 + 2);

	QSize waitingIconSize = myWaitingIcon->size();
	myWaitingIcon->move(rect().right() - frameWidth - waitingIconSize.width() - 2, (rect().bottom() + 1 - waitingIconSize.height())/2);
	QLineEdit::resizeEvent(ev);
}

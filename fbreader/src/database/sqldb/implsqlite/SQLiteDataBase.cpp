/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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


#include <ZLStringUtil.h>

#include "SQLiteDataBase.h"

#include "SQLiteConnection.h"
#include "SQLiteCommand.h"

//----------- Transaction subclass -----------------

SQLiteDataBase::Transaction::Transaction(SQLiteDataBase &db) : myDataBase(db), mySuccess(false), myStarted(false), myDepth((unsigned int)-1) {
}

SQLiteDataBase::Transaction::~Transaction() {
	if (myStarted) {
		end(mySuccess);
	}
}

bool SQLiteDataBase::Transaction::start() {
	myDepth = myDataBase.myTransactionDepth;
	if (myDepth == 0) {
		myStarted = myDataBase.myBeginTransaction->execute();
	} else {
		//((DBTextValue &) *myDataBase.myMakeSavePoint->parameter("@name").value()).setValue(name());
		//myStarted = myDataBase.myMakeSavePoint->execute();
		myStarted = true;
	}
	if (myStarted) {
		++myDataBase.myTransactionDepth;
	}
	return myStarted;
}

void SQLiteDataBase::Transaction::end(bool success) {
	--myDataBase.myTransactionDepth;
	if (myDepth == 0) {
		if (success) {
			myDataBase.myCommitTransaction->execute();
		} else {
			myDataBase.myRollbackTransaction->execute();
		}
	} else {
		if (success) {
			//((DBTextValue &) *myDataBase.myCommitSavePoint->parameter("@name").value()).setValue(name());
			//myDataBase.myCommitSavePoint->execute();
		} else {
			//((DBTextValue &) *myDataBase.myRollbackSavePoint->parameter("@name").value()).setValue(name());
			//myDataBase.myRollbackSavePoint->execute();
		}
	}
}

std::string SQLiteDataBase::Transaction::name() const {
	std::string name = "tran";
	ZLStringUtil::appendNumber(name, myDepth);
	return name;
}

//----------- End Transaction subclass -----------------



SQLiteDataBase::SQLiteDataBase(const std::string &path) : DataBase( new SQLiteConnection(path) ), myTransactionDepth(0) {
	myBeginTransaction    = SQLiteFactory::createCommand("BEGIN IMMEDIATE TRANSACTION", connection());
	myCommitTransaction   = SQLiteFactory::createCommand("COMMIT TRANSACTION", connection());
	myRollbackTransaction = SQLiteFactory::createCommand("ROLLBACK TRANSACTION", connection());
	myMakeSavePoint       = SQLiteFactory::createCommand("SAVEPOINT @name", connection(), "@name", DBValue::DBTEXT);
	myCommitSavePoint     = SQLiteFactory::createCommand("RELEASE @name", connection(), "@name", DBValue::DBTEXT);
	myRollbackSavePoint   = SQLiteFactory::createCommand("ROLLBACK TO @name; RELEASE @name", connection(), "@name", DBValue::DBTEXT);
}

SQLiteDataBase::~SQLiteDataBase() {
	if (connection().isOpened()) {
		connection().close();
	}
}

bool SQLiteDataBase::executeAsTransaction(DBRunnable &runnable) {
	Transaction tran(*this);
	if (tran.start() && runnable.run()) {
		tran.setSuccessful();
		return true;
	}
	return false;
}



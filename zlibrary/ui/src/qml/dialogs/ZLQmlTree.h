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

#ifndef ZLQMLTREE_H
#define ZLQMLTREE_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QWeakPointer>
#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <ZLTreeDialog.h>

class ZLQmlTreeDialog : public QAbstractItemModel, public ZLTreeDialog {
	Q_OBJECT
public:
	enum TreeRoles {
		SubTitleRole = Qt::UserRole,
		ActivatableRole,
		PageRole
	};
	
	ZLQmlTreeDialog();
	~ZLQmlTreeDialog();
	
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;
	
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	//    virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
	
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
	void onProgressUpdated(ZLTreeNode *node);
	void onProgressFinished(ZLTreeNode *node, const std::string &error);
	
	Q_INVOKABLE void fetchChildren(const QModelIndex &index);
	Q_INVOKABLE QVariant progressData(const QModelIndex &index);
	Q_INVOKABLE bool activate(const QModelIndex &index);
	Q_INVOKABLE QStringList actions(const QModelIndex &index);
	bool isVisibleAction(const QModelIndex &index, int action);
	Q_INVOKABLE void run(const QModelIndex &index, int action);
	Q_INVOKABLE void finish();
	
	virtual void onNodeBeginInsert(ZLTreeNode *parent, size_t index);
	virtual void onNodeEndInsert();
	virtual void onNodeBeginRemove(ZLTreeNode *parent, size_t index);
	virtual void onNodeEndRemove();
	virtual void onNodeUpdated(ZLTreeNode *node);
	virtual void run();
	virtual void onCloseRequest();
	
Q_SIGNALS:
	void finished();
	void progressChanged();
	
private:
	QModelIndex createIndex(ZLTreeNode *node) const;
	ZLTreeNode *treeNode(const QModelIndex &index) const;
	
	QMap<ZLTreeNode*, shared_ptr<ZLExecutionData::Listener> > myListeners;
};

class ZLQmlDataModel : public QAbstractListModel {
	Q_OBJECT
	Q_PROPERTY(QObject* model READ model WRITE setModel NOTIFY modelChanged)
	Q_PROPERTY(QModelIndex rootIndex READ rootIndex WRITE setRootIndex NOTIFY rootIndexChanged)
public:
	ZLQmlDataModel();
	~ZLQmlDataModel();
	
	QObject *model() const;
	void setModel(QObject *model);
	QModelIndex rootIndex() const;
	void setRootIndex(const QModelIndex &index);
	
	Q_INVOKABLE QModelIndex modelIndex(int index) const;
	
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
protected Q_SLOTS:
	void onRowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
	void onRowsInserted(const QModelIndex &parent, int first, int last);
	void onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
	void onRowsRemoved(const QModelIndex &parent, int first, int last);
	void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
	
protected:
	void doSetModel(QAbstractItemModel *model);
	
Q_SIGNALS:
	void modelChanged(QObject *model);
	void rootIndexChanged(const QModelIndex &index);
	
private:
	QWeakPointer<QAbstractItemModel> myModel;
	QModelIndex myIndex;
};

#endif // ZLQMLTREE_H

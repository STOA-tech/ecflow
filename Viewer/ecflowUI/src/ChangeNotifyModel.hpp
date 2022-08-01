//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef CHANGENOTIFYMODEL_HPP_
#define CHANGENOTIFYMODEL_HPP_

#include <QSortFilterProxyModel>

#include <vector>

#include "VInfo.hpp"

class VNodeList;

class ChangeNotifyModel : public QAbstractItemModel
{
 Q_OBJECT
public:
   	explicit ChangeNotifyModel(QObject *parent=nullptr);
   	~ChangeNotifyModel() override;

   	int columnCount (const QModelIndex& parent = QModelIndex() ) const override;
   	int rowCount (const QModelIndex& parent = QModelIndex() ) const override;

   	QVariant data (const QModelIndex& , int role = Qt::DisplayRole ) const override;
   	QVariant headerData(int,Qt::Orientation,int role = Qt::DisplayRole ) const override;

   	QModelIndex index (int, int, const QModelIndex& parent = QModelIndex() ) const override;
   	QModelIndex parent (const QModelIndex & ) const override;

    void resetData(VNodeList *);
	bool hasData() const;
	VNodeList* data();
    VInfo_ptr nodeInfo(const QModelIndex&) const;

public Q_SLOTS:
	void slotBeginAppendRow();
	void slotEndAppendRow();
	void slotBeginRemoveRow(int);
	void slotEndRemoveRow(int);
	void slotBeginRemoveRows(int,int);
	void slotEndRemoveRows(int,int);
	void slotBeginReset();
	void slotEndReset();

protected:
	VNodeList* data_{nullptr};
};

#endif


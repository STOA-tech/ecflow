//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractItemModel>
#include <QDateTime>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>

#include <string>
#include <vector>

#include "LogData.hpp"

class LogModel : public QAbstractItemModel
{
    Q_OBJECT
public:
   	explicit LogModel(QObject *parent=nullptr);
   	~LogModel() override;

   	int columnCount (const QModelIndex& parent = QModelIndex() ) const override;
   	int rowCount (const QModelIndex& parent = QModelIndex() ) const override;

   	Qt::ItemFlags flags ( const QModelIndex & index) const override;
   	QVariant data (const QModelIndex& , int role = Qt::DisplayRole ) const override;
   	QVariant headerData(int,Qt::Orientation,int role = Qt::DisplayRole ) const override;

   	QModelIndex index (int, int, const QModelIndex& parent = QModelIndex() ) const override;
   	QModelIndex parent (const QModelIndex & ) const override;

    void loadFromFile(const std::string&,size_t startPos=0);
    void setData(const std::string&);
	void setData(const std::vector<std::string>&);
	void appendData(const std::vector<std::string>&);
	bool hasData() const;
    void clearData();
    QModelIndex lastIndex() const;
    QModelIndex highlightPeriodIndex() const;

    QString entryText(const QModelIndex&) const;
    QString fullText(const QModelIndex&) const;

    void beginLoadFromReader();
    void endLoadFromReader();
    LogData* logData() {return &data_;}

public Q_SLOTS:
    void setPeriod(qint64,qint64);
    void setHighlightPeriod(qint64,qint64,qint64 tolerance=0);
    void resetPeriod();

Q_SIGNALS:
    void rerender();
    void scrollToHighlightedPeriod();

protected:
    int realRow(size_t idx) const;

    //QList<LogModelLine> data_;
    LogData data_;
    bool filterPeriod_{false};
    size_t periodStart_{0};
    size_t periodEnd_{0};
    bool highlightPeriod_{false};
    size_t highlightStart_{0};
    size_t highlightEnd_{0};
};

class LogDelegate : public QStyledItemDelegate
{
public:
    explicit LogDelegate(QWidget *parent=nullptr);
    void paint(QPainter *painter,const QStyleOptionViewItem &option,
                   const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const override;
};



#endif

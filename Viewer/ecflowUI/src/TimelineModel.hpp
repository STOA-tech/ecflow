//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef TIMELINEMODEL_CPP
#define TIMELINEMODEL_CPP

#include <QtGlobal>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QDateTime>
#include <QSet>
#include "StringMatchMode.hpp"

#if QT_VERSION < QT_VERSION_CHECK(5,12, 0)
#include <QRegExp>
#endif
#include <QRegularExpression>

class TimelineData;

class TimelineModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum CustomItemRole {PathSortRole = Qt::UserRole+1, TimeSortRole = Qt::UserRole+2,
                        UnchangedRole = Qt::UserRole+3, MeanDurationRole = Qt::UserRole+4,
                        DurationStatRole = Qt::UserRole+5,QtSortRole = Qt::UserRole+6,
                        DurationUnchangedRole = Qt::UserRole+7, TreeSortRole = Qt::UserRole+8};

    enum ColumnType {PathColumn=0, TimelineColumn=1, SubmittedDurationColumn=2,
                     ActiveDurationColumn=3};

    explicit TimelineModel(QObject *parent=nullptr);
    ~TimelineModel() override;

    int columnCount (const QModelIndex& parent = QModelIndex() ) const override;
    int rowCount (const QModelIndex& parent = QModelIndex() ) const override;

    Qt::ItemFlags flags ( const QModelIndex & index) const override;
    QVariant data (const QModelIndex& , int role = Qt::DisplayRole ) const override;
    QVariant headerData(int,Qt::Orientation,int role = Qt::DisplayRole ) const override;

    QModelIndex index (int, int, const QModelIndex& parent = QModelIndex() ) const override;
    QModelIndex parent (const QModelIndex & ) const override;

    void resetData(TimelineData* data);

    TimelineData* data() const {return data_;}
    void clearData();
    bool hasData() const;
    void setPeriod(QDateTime t1,QDateTime t2);
    void setStartDate(QDateTime t);
    void setEndDate(QDateTime t);
    void setUseMeanDuration(bool b) {useMeanDuration_ = b;}

Q_SIGNALS:
    void periodChanged();

protected:
    TimelineData* data_;
    QDateTime startDate_;
    QDateTime endDate_;
    bool useMeanDuration_{false};
};

class TimelineSortModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    TimelineSortModel(TimelineModel*,QObject *parent=nullptr);
    ~TimelineSortModel() override;

    enum SortMode {PathSortMode, TimeSortMode, TreeSortMode, QtSortMode};
    enum ChangeFilterMode {TimelineChangeFilterMode, DurationChangeFilterMode,NoChangeFilterMode};

    //From QSortFilterProxyModel:
    //we set the source model in the constructor. So this function should not do anything.
    void setSourceModel(QAbstractItemModel*) override {}
    TimelineModel* tlModel() const {return tlModel_;}

    void selectionChanged(QModelIndexList lst);
    void setSortMode(SortMode);
    void setSortDirection(bool ascending);
    void setSkipSort(bool b) {skipSort_=b;}
    void setPathMatchMode(StringMatchMode);
    void setPathFilter(QString);
    void setTaskFilter(bool);
    void setChangeFilterMode(ChangeFilterMode m);
    void setRootNodeFilter(QString);
    void sortAgain();
    SortMode sortMode() const {return sortMode_;}

protected Q_SLOTS:
    void slotPeriodChanged();

Q_SIGNALS:
    void invalidateCalled();

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &/*sourceParent*/) const override;

    TimelineModel* tlModel_;
    bool skipSort_;
    SortMode sortMode_;
    bool ascending_;
    StringMatchMode pathMatchMode_;
    QString pathFilter_;
    bool taskFilter_;
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    QRegularExpression pathFilterRx_;
#else
    QRegExp pathFilterRx_;
#endif
    ChangeFilterMode changeFilterMode_;
    QString rootNodeFilter_;
};

#endif // TIMELINEMODEL_CPP


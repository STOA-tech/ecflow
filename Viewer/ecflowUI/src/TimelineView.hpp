//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef TIMELINEVIEW_HPP
#define TIMELINEVIEW_HPP

#include <QAction>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QTreeView>
#include <QPen>
#include <QDateTime>
#include <QStack>

#include "VInfo.hpp"
#include "VProperty.hpp"

class ActionHandler;
class PropertyMapper;
class TimelineModel;
class TimelineSortModel;
class VSettings;

class TimelineHeader;
class TimelineDelegate;
class TimelineInfoDialog;
class MainTimelineHeader;

class TimelineView : public QTreeView,public VPropertyObserver
{
Q_OBJECT

public:
    explicit TimelineView(TimelineSortModel* model,QWidget *parent=nullptr);
    ~TimelineView() override;

    enum ViewMode {TimelineMode,DurationMode};
    ViewMode viewMode() const {return viewMode_;}

    enum DurationViewMode {FirstDurationMode, MeanDurationMode};

    void dataCleared();
    void rerender();

    VInfo_ptr currentSelection();
    void setCurrentSelection(VInfo_ptr n);
    void setStartDate(QDateTime);
    void setEndDate(QDateTime);
    void setPeriod(QDateTime t1,QDateTime t2);
    void setZoomActions(QAction* zoomInAction,QAction* zoomOutAction);
    void setViewMode(ViewMode vm, bool force=false);
    void setDurationViewMode(DurationViewMode mode);

    void notifyChange(VProperty* p) override;

    void readSettings(VSettings*);
    void writeSettings(VSettings*);

protected Q_SLOTS:
    void slotDoubleClickItem(const QModelIndex&);
    void slotContextMenu(const QPoint &position);
    void slotViewCommand(VInfo_ptr,QString);
    void slotHeaderContextMenu(const QPoint &position);
    void slotSizeHintChangedGlobal();
    void slotRerender();
    void periodSelectedInHeader(QDateTime t1,QDateTime t2);
    void slotHzScrollbar(int,int);
    void adjustHeader();


Q_SIGNALS:
    void selectionChanged(VInfo_ptr);
    void infoPanelCommand(VInfo_ptr,QString);
    void dashboardCommand(VInfo_ptr,QString);
    void periodSelected(QDateTime,QDateTime);
    void periodBeingZoomed(QDateTime,QDateTime);
    void lookupRequested(QString);
    void copyPathRequested(QString);

protected:
    QModelIndexList selectedList();
    void handleContextMenu(QModelIndex indexClicked,QModelIndexList indexLst,QPoint globalPos,QPoint widgetPos,QWidget *widget);
    void adjustBackground(QColor col);
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;
    void setSortingEnabledNoExec(bool b);
    void showDetails(const QModelIndex& indexClicked);
    void lookup(const QModelIndex&);
    void copyPath(const QModelIndex&);
    void updateDetails();
    void closeDetails();
    void updateDurations();
    int computeMaxDuration(QString);

    TimelineSortModel* model_;
    ActionHandler* actionHandler_;
    MainTimelineHeader* header_;
    bool headerBeingAdjusted_;
    TimelineDelegate *delegate_;
    bool needItemsLayout_;
    PropertyMapper* prop_;
    bool setCurrentIsRunning_;
    ViewMode viewMode_;
    QDateTime startDate_;
    QDateTime endDate_;
    bool durationColumnWidthInitialised_;
    TimelineInfoDialog* infoDialog_{nullptr};
    DurationViewMode durationViewMode_{FirstDurationMode};
};


class TimelineDelegate : public QStyledItemDelegate, public VPropertyObserver
{
 Q_OBJECT

public:
    explicit TimelineDelegate(TimelineModel* model,QWidget *parent);
    ~TimelineDelegate() override;

    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const override;
    void paint(QPainter *painter,const QStyleOptionViewItem &option,
                   const QModelIndex& index) const override;

    void notifyChange(VProperty* p) override;

    void setStartDate(QDateTime);
    void setEndDate(QDateTime);
    void setPeriod(QDateTime t1,QDateTime t2);
    void setMaxDurations(int submittedDuration,int activeDuration);
    void setUseMeanDuration(bool b) {useMeanDuration_ = b;}

Q_SIGNALS:
    void sizeHintChangedGlobal();

protected:
    void updateSettings();
    void renderTimeline(QPainter *painter,const QStyleOptionViewItem& option,int) const;
    void renderSubmittedDuration(QPainter *painter,const QStyleOptionViewItem& option,const QModelIndex&) const;
    void renderActiveDuration(QPainter *painter,const QStyleOptionViewItem& option,const QModelIndex&) const;
    void renderDuration(QPainter *painter, int val, float meanVal, int maxVal, int num, QColor col, QRect rect,int maxTextW) const;
    void renderDuration(QPainter *painter, int val, int maxVal, QColor col, QRect rect,int maxTextW) const;
    void drawCell(QPainter *painter,QRect r,QColor fillCol,bool hasGrad,bool lighter) const;
    int timeToPos(QRect r,unsigned int time) const;
    int getDurationMaxTextWidth(int duration) const;

    TimelineModel* model_;
    PropertyMapper* prop_;
    QFont font_;
    QFontMetrics fm_;
    QPen borderPen_;
    int topPadding_;
    int bottomPadding_;
    QDateTime startDate_;
    QDateTime endDate_;
    int submittedMaxDuration_;
    int activeMaxDuration_;
    int submittedMaxTextWidth_;
    int activeMaxTextWidth_;
    bool useMeanDuration_{false};

};

#endif // TIMELINEVIEW_HPP



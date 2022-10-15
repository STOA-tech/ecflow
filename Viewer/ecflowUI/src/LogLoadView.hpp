//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef LOGLOADVIEW_HPP
#define LOGLOADVIEW_HPP

#include <map>
#include <string>
#include <vector>

#include <QtGlobal>
#include <QAbstractItemModel>
#include <QByteArray>
#include <QGraphicsItem>
#include <QMap>
#include <QScrollArea>
#include <QStringList>
#include <QWidget>
#include <QSortFilterProxyModel>
#include <QtCharts>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtCharts;
#endif

#include "LogLoadData.hpp"

class LogConsumer;
class LogLoadData;
class LogLoadDataItem;
class LogLoadSuiteModel;
class LogLoadRequestModel;
class LogModel;
class LogRequestView;
class LogRequestViewHandler;
class ServerLoadView;
class VComboSettings;
class QSortFilterProxyModel;
class QHBoxLayout;
class QVBoxLayout;
class QComboBox;
class QToolBox;
class QTextBrowser;
class QTableView;
class QTabWidget;
class QTreeView;
class QSplitter;
class QLabel;

struct LogLoadRequestModelDataItem
{
    LogLoadRequestModelDataItem(QString name, float percentage, bool checked,int rank) :
        name_(name), percentage_(percentage), checked_(checked), rank_(rank) {}

    QString name_;
    float percentage_;
    bool checked_;
    QColor col_;
    int rank_;
};

struct LogLoadSuiteModelDataItem
{
    LogLoadSuiteModelDataItem(QString suiteName, float percentage, bool checked,int rank) :
        suiteName_(suiteName), percentage_(percentage), checked_(checked), rank_(rank) {}

    QString suiteName_;
    float percentage_;
    bool checked_;
    QColor col_;
    int rank_;
};

class LogLoadRequestSortModel : public QSortFilterProxyModel
{
public:
    LogLoadRequestSortModel(QObject* parent=0);

protected:
    bool lessThan(const QModelIndex &left,const QModelIndex &right) const;
};

//Model to dislay/select the suites
class LogLoadRequestModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit LogLoadRequestModel(QString dataName,QObject *parent=0);
    ~LogLoadRequestModel();

    int columnCount (const QModelIndex& parent = QModelIndex() ) const;
    int rowCount (const QModelIndex& parent = QModelIndex() ) const;

    Qt::ItemFlags flags ( const QModelIndex & index) const;
    QVariant data (const QModelIndex& , int role = Qt::DisplayRole ) const;
    bool setData(const QModelIndex& idx, const QVariant & value, int role );
    QVariant headerData(int,Qt::Orientation,int role = Qt::DisplayRole ) const;

    QModelIndex index (int, int, const QModelIndex& parent = QModelIndex() ) const;
    QModelIndex parent (const QModelIndex & ) const;

    void setData(const std::vector<LogLoadDataItem>& data,QList<bool> checkedLst);
    void setData(const std::vector<LogRequestItem>& data,QList<bool> checkedLst);
    void adjustStats(const std::vector<LogLoadDataItem>& data);
    void adjustStats(const std::vector<LogRequestItem>& data);

    bool hasData() const;
    void clearData();

    void setShowColour(bool);

Q_SIGNALS:
    void checkStateChanged(int,bool);

public Q_SLOTS:
    void updateItem(int,bool,QColor);
    void unselectAll();
    void selectAll();
    void selectFirstFourItems();
    void selectFirstItem();

protected:
    QString formatPrecentage(float perc) const;

    QString dataName_;
    QList<LogLoadRequestModelDataItem> data_;
    bool showColour_;
};

class ChartCallout : public QGraphicsItem
{
public:
    ChartCallout(QChart *parent);

    void setText(const QString &text);
    void setAnchor(QPointF point);
    QPointF anchor() const {return anchor_;}
    void updateGeometry();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

private:
    QString text_;
    QRectF textRect_;
    QRectF rect_;
    QPointF anchor_;
    QPointF bottomPos_;
    QFont font_;
    QChart *chart_;
};

class ChartView : public QChartView
{
    Q_OBJECT
public:
    ChartView(QChart *chart, QWidget *parent);

    void doZoom(QRectF);
    void doZoom(qint64,qint64);
    void adjustTimeAxis(qint64 periodInMs);
    qint64 widthToTimeRange(float wPix);
    void currentTimeRange(qint64& start,qint64& end);
    void setCallout(qreal);
    void adjustCallout();
    void removeCallout();

Q_SIGNALS:
    void chartZoomed(QRectF);
    void positionChanged(qreal);
    void positionClicked(qreal);
    void removeCalloutRequested();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    ChartCallout* callout_;
    bool leftButtonPressed_{false};
};

class LogRequestView;

class LogRequestViewHandler : public QObject
{
    Q_OBJECT

    friend class LogRequestView;
    friend class LogTotalRequestView;
    friend class LogCmdSuiteRequestViewiew;
    friend class LogSuiteCmdRequestViewiew;
    friend class LogUidCmdRequestView;
    friend class LogCmdUidRequestView;
    friend class LogStatRequestView;
    friend class LogStatUidCmdView;

public:
    LogRequestViewHandler(QWidget* parent);
    ~LogRequestViewHandler();

    QList<LogRequestView*> views() const {return views_;}
    QList<QWidget*> tabItems() const {return tabItems_;}

    LogLoadData* data() const {return data_;}
    void clear();
    void load(const std::string& logFile,size_t maxReadSize,const std::vector<std::string>& suites,LogConsumer*);
    void loadMultiLogFile(const std::string& logFile,const std::vector<std::string>& suites,
                          int logFileIndex, bool last,LogConsumer*);
    void loadPostProc();
    void setResolution(LogLoadData::TimeRes);
    QList<bool> suitePlotState() const {return suitePlotState_;}
    QList<bool> cmdPlotState() const {return cmdPlotState_;}
    QList<bool> uidPlotState() const {return uidPlotState_;}

    void writeSettings(VComboSettings* vs);
    void readSettings(VComboSettings* vs);

public Q_SLOTS:
    void showFullRange();
    void addRemoveSuite(int idx, bool st);
    void addRemoveCmd(int idx, bool st);
    void addRemoveUid(int idx, bool st);

protected Q_SLOTS:
    void slotZoomHappened(QRectF);

Q_SIGNALS:
    void scanDataChanged(QString);
    void suitePlotStateChanged(int,bool,QColor);
    void cmdPlotStateChanged(int,bool,QColor);
    void uidPlotStateChanged(int,bool,QColor);
    void timeRangeChanged(qint64,qint64);
    void timeRangeHighlighted(qint64,qint64,qint64);
    void timeRangeReset();

protected:
    void buildOtherTab(QWidget*);
    void buildTableTab(QWidget*);

    LogLoadData* data_;
    QList<LogRequestView*> views_;
    QList<QWidget*> tabItems_;
    QList<bool> suitePlotState_;
    QList<bool> cmdPlotState_;
    QList<bool> uidPlotState_;
    int lastScanIndex_;
};

class LogRequestViewControlItem
{
public:
    LogRequestViewControlItem() : model_(0), sortModel_(0), tree_(0) {}

    void adjustColumnWidth();
    bool isAnySet() const {return plotState_.contains(true);}

    LogLoadRequestModel* model_;
    QSortFilterProxyModel* sortModel_;
    QTreeView* tree_;
    QList<bool> plotState_;
};

class LogRequestView : public QScrollArea
{
    Q_OBJECT
public:
    explicit LogRequestView(LogRequestViewHandler* handler,QWidget* parent=0);
    ~LogRequestView();

    void clear();
    void load();
    void changeResolution();
    virtual void adjustZoom(QRectF r);

    void writeSettings(VComboSettings* vs);
    void readSettings(VComboSettings* vs);

Q_SIGNALS:
    void scanDataChanged(QString);
    void suitePlotStateChanged(int,bool,QColor);
    void cmdPlotStateChanged(int,bool,QColor);
    void uidPlotStateChanged(int,bool,QColor);
    void timeRangeChanged(qint64,qint64);
    void timeRangeHighlighted(qint64,qint64,qint64);
    void timeRangeReset();
    void zoomHappened(QRectF);

public Q_SLOTS:
    void showFullRange();
    virtual void addRemoveSuite(int /*idx*/, bool /*st*/) {}
    virtual void addRemoveCmd(int /*idx*/, bool /*st*/) {}
    virtual void addRemoveUid(int /*idx*/, bool /*st*/) {}

protected Q_SLOTS:
    void slotZoom(QRectF);
    void adjustZoom();
    void scanPositionChanged(qreal);
    void scanPositionClicked(qreal);
    void removeCallout();

protected:
    enum ControlType {SuiteType,CmdType,UidType};

    void buildControlCore(LogRequestViewControlItem* item,QString title,QString modelHeader,bool selectAll);
    void buildSuiteControl(LogRequestViewControlItem* item,QString title,QString modelHeader,bool selectAll);
    void buildCmdControl(LogRequestViewControlItem* item,QString title,QString modelHeader,bool selectAll);
    void buildUidControl(LogRequestViewControlItem* item,QString title,QString modelHeader,bool selectAll);

    QChart* addChartById(QString id);
    void removeChartById(QString id);
    QString chartId(ChartView* cv);
    void clearCharts();
    void clearViews();
    virtual void loadCore()=0;
    void loadSuites();

    virtual void addSuite(int)=0;
    virtual void removeSuite(int)=0;
    virtual void addCmd(int) {}
    virtual void removeCmd(int) {}
    virtual void adjustStats()=0;

    int seriesValue(QChart* chart,QString id,int idx);
    QColor seriesColour(QChart* chart,QString id);
    bool seriesPeriodIndex(qint64 startTime, qint64 endTime,size_t& startIdx,size_t& endIdx);
    bool seriesIndex(qint64 t,int startId,qint64 tolerance,int& idx);
    qint64 seriesTime(int idx);

    void adjustMaxVal();
    void build(ChartView* view,QLineSeries *series,QString title,int maxVal);

    void removeSeries(QChart* chart,QString id);

    void setScanText(QString);
    virtual void buildScanTable(QString& txt,int idx)=0;
    void buildScanRow(QString &txt,QString name,size_t val,QColor col) const;
    void buildScanRow(QString &txt,QString name,size_t tot,size_t ch,size_t us,QColor col) const;
    void buildEmptyScanRow(QString &txt,QString name,QColor lineCol) const;
    void buildEmptyScanRowSingleVal(QString &txt,QString name,QColor lineCol) const;

    void initSplitter();

    LogRequestViewHandler* handler_;
    LogLoadData* data_;
    QList<ChartView*> views_;
    QMap<QString,ChartView*> viewIds_;
    QSplitter* splitter_;
    QHBoxLayout* mainLayout_;
    QVBoxLayout* viewLayout_;
    QVBoxLayout* sideLayout_;
    QTabWidget *controlTab_;
    QLabel* scanLabel_;

    LogRequestViewControlItem suiteCtl_;
    LogRequestViewControlItem cmdCtl_;
    LogRequestViewControlItem uidCtl_;

    QMap<ControlType,LogRequestViewControlItem> control_;

    int maxVal_;
    int lastScanIndex_;

    bool splitterInited_{false};
    QByteArray splitterSavedState_;
};

class LogTotalRequestView : public LogRequestView
{
    Q_OBJECT
public:
    explicit LogTotalRequestView(LogRequestViewHandler* handler,QWidget* parent=0);
    ~LogTotalRequestView() = default;

public Q_SLOTS:
    void addRemoveSuite(int idx, bool st) override;

protected:
    enum ChartType {TotalChartType=0,ChildChartType=1,UserChartType=2};

    void loadCore() override;
    void addSuite(int) override;
    void removeSuite(int) override {}
    void adjustStats() override;

    QString suiteSeriesId(int idx) const;
    QChart* getChart(ChartType);
    ChartView* getView(ChartType);
    void buildScanTable(QString& txt,int idx) override;

    LogLoadRequestModel* suiteModel_{nullptr};
    LogLoadRequestSortModel* suiteSortModel_{nullptr};
    QTreeView* suiteTree_{nullptr};
};

class LogCmdSuiteRequestView : public  LogRequestView
{
    Q_OBJECT
public:
    explicit LogCmdSuiteRequestView(LogRequestViewHandler* handler,QWidget* parent=0);
    ~LogCmdSuiteRequestView() {}

public Q_SLOTS:
    void addRemoveSuite(int idx, bool st);
    void addRemoveCmd(int idx, bool st);

protected:
    void loadCore();

    void addTotal();
    void addSuite(int);
    void removeSuite(int) {}
    void addCmd(int);
    void removeCmd(int);
    void adjustStats() {}

    QString cmdSeriesId(int) const;
    QColor cmdSeriesColour(QChart* chart,size_t idx);
    void buildScanTable(QString& txt,int idx);
};

class LogSuiteCmdRequestView : public  LogRequestView
{
    Q_OBJECT
public:
    explicit LogSuiteCmdRequestView(LogRequestViewHandler* handler,QWidget* parent=0);
    ~LogSuiteCmdRequestView() {}

public Q_SLOTS:
    void addRemoveSuite(int idx, bool st);
    void addRemoveCmd(int idx, bool st);

protected:
    void loadCore();

    void addTotal();
    void addSuite(int);
    void removeSuite(int);
    void addCmd(int);
    void adjustStats() {}

    QString cmdChartId(int idx) const;
    QString suiteSeriesId(int childIdx) const;
    QColor suiteSeriesColour(QChart* chart,size_t idx);

    void buildScanTable(QString& txt,int idx);
};

class LogUidCmdRequestView : public  LogRequestView
{
    Q_OBJECT
public:
    explicit LogUidCmdRequestView(LogRequestViewHandler* handler,QWidget* parent=0);
    ~LogUidCmdRequestView() {}

public Q_SLOTS:
    void addRemoveUid(int idx, bool st);
    void addRemoveCmd(int idx, bool st);

protected:
    void loadCore();

    void addTotal();
    void addSuite(int) {}
    void removeSuite(int) {}
    void addUid(int);
    void removeUid(int);
    void addCmd(int);
    void removeCmd(int) {}

    void adjustStats() {}

    QString cmdChartId(int idx) const;
    QString uidSeriesId(int uidIdx) const;
    QColor uidSeriesColour(QChart*,int uidIdx);
    void buildScanTable(QString& txt,int idx);
};

class LogCmdUidRequestView : public  LogRequestView
{
    Q_OBJECT
public:
    explicit LogCmdUidRequestView(LogRequestViewHandler* handler,QWidget* parent=0);
    ~LogCmdUidRequestView() {}

public Q_SLOTS:
    void addRemoveUid(int idx, bool st);
    void addRemoveCmd(int idx, bool st);

protected:
    void loadCore();

    void addTotal();
    void addSuite(int) {}
    void removeSuite(int) {}
    void addUid(int);
    void removeUid(int);
    void addCmd(int);
    void removeCmd(int);
    void adjustStats() {}

    QString uidChartId(int idx) const;
    QString cmdSeriesId(int) const;
    QColor cmdSeriesColour(QChart*,int);
    void buildScanTable(QString& txt,int idx);
};


class LogStatRequestModelData;
class LogStatRequestModel;

class LogStatRequestView : public  LogRequestView
{
    Q_OBJECT
public:
    explicit LogStatRequestView(LogRequestViewHandler* handler,QWidget* parent=0);
    ~LogStatRequestView() {}

    void adjustZoom(QRectF r);

protected:
    void addSuite(int) {}
    void removeSuite(int) {}
    void buildScanTable(QString& /*txt*/,int /*idx*/) {}

    LogStatRequestModel* statModel_;
    QSortFilterProxyModel *statSortModel_;
    QTableView* statTable_;
};

class LogStatCmdUidView : public  LogStatRequestView
{
    Q_OBJECT
public:
    explicit LogStatCmdUidView(LogRequestViewHandler* handler,QWidget* parent=0);
    ~LogStatCmdUidView() {}

public Q_SLOTS:
    void addRemoveUid(int idx, bool st);

protected:
    void adjustStats();
    void loadCore();
};

class LogStatUidCmdView : public  LogStatRequestView
{
    Q_OBJECT
public:
    explicit LogStatUidCmdView(LogRequestViewHandler* handler,QWidget* parent=0);
    ~LogStatUidCmdView() {}

public Q_SLOTS:
    void addRemoveCmd(int idx, bool st);

protected:
    void adjustStats();
    void loadCore();
};

class LogStatCmdSuiteView : public  LogStatRequestView
{
    Q_OBJECT
public:
    explicit LogStatCmdSuiteView(LogRequestViewHandler* handler,QWidget* parent=0);
    ~LogStatCmdSuiteView() {}

public Q_SLOTS:
    void addRemoveSuite(int idx, bool st);

protected:
    void adjustStats();
    void loadCore();
};


class LogStatSuiteCmdView : public  LogStatRequestView
{
    Q_OBJECT
public:
    explicit LogStatSuiteCmdView(LogRequestViewHandler* handler,QWidget* parent=0);
    ~LogStatSuiteCmdView() {}

public Q_SLOTS:
    void addRemoveCmd(int idx, bool st);

protected:
    void adjustStats();
    void loadCore();
};

class LogStatRequestModelData
{
public:
    int rowNum() const {return rowLabels_.count();}
    int colNum() const {return colLabels_.count();}
    void clear() {dataIndex_.clear(); rowLabels_.clear(); colLabels_.clear(); vals_.clear();}

    QVector<int> dataIndex_;
    QStringList rowLabels_;
    QStringList colLabels_;
    QVector<QVector<float> > vals_;
};

class LogStatRequestModel : public QAbstractItemModel
{
public:
    explicit LogStatRequestModel(QObject *parent=0);
    ~LogStatRequestModel();

    int columnCount (const QModelIndex& parent = QModelIndex() ) const;
    int rowCount (const QModelIndex& parent = QModelIndex() ) const;

    QVariant data (const QModelIndex& , int role = Qt::DisplayRole ) const;
    QVariant headerData(int,Qt::Orientation,int role = Qt::DisplayRole ) const;

    QModelIndex index (int, int, const QModelIndex& parent = QModelIndex() ) const;
    QModelIndex parent (const QModelIndex & ) const;

    void setDataUidCmd(const LogLoadDataItem& total,const std::vector<LogLoadDataItem>& data);
    void setDataCmdUid(const LogLoadDataItem& total,const std::vector<LogLoadDataItem>& data);
    void setDataCmdSuite(const LogLoadDataItem& total,const std::vector<LogLoadDataItem>& data);
    void setDataSuiteCmd(const LogLoadDataItem& total,const std::vector<LogLoadDataItem>& data);
    void resetData(const std::vector<LogRequestItem>& data);

    bool hasData() const;
    void clearData();
    int dataIndex(int i) const {return(i >=0 && i < data_.dataIndex_.count())?(data_.dataIndex_[i]):0;}


protected:
    LogStatRequestModelData data_;
    enum ColumnOrder {NameOrder, ValueOrder};
    ColumnOrder columnOrder_;
};

#endif // LOGLOADVIEW_HPP

//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef LOGLOADDATA_HPP
#define LOGLOADDATA_HPP

#include <map>
#include <string>
#include <vector>

#include <QAbstractItemModel>
#include <QGraphicsItem>
#include <QMap>
#include <QStringList>
#include <QWidget>
#include <QtCharts>
#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtCharts;
#endif

class LogLoadData;
class LogLoadDataItem;
class LogConsumer;

struct LogLoadStatItem
{
    LogLoadStatItem() : sumTotal_(0), maxTotal_(0), rank_(0), percentage_(0.) {}

    void clear() { sumTotal_ = 0, maxTotal_ = 0, rank_ = 0, percentage_ = 0.; }
    float percentage() const { return percentage_; }
    size_t sumTotal() const { return sumTotal_; }
    size_t maxTotal() const { return maxTotal_; }
    int rank() const { return rank_; }

    size_t sumTotal_;  // sum of all
    size_t maxTotal_;  // the maximum value
    int rank_;         // the rank of this item within other items with regards to sumTotal_
    float percentage_; // 0-100, the percentage of sumTotal_ with respect to the
                       // sum of sumTotal_ of all the items
};

// Data for one type of request
class LogRequestItem {
public:
    enum Type { ChildCmd, UserCmd, OtherCmd };

    LogRequestItem() : global_(false), type_(OtherCmd), counter_(0) {}
    LogRequestItem(const std::string& name, Type type, const std::string& pattern, bool global = false)
        : global_(global),
          name_(name),
          type_(type),
          pattern_(pattern),
          counter_(0) {}

    const LogLoadStatItem& periodStat() const { return periodStat_; }
    void computeStat(size_t startIndex, size_t endIndex);
    void add(size_t index, size_t val);

    // Type type_;
    bool global_;
    std::string name_;
    Type type_;
    std::string pattern_; // used to identify the request in the log file
    std::vector<size_t> index_;
    std::vector<int> req_;
    LogLoadStatItem periodStat_;
    LogLoadStatItem fullStat_;
    size_t counter_;
};

// Helper structure for data collection
struct LogReqCounter
{
    LogReqCounter(const std::string& name);

    void clear();
    void add(bool childCmd, const std::string& line);

    std::string name_; // used for suites
    size_t childReq_;
    size_t userReq_;
    std::vector<LogRequestItem> subReq_;
};

// Data structure containing load/request statistics for
// an individual suite or for all the suites
class LogLoadDataItem {
public:
    LogLoadDataItem(const std::string& name);
    LogLoadDataItem();

    void clear();
    void init(size_t num);
    size_t size() const { return childReq_.size(); }

    const LogLoadStatItem& periodStat() const { return periodStat_; }
    float percentage() const { return periodStat_.percentage_; }
    void setPercentage(float v) { periodStat_.percentage_ = v; }
    size_t sumTotal() const { return periodStat_.sumTotal_; }
    size_t maxTotal() const { return periodStat_.maxTotal_; }
    size_t subReqMax() const { return subReqMax_; }
    int rank() const { return periodStat_.rank_; }
    void setRank(int v) { periodStat_.rank_ = v; }

    const std::vector<int>& childReq() const { return childReq_; }
    const std::vector<int>& userReq() const { return userReq_; }
    const std::string& name() const { return name_; }
    void valuesAt(size_t idx, size_t& total, size_t& child, size_t& user) const;

    const std::vector<LogRequestItem>& subReq() const { return subReq_; }

    void add(size_t index, const LogReqCounter&);

    static void buildSubReq(std::vector<LogRequestItem>& subReq);

    void saveFullStat();
    void useFullStat();
    void computeStat(size_t startIndex, size_t endIndex);

protected:
    void buildSubReq();
    void computeSubReqStat(std::vector<LogRequestItem>& childSubReq, size_t startIndex, size_t endIndex);
    void computeSubReqMax();

    std::vector<int> childReq_; // per seconds
    std::vector<int> userReq_;  // per seconds
    std::vector<LogRequestItem> subReq_;
    LogLoadStatItem fullStat_;
    LogLoadStatItem periodStat_;
    size_t subReqMax_{0};

    std::string name_; // the name of the item (only makes sense for suites)
};

class LogLoadDataStats {};

struct LogDataParseHelper
{
    // A collector total counts
    LogReqCounter total{"total"};

    // A collector for suite related data
    std::vector<LogReqCounter> suite_vec;

    // A collector for uid related data
    std::vector<LogReqCounter> uid_vec;

    std::vector<std::string> new_time_stamp;
    std::vector<std::string> old_time_stamp;

    std::map<std::string, size_t> uidCnt;
};

// The top level class for load/request statistics
class LogLoadData : public QObject {
    Q_OBJECT
public:
    enum TimeRes { SecondResolution, MinuteResolution, HourResolution };
    enum LoadStatus { LoadNotTried, LoadFailed, LoadDone };

    LogLoadData(QObject* parent = nullptr) : QObject(parent) {}

    void
    loadLogFile(const std::string& logFile, size_t maxReadSize, const std::vector<std::string>& suites, LogConsumer*);
    void loadMultiLogFile(const std::string& logFile,
                          const std::vector<std::string>& suites,
                          int logFileIndex,
                          bool last,
                          LogConsumer*);

    QDateTime loadedAt() const { return loadedAt_; }
    void clear();
    const LogLoadDataItem& dataItem() const { return total_; }
    QStringList suiteNames() const { return suites_; }
    const std::vector<LogLoadDataItem>& suites() const { return suiteData_; }
    const std::vector<LogLoadDataItem>& suiteData() const { return suiteData_; }
    const std::vector<LogLoadDataItem>& uidData() const { return uidData_; }
    const LogLoadDataItem& total() const { return total_; }
    TimeRes timeRes() const { return timeRes_; }
    void setTimeRes(TimeRes);
    // void loadLogFile(const std::string& logFile,int numOfRows=0);

    void getChildReq(QLineSeries& series);
    void getUserReq(QLineSeries& series);
    void getTotalReq(QLineSeries& series, int& maxVal);

    void getSuiteChildReq(size_t, QLineSeries& series);
    void getSuiteUserReq(size_t, QLineSeries& series);
    void getSuiteTotalReq(size_t, QLineSeries& series);
    void getSubReq(size_t subIdx, QLineSeries& series, int& maxVal);
    void getSuiteSubReq(size_t, size_t, QLineSeries& series);
    void getUidTotalReq(size_t, QLineSeries& series, int& maxVal);
    void getUidSubReq(size_t, size_t, QLineSeries& series);

    size_t size() const { return time_.size(); }
    QDateTime startTime() const;
    QDateTime endTime() const;
    const std::vector<qint64>& time() const { return time_; }
    qint64 period() const;
    bool indexOfTime(qint64 t, size_t&, size_t, qint64) const;

    void computeStat();
    void computeStat(size_t startIndex, size_t endIndex);

    QString subReqName(int idx) const;
    QString uidName(int idx) const;
    size_t subReqMax() const;
    int numOfRows() const { return numOfRows_; }
    std::streamoff startPos() const { return startPos_; }

    bool isFullRead() const { return fullRead_; }
    LoadStatus loadStatus() const { return loadStatus_; }
    void markAsLoadDone() { loadStatus_ = LoadDone; }

Q_SIGNALS:
    void loadProgress(size_t current, size_t total);

private:
    void loadLogFileCore(const std::string& logFile,
                         size_t maxReadSize,
                         const std::vector<std::string>& suites,
                         bool multi,
                         LogConsumer*);
    std::streamoff getStartPos(const std::string& logFile, int numOfRows);
    void getSeries(QLineSeries& series, const LogRequestItem& item, int& maxVal);
    void getSeries(QLineSeries& series, const std::vector<int>& vals, int& maxVal);
    void getSeries(QLineSeries& series, const std::vector<int>& vals1, const std::vector<int>& vals2, int& maxVal);
    void add(std::vector<std::string> time_stamp,
             const LogReqCounter& total,
             const std::vector<LogReqCounter>& suite_vec,
             const std::vector<LogReqCounter>& uid_vec);

    void computeInitialStat();
    void computeStat(std::vector<LogLoadDataItem>& items, size_t startIndex, size_t endIndex, bool fullPeriod);

    void extract_uid(const std::string& line, std::string& uid);
    //    bool extract_uid_data(const std::string& line,std::vector<LogReqCounter>& uid_vec);
    void extract_uid_data(const std::string& line, const std::string& uid, std::vector<LogReqCounter>& uid_vec);
    bool extract_suite_path(const std::string& line,
                            bool child_cmd,
                            std::vector<LogReqCounter>& suite_vec,
                            size_t& column_index);

    TimeRes timeRes_{SecondResolution};
    std::vector<qint64> time_; // times stored as msecs since the epoch

    LogLoadDataItem total_;                  // generic data item for all the suites
    std::vector<LogLoadDataItem> suiteData_; // suite-related data items
    std::vector<LogLoadDataItem> uidData_;
    bool fullStatComputed_{false};

    QStringList suites_;
    QDateTime loadedAt_;
    size_t maxReadSize_{0};
    int numOfRows_{0};
    bool fullRead_{false};
    LoadStatus loadStatus_{LoadNotTried};
    std::streamoff startPos_{0};
    LogDataParseHelper parseHelper_;
};

#endif // LOGLOADDATA_HPP

#ifndef SUITEMODEL_H
#define SUITEMODEL_H

#include <QAbstractItemModel>
#include <QColor>

#include "SuiteFilterObserver.hpp"

class ServerHandler;
class SuiteFilter;

class SuiteModel : public QAbstractItemModel, public SuiteFilterObserver {
    Q_OBJECT
public:
    explicit SuiteModel(QObject* parent = nullptr);
    ~SuiteModel() override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QVariant headerData(int, Qt::Orientation, int role = Qt::DisplayRole) const override;

    QModelIndex index(int, int, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex&) const override;

    void setData(SuiteFilter* filter, ServerHandler* server);
    void reloadData();
    void resetData();

    SuiteFilter* filter() const { return data_; }
    SuiteFilter* realFilter() const { return realData_; }
    void setEdited(bool);
    bool isEdited() const { return edited_; }

    void notifyChange(SuiteFilter*) override;
    void notifyDelete(SuiteFilter*) override;

Q_SIGNALS:
    void dataUpdated();

protected:
    bool hasData() const;
    void clearData();
    void updateData();

    ServerHandler* server_{nullptr};
    SuiteFilter* data_{nullptr};
    SuiteFilter* realData_{nullptr};
    QColor presentCol_;
    QColor notPresentCol_;
    bool edited_{false};
};

#endif

/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_CommandOutputWidget_HPP
#define ecflow_viewer_CommandOutputWidget_HPP

#include <QAbstractItemModel>
#include <QSettings>
#include <QWidget>

#include "CommandOutput.hpp"
#include "ui_CommandOutputWidget.h"

class ModelColumn;

class CommandOutputModel : public QAbstractItemModel {
public:
    explicit CommandOutputModel(QObject* parent = nullptr);
    ~CommandOutputModel() override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const override;
    QVariant headerData(int, Qt::Orientation, int role = Qt::DisplayRole) const override;

    QModelIndex index(int, int, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex&) const override;

    void dataIsAboutToChange();
    void dataChanged();
    bool updateData();
    bool hasData() const;
    CommandOutput_ptr indexToItem(const QModelIndex& idx) const;
    QModelIndex itemToStatusIndex(CommandOutput_ptr item) const;

protected:
    ModelColumn* columns_{nullptr};
};

class CommandOutputWidget : public QWidget, protected Ui::CommandOutputWidget {
    Q_OBJECT

public:
    explicit CommandOutputWidget(QWidget* parent = nullptr);
    ~CommandOutputWidget() override;

    void readSettings(QSettings&);
    void writeSettings(QSettings&);

protected Q_SLOTS:
    void slotItemSelected(const QModelIndex&, const QModelIndex&);
    void slotItemAddBegin();
    void slotItemAddEnd();
    void slotItemOutputAppend(CommandOutput_ptr, QString);
    void slotItemErrorAppend(CommandOutput_ptr, QString);
    void slotItemOutputReload(CommandOutput_ptr);
    void slotItemErrorReload(CommandOutput_ptr);
    void slotItemStatusChanged(CommandOutput_ptr);
    void on_searchTb__clicked();
    void on_gotoLineTb__clicked();
    void on_fontSizeUpTb__clicked();
    void on_fontSizeDownTb__clicked();

Q_SIGNALS:
    void editorFontSizeChanged();

protected:
    bool isCurrent(CommandOutput_ptr item);
    void loadItem(CommandOutput_ptr);
    void updateInfoLabel(CommandOutput_ptr);
    void removeSpacer();
    QString formatErrorText(QString txt);

    CommandOutputModel* model_;
    QColor errCol_;
};

#endif /* ecflow_viewer_CommandOutputWidget_HPP */

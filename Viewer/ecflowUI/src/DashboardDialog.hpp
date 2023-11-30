/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_DashboardDialog_HPP
#define ecflow_viewer_DashboardDialog_HPP

#include <QDialog>

#include "ui_DashboardDialog.h"

class DashboardWidget;

class DashboardDialog : public QDialog, protected Ui::DashboardDialog {
    Q_OBJECT

public:
    explicit DashboardDialog(QWidget* parent = nullptr);

    void add(DashboardWidget*);
    DashboardWidget* dashboardWidget() const { return dw_; }

public Q_SLOTS:
    void reject() override;
    void slotUpdateTitle(QString, QString);
    void slotOwnerDelete();

Q_SIGNALS:
    void aboutToClose();

protected:
    void closeEvent(QCloseEvent* event) override;
    void readSettings();
    void writeSettings();

    DashboardWidget* dw_{nullptr};
};

#endif /* ecflow_viewer_DashboardDialog_HPP */

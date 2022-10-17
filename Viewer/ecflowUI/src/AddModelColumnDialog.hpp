//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef ADDMODELCOLUMNDIALOG_HPP
#define ADDMODELCOLUMNDIALOG_HPP

#include <QDialog>
#include <QString>
#include <set>
#include <string>

class ModelColumn;

namespace Ui {
class AddModelColumnDialog;
}

class AddModelColumnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddModelColumnDialog(QWidget *parent = nullptr);
    ~AddModelColumnDialog() override;

    void init(ModelColumn* mc,const std::set<std::string>&,QString defaultText = QString(""));

public Q_SLOTS:
    void accept() override ;

protected:
    Ui::AddModelColumnDialog *ui;
    ModelColumn* modelColumn_{nullptr};
};

class ChangeModelColumnDialog : public AddModelColumnDialog
{
    Q_OBJECT

public:
    explicit ChangeModelColumnDialog(QWidget *parent = nullptr);

    void setColumn(QString);

public Q_SLOTS:
    void accept() override;

protected:
    QString columnName_;
};


#endif // ADDMODELCOLUMNDIALOG_HPP

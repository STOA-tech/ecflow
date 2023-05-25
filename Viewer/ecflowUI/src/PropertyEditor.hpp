//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef PROPERTYEDITOR_INC_
#define PROPERTYEDITOR_INC_

#include <QWidget>

#include "ui_PropertyEditor.h"

class QGridLayout;
class QTabWidget;

class PropertyLine;
class VProperty;

class PropertyEditor : public QWidget, protected Ui::PropertyEditor {
    Q_OBJECT

public:
    explicit PropertyEditor(QWidget* parent = nullptr);
    ~PropertyEditor() override;

    void edit(VProperty*, QPixmap pixmap);
    void edit(VProperty*, QString label);
    void empty();
    bool applyChange();
    VProperty* property() const { return group_; }
    int currentTopLevelTabIndex() const;
    void setCurrentTopLevelTabIndex(int idx);

Q_SIGNALS:
    void changed();

private:
    void clear();
    void build();
    void addRules();
    void addHelpers();

    void addItem(VProperty*, QVBoxLayout*, QWidget*);
    PropertyLine* addLine(VProperty* vProp, QGridLayout* grid, QWidget*);
    void addGroup(VProperty*, QVBoxLayout*, QWidget*);
    void addGrid(VProperty*, QVBoxLayout*, QWidget*);
    void addGridRow(VProperty* prop, QGridLayout* grid, QWidget*);
    void addNotification(VProperty* prop, QVBoxLayout*, QWidget*);
    void addTabs(VProperty*, QVBoxLayout*, QWidget*);
    void addTab(VProperty*, QTabWidget*);
    void addNote(VProperty* vProp, QVBoxLayout*, QWidget*);
    void addNote(VProperty* vProp, QGridLayout* layout, QWidget*);
    QString buildNoteText(VProperty* vProp) const;

    VProperty* group_{nullptr};
    QGridLayout* currentGrid_{nullptr};
    QList<PropertyLine*> lineItems_;
    QString serverName_;
    QWidget* holder_{nullptr};
    int lineLabelLen_{-1};
    QTabWidget* topLevelTabW_{nullptr};
};

#endif

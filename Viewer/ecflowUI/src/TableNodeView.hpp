/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_TableNodeView_HPP
#define ecflow_viewer_TableNodeView_HPP

#include <cassert>
#include <set>

#include <QHeaderView>
#include <QTreeView>

#include "NodeViewBase.hpp"
#include "VInfo.hpp"
#include "VProperty.hpp"

class QComboBox;

class ActionHandler;
class TableNodeModel;
class TableNodeSortModel;
class NodeFilterDef;
class PropertyMapper;
class TableNodeHeader;

class TableNodeView : public QTreeView, public NodeViewBase, public VPropertyObserver {
    Q_OBJECT
public:
    explicit TableNodeView(TableNodeSortModel* model, NodeFilterDef* filterDef, QWidget* parent = nullptr);
    ~TableNodeView() override;

    void reload() override {}
    void rerender() override;
    QWidget* realWidget() override;
    QObject* realObject() override;
    VInfo_ptr currentSelection() override;
    void setCurrentSelection(VInfo_ptr n) override;
    void selectFirstServer() override {}
    void setTableModel(TableNodeSortModel* model);

    void notifyChange(VProperty* p) override;

    void readSettings(VSettings*) override;
    void writeSettings(VSettings*) override;

public Q_SLOTS:
    void slotDoubleClickItem(const QModelIndex&);
    void slotContextMenu(const QPoint& position);
    void slotCommandShortcut();
    void slotViewCommand(VInfo_ptr, QString);
    void slotHeaderContextMenu(const QPoint& position);
    void slotSizeHintChangedGlobal();
    void slotRerender();
    void slotAddVariableColumn();
    void slotUpdateBegin();
    void slotUpdateEnd();
    void slotSelectionAutoScrollChanged(bool);

Q_SIGNALS:
    void selectionChanged(VInfo_ptr);
    void infoPanelCommand(VInfo_ptr, QString);
    void dashboardCommand(VInfo_ptr, QString);
    void headerButtonClicked(QString, QPoint);

protected:
    QModelIndexList selectedList();
    void handleContextMenu(QModelIndex indexClicked,
                           QModelIndexList indexLst,
                           QPoint globalPos,
                           QPoint widgetPos,
                           QWidget* widget);
    void adjustBackground(QColor col);
    void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;
    void setSortingEnabledNoExec(bool b);
    void collectVariableNames(std::set<std::string>& vars);
    void changeVariableColumn(QString varName);
    void setCurrentSelectionAfterUpdate(VInfo_ptr info);

    TableNodeSortModel* model_;
    ActionHandler* actionHandler_;
    TableNodeHeader* header_;
    bool needItemsLayout_;
    PropertyMapper* prop_;
    bool setCurrentIsRunning_;
    VInfo_ptr lastSelection_;
    bool setCurrentAfterUpdateIsRunning_;
    bool autoScrollToSelection_;

private:
    // we enforce the usage of setTableModel()
    void setModel(QAbstractItemModel*) override { assert(false); }
};

class TableNodeHeaderButton {
public:
    TableNodeHeaderButton(QString id) : id_(id) {}

    QString id() const { return id_; }
    void setRect(QRect r) { rect_ = r; }
    QRect rect() const { return rect_; }

    QString id_;
    QRect rect_;
};

class TableNodeHeader : public QHeaderView {
    Q_OBJECT

public:
    explicit TableNodeHeader(QWidget* parent = nullptr);

    QSize sizeHint() const override;
    void setModel(QAbstractItemModel* model) override;

public Q_SLOTS:
    void slotSectionResized(int i);

Q_SIGNALS:
    void customButtonClicked(QString, QPoint);

protected:
    void showEvent(QShowEvent* QSize) override;
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const override;
    void mousePressEvent(QMouseEvent* event) override;

    QPixmap customPix_;
    mutable QMap<int, TableNodeHeaderButton> customButton_;
};

#endif /* ecflow_viewer_TableNodeView_HPP */

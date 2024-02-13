/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_NodeQueryResultView_HPP
#define ecflow_viewer_NodeQueryResultView_HPP

#include <QTreeView>

#include "VInfo.hpp"

class ActionHandler;
class NodeQueryResultModel;
class NodeQueryViewDelegate;

class QSortFilterProxyModel;

class NodeQueryResultView : public QTreeView {
    Q_OBJECT

public:
    explicit NodeQueryResultView(QWidget* parent = nullptr);
    ~NodeQueryResultView() override;

    void reload();
    void rerender();
    VInfo_ptr currentSelection();
    void currentSelection(VInfo_ptr n);
    void selectFirstServer();
    void setSourceModel(NodeQueryResultModel* model);
    void enableContextMenu(bool enable);
    void getListOfSelectedNodes(std::vector<VInfo_ptr>& nodeList);

    // void readSettings(VSettings* vs) {};

public Q_SLOTS:
    void slotSelectItem(const QModelIndex&);
    void slotDoubleClickItem(const QModelIndex&);
    void slotContextMenu(const QPoint& position);
    void slotCommandShortcut();
    void slotViewCommand(std::vector<VInfo_ptr>, QString);
    void slotSetCurrent(VInfo_ptr);
    void slotRerender();
    void slotSizeHintChangedGlobal();
    void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;

Q_SIGNALS:
    void selectionChanged(VInfo_ptr);
    void selectionChanged();
    void infoPanelCommand(VInfo_ptr, QString);

protected:
    QModelIndexList selectedList();
    void handleContextMenu(QModelIndex indexClicked,
                           QModelIndexList indexLst,
                           QPoint globalPos,
                           QPoint widgetPos,
                           QWidget* widget);

    NodeQueryResultModel* model_{nullptr};
    QSortFilterProxyModel* sortModel_{nullptr};
    ActionHandler* actionHandler_;
    bool needItemsLayout_{false};
    NodeQueryViewDelegate* delegate_;
};

#endif /* ecflow_viewer_NodeQueryResultView_HPP */

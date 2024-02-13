/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_NodeViewBase_HPP
#define ecflow_viewer_NodeViewBase_HPP

#include "VInfo.hpp"
#include "Viewer.hpp"

class QWidget;
class QObject;

class TableNodeSortModel;
class NodeFilterDef;
class VSettings;
class QModelIndex;

class NodeViewBase {
public:
    explicit NodeViewBase(NodeFilterDef*);
    virtual ~NodeViewBase() = default;

    virtual void reload()                         = 0;
    virtual void rerender()                       = 0;
    virtual QWidget* realWidget()                 = 0;
    virtual QObject* realObject()                 = 0;
    virtual VInfo_ptr currentSelection()          = 0;
    virtual void selectFirstServer()              = 0;
    virtual void setCurrentSelection(VInfo_ptr n) = 0;

    virtual void readSettings(VSettings* vs)  = 0;
    virtual void writeSettings(VSettings* vs) = 0;

protected:
    NodeFilterDef* filterDef_;
};

#endif /* ecflow_viewer_NodeViewBase_HPP */

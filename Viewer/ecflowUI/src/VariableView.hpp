//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef VARIABLEVIEW_HPP_
#define VARIABLEVIEW_HPP_


#include <QBrush>
#include <QMap>
#include <QPen>
#include <QStyledItemDelegate>

#include "TreeView.hpp"

class VariableView;

class VariableDelegate : public QStyledItemDelegate
{
    friend class VariableView;

public:
    explicit VariableDelegate(QTreeView *parent);
    void paint(QPainter *painter,const QStyleOptionViewItem &option,
                   const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const override;

protected:
    QPen selectPen_;
    QBrush selectBrush_;
    QBrush selectBrushBlock_;
    QPen borderPen_;
    QPixmap lockPix_;
    int genVarPixId_;
    int shadowGenVarPixId_;
    QTreeView* view_;

};

class VariableView : public TreeView
{
public:
	explicit VariableView(QWidget *parent=nullptr);

protected:
    void drawBranches(QPainter* painter,const QRect& rect,const QModelIndex& index ) const override;

    VariableDelegate *delegate_;
};


#endif

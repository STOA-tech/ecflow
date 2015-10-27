//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef TreeNodeViewDelegate_HPP_
#define TreeNodeViewDelegate_HPP_

#include <QBrush>
#include <QLinearGradient>
#include <QMap>
#include <QPen>
#include <QStyledItemDelegate>

#include "NodeViewDelegate.hpp"
#include "VProperty.hpp"

#include <string>

class AnimationHandler;
class PropertyMapper;

class TreeNodeViewDelegate : public NodeViewDelegate
{
Q_OBJECT

public:
	explicit TreeNodeViewDelegate(QWidget *parent=0);
	~TreeNodeViewDelegate();

    void paint(QPainter *painter,const QStyleOptionViewItem &option,
                   const QModelIndex& index) const;

    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    void setIndentation(int o) {indentation_=o;}


Q_SIGNALS:
    void sizeHintChangedGlobal();

protected:
    void updateSettings();

	void renderServer(QPainter *painter,const QModelIndex& index,
			            const QStyleOptionViewItemV4& option,QString text) const;

	void renderNode(QPainter *painter,const QModelIndex& index,
            		const QStyleOptionViewItemV4& option,QString text) const;

	void renderNodeCell(QPainter *painter,QColor bg,QColor realBg,QColor fg,
                      QRect stateRect,QRect filleRect,QRect realRect,QRect textRect, QString text,
                      bool selected) const;

    enum NodeStyle {ClassicNodeStyle,BoxAndTextNodeStyle};
                    
    AnimationHandler* animation_;

    int nodeRectRad_;
	bool drawChildCount_;
    NodeStyle nodeStyle_;
    bool useNodeGrad_;
    int indentation_;

    QFont serverNumFont_;
	QFont suiteNumFont_;
	QFont serverInfoFont_;

	mutable QLinearGradient grad_;
	static int lighter_;

};

#endif




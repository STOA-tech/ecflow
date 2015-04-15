/***************************** LICENSE START ***********************************

 Copyright 2015 ECMWF and INPE. This software is distributed under the terms
 of the Apache License version 2.0. In applying this license, ECMWF does not
 waive the privileges and immunities granted to it by virtue of its status as
 an Intergovernmental Organization or submit itself to any jurisdiction.

 ***************************** LICENSE END *************************************/

#include "NodeWidget.hpp"
#include "NodeViewBase.hpp"

#include "AbstractNodeModel.hpp"
#include "NodeFilterModel.hpp"

#include <QWidget>

NodeWidget::NodeWidget(QWidget* parent) : DashboardWidget(parent),
   model_(0),
   filterModel_(0),
   view_(0),
   icons_(0),
   filter_(0),
   data_(0)
{
}

NodeWidget::~NodeWidget()
{

}

QWidget* NodeWidget::widget()
{
	return view_->realWidget();
}

VInfo_ptr NodeWidget::currentSelection()
{
	return view_->currentSelection();
}

void NodeWidget::currentSelection(VInfo_ptr info)
{
	view_->currentSelection(info);
}

void NodeWidget::reload()
{
	active(true);
	//model_->reload();
}

void NodeWidget::active(bool b)
{
	model_->active(b);
	view_->realWidget()->setEnabled(b);
}

bool NodeWidget::active() const
{
	return model_->active();
}
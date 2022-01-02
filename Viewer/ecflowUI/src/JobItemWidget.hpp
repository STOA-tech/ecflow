//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef JOBITEMWIDGET_HPP_
#define JOBITEMWIDGET_HPP_

#include "InfoPanelItem.hpp"
#include "CodeItemWidget.hpp"
#include "VInfo.hpp"

#include "ServerHandler.hpp"

class JobItemWidget : public CodeItemWidget, public InfoPanelItem
{
public:
	explicit JobItemWidget(QWidget *parent=nullptr);
	~JobItemWidget() override;

	void reload(VInfo_ptr) override;
	QWidget* realWidget() override;
    void clearContents() override;

	//From VInfoPresenter
    void infoReady(VReply*) override;
    void infoFailed(VReply*) override;
    void infoProgress(VReply*) override;

    void nodeChanged(const VNode*, const std::vector<ecf::Aspect::Type>&) override {}
    void defsChanged(const std::vector<ecf::Aspect::Type>&) override {}

protected:
    void updateState(const ChangeFlags&) override;
    void reloadRequested() override;
};

#endif


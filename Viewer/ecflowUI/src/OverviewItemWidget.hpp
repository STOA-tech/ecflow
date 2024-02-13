/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_OverviewItemWidget_HPP
#define ecflow_viewer_OverviewItemWidget_HPP

#include "CodeItemWidget.hpp"
#include "InfoPanelItem.hpp"

class OverviewItemWidget : public CodeItemWidget, public InfoPanelItem {
public:
    explicit OverviewItemWidget(QWidget* parent = nullptr);
    ~OverviewItemWidget() override;

    void reload(VInfo_ptr) override;
    QWidget* realWidget() override;
    void clearContents() override;

    // From VInfoPresenter
    void infoReady(VReply*) override;
    void infoFailed(VReply*) override;
    void infoProgress(VReply*) override;

    void nodeChanged(const VNode*, const std::vector<ecf::Aspect::Type>&) override;
    void defsChanged(const std::vector<ecf::Aspect::Type>&) override;
    void connectStateChanged() override;

protected:
    void reload();
    void updateState(const ChangeFlags&) override;
    void reloadRequested() override;

    int lastScrollPos_{0};
};

#endif /* ecflow_viewer_OverviewItemWidget_HPP */

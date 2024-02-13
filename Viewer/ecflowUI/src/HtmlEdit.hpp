/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_HtmlEdit_HPP
#define ecflow_viewer_HtmlEdit_HPP

#include <QTextBrowser>

#include "VProperty.hpp"

class HtmlEdit : public QTextBrowser, public VPropertyObserver {
public:
    explicit HtmlEdit(QWidget* parent = nullptr);
    ~HtmlEdit() override;

    void setFontProperty(VProperty* p);
    void updateFont();
    void notifyChange(VProperty* p) override;

protected:
    void wheelEvent(QWheelEvent* event) override;

private:
    void fontSizeChangedByZoom();

    VProperty* fontProp_{nullptr};
};

#endif /* ecflow_viewer_HtmlEdit_HPP */

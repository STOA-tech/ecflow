/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "RichTextSearchLine.hpp"

#include <cassert>

#include "RichTextSearchInterface.hpp"

RichTextSearchLine::RichTextSearchLine(QWidget* parent) : TextEditSearchLine(parent) {
    interface_ = new RichTextSearchInterface;
    TextEditSearchLine::setSearchInterface(interface_);
}

RichTextSearchLine::~RichTextSearchLine() {
    delete interface_;
}

void RichTextSearchLine::setEditor(QTextBrowser* e) {
    auto* pti = static_cast<RichTextSearchInterface*>(interface_);
    assert(pti);
    pti->setEditor(e);
}

/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_PlainTextSearchInterface_HPP
#define ecflow_viewer_PlainTextSearchInterface_HPP

#include "AbstractTextEditSearchInterface.hpp"

class QPlainTextEdit;

class PlainTextSearchInterface : public AbstractTextEditSearchInterface {
public:
    PlainTextSearchInterface();
    void setEditor(QPlainTextEdit* e) { editor_ = e; }

    bool findString(QString str,
                    bool highlightAll,
                    QTextDocument::FindFlags findFlags,
                    QTextCursor::MoveOperation move,
                    int iteration,
                    StringMatchMode::Mode matchMode) override;

    void automaticSearchForKeywords(bool) override;
    void refreshSearch() override;
    void clearHighlights() override;
    void disableHighlights() override;
    void enableHighlights() override {}
    bool highlightsNeedSearch() override { return true; }
    void gotoLastLine() override;

protected:
    QPlainTextEdit* editor_{nullptr};
};

#endif /* ecflow_viewer_PlainTextSearchInterface_HPP */

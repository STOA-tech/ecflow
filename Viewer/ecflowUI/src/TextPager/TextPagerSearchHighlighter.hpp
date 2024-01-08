//============================================================================
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef VIEWER_SRC_TEXTPAGER_TEXTPAGERSEARCHHIGHLIGHTER_HPP_
#define VIEWER_SRC_TEXTPAGER_TEXTPAGERSEARCHHIGHLIGHTER_HPP_

#include <QColor>

#include "TextPagerDocument.hpp"
#include "syntaxhighlighter.hpp"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    #include <QRegExp>
#else
    #include <QtCore5Compat/QRegExp>
#endif

class TextPagerSearchHighlighter : public SyntaxHighlighter {
public:
    explicit TextPagerSearchHighlighter(QObject* parent = nullptr);
    void highlightBlock(const QString& string) override;
    void reset(QString txt, TextPagerDocument::FindMode mode, bool apply);
    void reset(QRegExp rx, TextPagerDocument::FindMode mode, bool apply);
    void clear();
    enum Mode { NoMode, TextMode, RegexpMode };

protected:
    bool isWordCharacter(const QChar& ch) const;

    Mode mode_{NoMode};
    QRegExp rx_;
    QString text_;
    QTextCharFormat format_;
    bool caseSensitive_{false};
    bool wholeWords_{false};
    static QColor bgColour_;
};

#endif /* VIEWER_SRC_TEXTPAGER_TEXTPAGERSEARCHHIGHLIGHTER_HPP_ */

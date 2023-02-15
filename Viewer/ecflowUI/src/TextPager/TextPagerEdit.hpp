// Copyright 2010 Anders Bakken
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TEXTPAGEREDIT_HPP__
#define TEXTPAGEREDIT_HPP__

#include <QAbstractScrollArea>
#include <QtGui>

#include "TextCodecWrapper.hpp"
#include "TextPagerCursor.hpp"
#include "TextPagerDocument.hpp"
#include "TextPagerSection.hpp"
#include "VProperty.hpp"
#include "syntaxhighlighter.hpp"

class TextPagerLineNumberArea;
class TextEditPrivate;
class TextPagerSearchHighlighter;

class TextPagerEdit : public QAbstractScrollArea, public VPropertyObserver {
    friend class TextPagerLineNumberArea;

    Q_OBJECT
    Q_PROPERTY(int cursorWidth READ cursorWidth WRITE setCursorWidth)
    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly)
    Q_PROPERTY(bool cursorVisible READ cursorVisible WRITE setCursorVisible)
    Q_PROPERTY(QString selectedText READ selectedText)
    Q_PROPERTY(int maximumSizeCopy READ maximumSizeCopy WRITE setMaximumSizeCopy)
    Q_PROPERTY(bool lineBreaking READ lineBreaking WRITE setLineBreaking)

public:
    TextPagerEdit(QWidget* parent = nullptr);
    ~TextPagerEdit() override;

    TextPagerDocument* document() const;
    void setDocument(TextPagerDocument* doc);

    int cursorWidth() const;
    void setCursorWidth(int cc);

    struct ExtraSelection
    {
        TextPagerCursor cursor;
        QTextCharFormat format;
    };

    void setExtraSelections(const QList<ExtraSelection>& selections);
    QList<ExtraSelection> extraSelections() const;

    void setSyntaxHighlighter(SyntaxHighlighter* h);
    inline SyntaxHighlighter* syntaxHighlighter() const { return syntaxHighlighters().value(0); }

    QList<SyntaxHighlighter*> syntaxHighlighters() const;
    void addSyntaxHighlighter(SyntaxHighlighter* highlighter);
    void takeSyntaxHighlighter(SyntaxHighlighter* highlighter);
    void removeSyntaxHighlighter(SyntaxHighlighter* highlighter);
    void clearSyntaxHighlighters();

    bool load(const QString& fileName,
              TextPagerDocument::DeviceMode mode = TextPagerDocument::Sparse,
              TextCodecWrapper                   = {});

    void paintEvent(QPaintEvent* e) override;
    void scrollContentsBy(int dx, int dy) override;

    bool moveCursorPosition(TextPagerCursor::MoveOperation op,
                            TextPagerCursor::MoveMode = TextPagerCursor::MoveAnchor,
                            int n                     = 1);
    void setCursorPosition(int pos, TextPagerCursor::MoveMode mode = TextPagerCursor::MoveAnchor);

    int viewportPosition() const;
    int cursorPosition() const;

    int textPositionAt(const QPoint& pos) const;

    bool readOnly() const;
    void setReadOnly(bool rr);

    bool lineBreaking() const;
    void setLineBreaking(bool lb);

    int maximumSizeCopy() const;
    void setMaximumSizeCopy(int max);

    QRect cursorBlockRect(const TextPagerCursor& cursor) const;
    QRect cursorRect(const TextPagerCursor& cursor) const;

    int lineNumber(int position) const;
    int columnNumber(int position) const;
    int lineNumber(const TextPagerCursor& cursor) const;
    int columnNumber(const TextPagerCursor& cursor) const;

    bool cursorVisible() const;
    void setCursorVisible(bool cc);

    QString selectedText() const;
    bool hasSelection() const;

    void setText(const QString& text);
    QString read(int pos, int size) const;
    QChar readCharacter(int index) const;

    void insert(int pos, const QString& text);
    void remove(int from, int size);

    TextPagerCursor& textCursor();
    const TextPagerCursor& textCursor() const;
    void setTextCursor(const TextPagerCursor& textCursor);

    TextPagerCursor cursorForPosition(const QPoint& pos) const;

    TextPagerSection* sectionAt(const QPoint& pos) const;

    QList<TextPagerSection*> sections(int from = 0, int size = -1, TextPagerSection::TextSectionOptions opt = {}) const;
    inline TextPagerSection* sectionAt(int pos) const {
        return sections(pos, 1, TextPagerSection::IncludePartial).value(0);
    }
    TextPagerSection* insertTextSection(int pos,
                                        int size,
                                        const QTextCharFormat& format = QTextCharFormat(),
                                        const QVariant& data          = QVariant());

    void ensureCursorVisible(const TextPagerCursor& cursor, int linesMargin = 0);

    void setEnableSearchHighlighter(bool);
    void clearSearchHighlighter();
    void setSearchHighlighter(QString txt, TextPagerDocument::FindMode mode);
    void setSearchHighlighter(QRegExp rx, TextPagerDocument::FindMode mode);

    void gotoLine(int);
    void setFontProperty(VProperty* p);
    void notifyChange(VProperty* p) override;
    void zoomIn();
    void zoomOut();
    void toDocStart();
    void toDocEnd();
    void toLineStart();
    void toLineEnd();

    void updateLineNumberArea();
    void setShowLineNumbers(bool b);
    void setLineNumberArea(TextPagerLineNumberArea* a);

    enum ActionType { CopyAction, SelectAllAction };
    QAction* action(ActionType type) const;

public Q_SLOTS:
    void ensureCursorVisible();
    void copy(QClipboard::Mode mode = QClipboard::Clipboard);
    void selectAll();
    void clearSelection();

Q_SIGNALS:
    void copyAvailable(bool on);
    void textChanged();
    void selectionChanged();
    void cursorPositionChanged(int pos);
    void sectionClicked(TextPagerSection* section, const QPoint& pos);

protected:
    // virtual void paste(int position, QClipboard::Mode mode);
    void changeEvent(QEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    void updateFont();
    void fontSizeChangedByZoom();

    void lineNumberAreaPaintEvent(QPaintEvent* e);
    int lineNumberAreaWidth();

    TextEditPrivate* d;
    friend class TextLayoutCacheManager;
    friend class TextEditPrivate;
    friend class TextPagerCursor;

    TextPagerSearchHighlighter* searchHighlight_;
    bool useSearchHighlight_{false};

    bool showLineNum_{false};
    TextPagerLineNumberArea* lineNumArea_{nullptr};
    VProperty* fontProp_{nullptr};
};

class TextPagerLineNumberArea : public QWidget {
public:
    explicit TextPagerLineNumberArea(TextPagerEdit* editor);
    QSize sizeHint() const override { return {computeWidth(), 0}; }
    int rightMargin() const { return rightMargin_; }
    void updateWidth(int maxLineNum = -1);
    QColor bgColour() const { return bgCol_; }
    QColor fontColour() const { return fontCol_; }
    QColor separatorColour() const { return separatorCol_; }
    QColor currentColour() const { return currentCol_; }

protected:
    void paintEvent(QPaintEvent* event) override { textEditor_->lineNumberAreaPaintEvent(event); }

private:
    int computeWidth(int maxLineNum = -1) const;

    TextPagerEdit* textEditor_;
    mutable int digits_;
    int rightMargin_;
    QColor bgCol_;
    QColor fontCol_;
    QColor separatorCol_;
    QColor currentCol_;
};

#endif

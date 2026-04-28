#include "FluentLineEdit.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QKeyEvent>
#include <QInputMethodEvent>
#include <QTimer>
#include <QFontMetrics>
#include <QApplication>

FluentLineEdit::FluentLineEdit(const QString& text, QWidget* parent)
    : FluentWidget(parent)
    , m_text(text)
    , m_cursorPos(text.length())
{
    setupSize();
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::IBeamCursor);
    setAttribute(Qt::WA_InputMethodEnabled, true);

    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

QString FluentLineEdit::text() const { return m_text; }
void FluentLineEdit::setText(const QString& text) {
    if (m_text != text) {
        m_text = text;
        m_cursorPos = qMin(m_cursorPos, m_text.length());
        ensureCursorVisible();
        update();
        emit textChanged(m_text);
    }
}

QString FluentLineEdit::placeholder() const { return m_placeholder; }
void FluentLineEdit::setPlaceholder(const QString& placeholder) {
    if (m_placeholder != placeholder) {
        m_placeholder = placeholder;
        update();
        emit placeholderChanged();
    }
}

FluentSize FluentLineEdit::size() const { return m_size; }
void FluentLineEdit::setSize(FluentSize size) {
    if (m_size != size) {
        m_size = size;
        setupSize();
        update();
        emit sizeChanged();
    }
}

bool FluentLineEdit::passwordMode() const { return m_passwordMode; }
void FluentLineEdit::setPasswordMode(bool enabled) {
    if (m_passwordMode != enabled) {
        m_passwordMode = enabled;
        update();
        emit passwordModeChanged();
    }
}

bool FluentLineEdit::showError() const { return m_showError; }
void FluentLineEdit::setShowError(bool show) {
    if (m_showError != show) {
        m_showError = show;
        update();
        emit showErrorChanged();
    }
}

QString FluentLineEdit::errorMessage() const { return m_errorMessage; }
void FluentLineEdit::setErrorMessage(const QString& msg) {
    if (m_errorMessage != msg) {
        m_errorMessage = msg;
        update();
        emit errorMessageChanged();
    }
}

bool FluentLineEdit::clearButtonEnabled() const { return m_clearButtonEnabled; }
void FluentLineEdit::setClearButtonEnabled(bool enabled) {
    if (m_clearButtonEnabled != enabled) {
        m_clearButtonEnabled = enabled;
        update();
        emit clearButtonEnabledChanged();
    }
}

QSize FluentLineEdit::sizeHint() const {
    return QSize(200, height());
}

QSize FluentLineEdit::minimumSizeHint() const {
    return QSize(60, height());
}

void FluentLineEdit::paintFluent(QPainter* painter) {
    auto* t = theme();
    const QRect r = rect();
    const int radius = t->cornerRadius();

    // 1. 背景
    QPainterPath path;
    path.addRoundedRect(r, radius, radius);

    QColor bgColor = t->cardColor();
    if (!isEnabled()) {
        bgColor = t->surfaceColor();
    } else if (hasFocus()) {
        bgColor = t->backgroundColor();
    } else if (isHovered()) {
        bgColor = t->backgroundColor();
    }
    painter->setPen(Qt::NoPen);
    painter->setBrush(bgColor);
    painter->drawPath(path);

    // 2. 边框
    QPen borderPen;
    if (m_showError) {
        borderPen = QPen(t->errorColor(), t->borderWidth());
    } else if (hasFocus()) {
        borderPen = QPen(t->primaryColor(), t->focusBorderWidth());
    } else if (isHovered()) {
        borderPen = QPen(t->textColorSecondary(), t->borderWidth());
    } else {
        borderPen = QPen(t->borderColor(), t->borderWidth());
    }
    painter->setPen(borderPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);

    // 3. 底部强调线 (聚焦时)
    if (hasFocus() && !m_showError) {
        QColor accentColor = t->primaryColor();
        accentColor.setAlphaF(0.3);
        painter->setPen(Qt::NoPen);
        painter->setBrush(accentColor);
        QRect bottomLine = r.adjusted(radius, r.height() - 2, -radius, r.height());
        painter->drawRect(bottomLine);
    }

    // 4. 绘制文本或占位符
    painter->setClipRect(textRect());
    QFont font = t->bodyFont();
    painter->setFont(font);

    QFontMetrics fm(font);
    QString displayText = m_passwordMode ? QString(m_text.length(), QChar(0x2022)) : m_text;

    if (m_text.isEmpty() && !hasFocus()) {
        // 占位符
        painter->setPen(t->textColorDisabled());
        painter->drawText(textRect().adjusted(-m_scrollOffset, 0, 0, 0),
                          Qt::AlignVCenter | Qt::AlignLeft, m_placeholder);
    } else if (!displayText.isEmpty()) {
        // 选中区域背景
        if (m_selectionStart != m_selectionEnd) {
            int selStart = qMin(m_selectionStart, m_selectionEnd);
            int selEnd = qMax(m_selectionStart, m_selectionEnd);

            int x1 = fm.horizontalAdvance(displayText.left(selStart)) - m_scrollOffset + textOffset();
            int x2 = fm.horizontalAdvance(displayText.left(selEnd)) - m_scrollOffset + textOffset();
            x1 = qMax(textOffset(), x1);
            x2 = qMin(width() - RIGHT_PADDING, x2);

            QColor selColor = t->primaryColor();
            selColor.setAlphaF(0.2);
            painter->setPen(Qt::NoPen);
            painter->setBrush(selColor);
            painter->drawRect(QRect(x1, 4, x2 - x1, r.height() - 8));
        }

        // 文字
        QColor textColor = isEnabled() ? t->textColorPrimary() : t->textColorDisabled();
        painter->setPen(textColor);
        QRect textDrawRect = textRect().adjusted(-m_scrollOffset, 0, 0, 0);
        painter->drawText(textDrawRect, Qt::AlignVCenter | Qt::AlignLeft, displayText);
    }
    painter->setClipping(false);

    // 5. 光标
    if (hasFocus() && m_cursorBlink) {
        QString textBeforeCursor = displayText.left(m_cursorPos);
        int cursorX = fm.horizontalAdvance(textBeforeCursor) - m_scrollOffset + textOffset();

        painter->setPen(t->textColorPrimary());
        painter->drawLine(cursorX, 6, cursorX, r.height() - 6);
    }

    // 6. 清除按钮
    if (m_clearButtonEnabled && !m_text.isEmpty() && hasFocus() && isEnabled()) {
        QRect clearRect = clearButtonRect();
        painter->setPen(t->textColorSecondary());
        painter->setBrush(Qt::NoBrush);

        // 画圆形背景
        painter->setPen(Qt::NoPen);
        QColor clearBg = t->textColorSecondary();
        clearBg.setAlphaF(0.15);
        painter->setBrush(clearBg);
        painter->drawEllipse(clearRect);

        // 画 X
        QPen xPen(t->textColorSecondary(), 1.2);
        painter->setPen(xPen);
        int margin = 4;
        painter->drawLine(clearRect.topLeft() + QPoint(margin, margin),
                          clearRect.bottomRight() - QPoint(margin, margin));
        painter->drawLine(clearRect.topRight() + QPoint(-margin, margin),
                          clearRect.bottomLeft() + QPoint(margin, -margin));
    }

    // 7. 错误提示
    if (m_showError && !m_errorMessage.isEmpty()) {
        painter->setPen(t->errorColor());
        painter->setFont(t->captionFont());
        painter->drawText(QRect(LEFT_PADDING, r.height(), width() - RIGHT_PADDING, 20),
                          Qt::AlignLeft | Qt::AlignVCenter, m_errorMessage);
    }
}

void FluentLineEdit::focusInEvent(QFocusEvent* event) {
    FluentWidget::focusInEvent(event);
    // 启动光标闪烁
    m_cursorBlink = true;
    if (m_blinkTimerId == 0) {
        m_blinkTimerId = startTimer(500);
    }
    update();
}

void FluentLineEdit::focusOutEvent(QFocusEvent* event) {
    FluentWidget::focusOutEvent(event);
    // 停止光标闪烁
    if (m_blinkTimerId != 0) {
        killTimer(m_blinkTimerId);
        m_blinkTimerId = 0;
    }
    m_cursorBlink = false;
    m_selectionStart = m_selectionEnd = 0;
    emit editingFinished();
    update();
}

void FluentLineEdit::keyPressEvent(QKeyEvent* event) {
    if (!isEnabled()) return;

    QString text = event->text();

    if (event->key() == Qt::Key_Backspace) {
        if (m_selectionStart != m_selectionEnd) {
            // 删除选中文字
            int selStart = qMin(m_selectionStart, m_selectionEnd);
            int selEnd = qMax(m_selectionStart, m_selectionEnd);
            m_text.remove(selStart, selEnd - selStart);
            m_cursorPos = selStart;
            m_selectionStart = m_selectionEnd = 0;
        } else if (m_cursorPos > 0) {
            m_text.remove(m_cursorPos - 1, 1);
            m_cursorPos--;
        }
        ensureCursorVisible();
        m_cursorBlink = true;
        update();
        emit textChanged(m_text);
    } else if (event->key() == Qt::Key_Delete) {
        if (m_selectionStart != m_selectionEnd) {
            int selStart = qMin(m_selectionStart, m_selectionEnd);
            int selEnd = qMax(m_selectionStart, m_selectionEnd);
            m_text.remove(selStart, selEnd - selStart);
            m_cursorPos = selStart;
            m_selectionStart = m_selectionEnd = 0;
        } else if (m_cursorPos < m_text.length()) {
            m_text.remove(m_cursorPos, 1);
        }
        ensureCursorVisible();
        m_cursorBlink = true;
        update();
        emit textChanged(m_text);
    } else if (event->key() == Qt::Key_Left) {
        if (m_cursorPos > 0) {
            if (event->modifiers() & Qt::ShiftModifier) {
                if (m_selectionStart == m_selectionEnd) {
                    m_selectionStart = m_selectionEnd = m_cursorPos;
                }
                m_cursorPos--;
                m_selectionEnd = m_cursorPos;
            } else {
                m_selectionStart = m_selectionEnd = 0;
                m_cursorPos--;
            }
        }
        ensureCursorVisible();
        m_cursorBlink = true;
        update();
    } else if (event->key() == Qt::Key_Right) {
        if (m_cursorPos < m_text.length()) {
            if (event->modifiers() & Qt::ShiftModifier) {
                if (m_selectionStart == m_selectionEnd) {
                    m_selectionStart = m_selectionEnd = m_cursorPos;
                }
                m_cursorPos++;
                m_selectionEnd = m_cursorPos;
            } else {
                m_selectionStart = m_selectionEnd = 0;
                m_cursorPos++;
            }
        }
        ensureCursorVisible();
        m_cursorBlink = true;
        update();
    } else if (event->key() == Qt::Key_Home) {
        m_cursorPos = 0;
        m_selectionStart = m_selectionEnd = 0;
        ensureCursorVisible();
        m_cursorBlink = true;
        update();
    } else if (event->key() == Qt::Key_End) {
        m_cursorPos = m_text.length();
        m_selectionStart = m_selectionEnd = 0;
        ensureCursorVisible();
        m_cursorBlink = true;
        update();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit returnPressed();
    } else if (!text.isEmpty() && text[0].isPrint() && !(event->modifiers() & Qt::ControlModifier)) {
        // 输入字符
        if (m_selectionStart != m_selectionEnd) {
            int selStart = qMin(m_selectionStart, m_selectionEnd);
            int selEnd = qMax(m_selectionStart, m_selectionEnd);
            m_text.remove(selStart, selEnd - selStart);
            m_cursorPos = selStart;
            m_selectionStart = m_selectionEnd = 0;
        }
        m_text.insert(m_cursorPos, text);
        m_cursorPos += text.length();
        ensureCursorVisible();
        m_cursorBlink = true;
        update();
        emit textChanged(m_text);
    }
}

void FluentLineEdit::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // 检查清除按钮
        if (isOverClearButton(event->pos())) {
            m_text.clear();
            m_cursorPos = 0;
            m_scrollOffset = 0;
            m_selectionStart = m_selectionEnd = 0;
            update();
            emit textChanged(m_text);
            return;
        }

        // 点击定位光标
        QFontMetrics fm(theme()->bodyFont());
        QString displayText = m_passwordMode ? QString(m_text.length(), QChar(0x2022)) : m_text;

        int clickX = event->pos().x() - textOffset() + m_scrollOffset;
        m_cursorPos = 0;
        int accumWidth = 0;
        for (int i = 0; i < displayText.length(); i++) {
            int charWidth = fm.horizontalAdvance(displayText[i]);
            if (accumWidth + charWidth / 2 >= clickX) break;
            accumWidth += charWidth;
            m_cursorPos = i + 1;
        }
        m_cursorPos = qMin(m_cursorPos, m_text.length());

        m_selectionStart = m_selectionEnd = m_cursorPos;
        m_selecting = true;
        m_cursorBlink = true;
        update();
    }
    FluentWidget::mousePressEvent(event);
}

void FluentLineEdit::mouseMoveEvent(QMouseEvent* event) {
    if (m_selecting && (event->buttons() & Qt::LeftButton)) {
        QFontMetrics fm(theme()->bodyFont());
        QString displayText = m_passwordMode ? QString(m_text.length(), QChar(0x2022)) : m_text;

        int moveX = event->pos().x() - textOffset() + m_scrollOffset;
        int newPos = 0;
        int accumWidth = 0;
        for (int i = 0; i < displayText.length(); i++) {
            int charWidth = fm.horizontalAdvance(displayText[i]);
            if (accumWidth + charWidth / 2 >= moveX) break;
            accumWidth += charWidth;
            newPos = i + 1;
        }
        newPos = qMin(newPos, m_text.length());

        if (newPos != m_cursorPos) {
            m_cursorPos = newPos;
            m_selectionEnd = newPos;
            ensureCursorVisible();
            update();
        }
    }

    // 清除按钮悬停光标
    if (isOverClearButton(event->pos())) {
        setCursor(Qt::ArrowCursor);
    } else {
        setCursor(Qt::IBeamCursor);
    }
}

void FluentLineEdit::inputMethodEvent(QInputMethodEvent* event) {
    if (!isEnabled()) return;

    QString commitText = event->commitString();
    if (!commitText.isEmpty()) {
        if (m_selectionStart != m_selectionEnd) {
            int selStart = qMin(m_selectionStart, m_selectionEnd);
            int selEnd = qMax(m_selectionStart, m_selectionEnd);
            m_text.remove(selStart, selEnd - selStart);
            m_cursorPos = selStart;
            m_selectionStart = m_selectionEnd = 0;
        }
        m_text.insert(m_cursorPos, commitText);
        m_cursorPos += commitText.length();
        ensureCursorVisible();
        m_cursorBlink = true;
        update();
        emit textChanged(m_text);
    }
    event->accept();
}

void FluentLineEdit::timerEvent(QTimerEvent* event) {
    if (event->timerId() == m_blinkTimerId) {
        m_cursorBlink = !m_cursorBlink;
        update();
    }
}

void FluentLineEdit::setupSize() {
    int h = 0;
    switch (m_size) {
    case FluentSize::Small:  h = theme()->controlHeightSmall(); break;
    case FluentSize::Medium: h = theme()->controlHeight(); break;
    case FluentSize::Large:  h = theme()->controlHeightLarge(); break;
    }
    setFixedHeight(h);
    setMinimumWidth(60);
}

void FluentLineEdit::ensureCursorVisible() {
    QFontMetrics fm(theme()->bodyFont());
    QString displayText = m_passwordMode ? QString(m_text.length(), QChar(0x2022)) : m_text;
    int textBeforeCursor = fm.horizontalAdvance(displayText.left(m_cursorPos));

    int availableWidth = textRect().width();
    int cursorRelX = textBeforeCursor - m_scrollOffset;

    if (cursorRelX < 0) {
        m_scrollOffset = textBeforeCursor;
    } else if (cursorRelX > availableWidth) {
        m_scrollOffset = textBeforeCursor - availableWidth;
    }
}

int FluentLineEdit::textOffset() const {
    return LEFT_PADDING;
}

int FluentLineEdit::textWidth() const {
    int w = width() - LEFT_PADDING - RIGHT_PADDING;
    if (m_clearButtonEnabled && !m_text.isEmpty() && hasFocus()) {
        w -= CLEAR_BTN_SIZE + 4;
    }
    return qMax(w, 20);
}

QRect FluentLineEdit::textRect() const {
    return QRect(textOffset(), 0, textWidth(), height());
}

QRect FluentLineEdit::clearButtonRect() const {
    int x = width() - RIGHT_PADDING - CLEAR_BTN_SIZE;
    int y = (height() - CLEAR_BTN_SIZE) / 2;
    return QRect(x, y, CLEAR_BTN_SIZE, CLEAR_BTN_SIZE);
}

bool FluentLineEdit::isOverClearButton(const QPoint& pos) const {
    return m_clearButtonEnabled && !m_text.isEmpty() && hasFocus() && clearButtonRect().contains(pos);
}

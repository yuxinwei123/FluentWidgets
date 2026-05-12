#include "FluentSpinBox.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QInputMethodEvent>
#include <QFontMetrics>
#include <QtMath>

// ============================================================
// FluentSpinBox
// ============================================================

FluentSpinBox::FluentSpinBox(QWidget* parent)
    : FluentWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setFixedSize(sizeHint());

    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

FluentSpinBox::~FluentSpinBox() {
    if (m_blinkTimerId) killTimer(m_blinkTimerId);
    if (m_repeatTimerId) killTimer(m_repeatTimerId);
}

// ============================================================
// 属性
// ============================================================

int FluentSpinBox::intValue() const { return qRound(m_value); }
void FluentSpinBox::setIntValue(int val) { setDoubleValue(val); }

double FluentSpinBox::doubleValue() const { return m_value; }
void FluentSpinBox::setDoubleValue(double val) {
    val = boundValue(val);
    if (qFuzzyCompare(m_value, val)) return;
    m_value = val;
    m_editText.clear();
    update();
    emit valueChanged(val);
    emit valueChanged(intValue());
}

double FluentSpinBox::minimum() const { return m_minimum; }
void FluentSpinBox::setMinimum(double min) { m_minimum = min; if (m_value < min) setDoubleValue(min); emit minimumChanged(); }
double FluentSpinBox::maximum() const { return m_maximum; }
void FluentSpinBox::setMaximum(double max) { m_maximum = max; if (m_value > max) setDoubleValue(max); emit maximumChanged(); }
void FluentSpinBox::setRange(double min, double max) { setMinimum(min); setMaximum(max); }

double FluentSpinBox::singleStep() const { return m_singleStep; }
void FluentSpinBox::setSingleStep(double step) { m_singleStep = step; emit singleStepChanged(); }

int FluentSpinBox::decimals() const { return m_decimals; }
void FluentSpinBox::setDecimals(int n) { m_decimals = n; update(); emit decimalsChanged(); }

QString FluentSpinBox::prefix() const { return m_prefix; }
void FluentSpinBox::setPrefix(const QString& p) { m_prefix = p; update(); emit prefixChanged(); }
QString FluentSpinBox::suffix() const { return m_suffix; }
void FluentSpinBox::setSuffix(const QString& s) { m_suffix = s; update(); emit suffixChanged(); }

bool FluentSpinBox::readOnly() const { return m_readOnly; }
void FluentSpinBox::setReadOnly(bool ro) { m_readOnly = ro; update(); emit readOnlyChanged(); }

QSize FluentSpinBox::sizeHint() const { return QSize(130, HEIGHT); }
QSize FluentSpinBox::minimumSizeHint() const { return QSize(80, HEIGHT); }

// ============================================================
// 布局
// ============================================================

int FluentSpinBox::buttonWidth() const { return BTN_WIDTH; }

QRect FluentSpinBox::upButtonRect() const {
    int bw = buttonWidth();
    return QRect(width() - bw, 0, bw, HEIGHT / 2);
}

QRect FluentSpinBox::downButtonRect() const {
    int bw = buttonWidth();
    return QRect(width() - bw, HEIGHT / 2, bw, HEIGHT / 2);
}

QRect FluentSpinBox::textRect() const {
    int bw = buttonWidth();
    return QRect(LEFT_PADDING, 0, width() - bw - LEFT_PADDING - RIGHT_PADDING, HEIGHT);
}

// ============================================================
// 操作
// ============================================================

void FluentSpinBox::stepUp() { setDoubleValue(m_value + m_singleStep); }
void FluentSpinBox::stepDown() { setDoubleValue(m_value - m_singleStep); }
void FluentSpinBox::stepBy(int steps) { setDoubleValue(m_value + m_singleStep * steps); }

QString FluentSpinBox::formatValue(double val) const {
    QString s;
    if (m_decimals == 0) {
        s = QString::number(qRound(val));
    } else {
        s = QString::number(val, 'f', m_decimals);
    }
    if (!m_prefix.isEmpty()) s = m_prefix + s;
    if (!m_suffix.isEmpty()) s = s + m_suffix;
    return s;
}

double FluentSpinBox::boundValue(double val) const {
    return qBound(m_minimum, val, m_maximum);
}

void FluentSpinBox::commitText() {
    if (m_editText.isEmpty()) return;
    // 去掉前后缀
    QString numText = m_editText;
    if (!m_prefix.isEmpty() && numText.startsWith(m_prefix)) numText.remove(0, m_prefix.length());
    if (!m_suffix.isEmpty() && numText.endsWith(m_suffix)) numText.chop(m_suffix.length());
    bool ok = false;
    double val = numText.toDouble(&ok);
    if (ok) setDoubleValue(val);
    m_editText.clear();
    m_cursorPos = 0;
    m_selectionStart = 0;
    m_selectionEnd = 0;
}

// ============================================================
// 绘制
// ============================================================

void FluentSpinBox::paintFluent(QPainter* painter) {
    auto* t = theme();
    bool focused = hasFocus();

    // 背景
    QPainterPath bgPath;
    bgPath.addRoundedRect(rect(), BORDER_RADIUS, BORDER_RADIUS);
    painter->setPen(Qt::NoPen);
    painter->setBrush(t->cardColor());
    painter->drawPath(bgPath);

    // 边框
    QColor borderColor = focused ? t->primaryColor() :
                         (m_hoveredArea != HoverArea::None || underMouse() ? t->textColorSecondary() : t->dividerColor());
    painter->setPen(QPen(borderColor, focused ? 2 : 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(bgPath);

    // 绘制文本
    QString displayText = m_editText.isEmpty() ? formatValue(m_value) : m_editText;
    QRect tr = textRect();
    painter->setPen(t->textColorPrimary());
    painter->setFont(t->bodyFont());

    if (hasFocus() && !m_readOnly) {
        // 计算滚动偏移
        QFontMetrics fm(t->bodyFont());
        int textW = fm.horizontalAdvance(displayText);
        int visibleW = tr.width();
        int cursorX = fm.horizontalAdvance(displayText.left(m_cursorPos));
        if (cursorX - m_scrollOffset > visibleW) m_scrollOffset = cursorX - visibleW + 4;
        if (cursorX - m_scrollOffset < 0) m_scrollOffset = qMax(0, cursorX - 4);

        painter->save();
        painter->setClipRect(tr);

        // 选中高亮
        if (m_selectionStart != m_selectionEnd) {
            int selMin = qMin(m_selectionStart, m_selectionEnd);
            int selMax = qMax(m_selectionStart, m_selectionEnd);
            int selX1 = fm.horizontalAdvance(displayText.left(selMin)) - m_scrollOffset + tr.x();
            int selX2 = fm.horizontalAdvance(displayText.left(selMax)) - m_scrollOffset + tr.x();
            painter->setPen(Qt::NoPen);
            painter->setBrush(t->primaryColor());
            painter->setOpacity(0.3);
            painter->drawRect(QRect(selX1, tr.y() + 4, selX2 - selX1, tr.height() - 8));
            painter->setOpacity(1.0);
        }

        painter->drawText(tr.adjusted(-m_scrollOffset, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, displayText);

        // 光标
        if (m_cursorBlink) {
            int cx = fm.horizontalAdvance(displayText.left(m_cursorPos)) - m_scrollOffset + tr.x();
            painter->setPen(QPen(t->primaryColor(), 1.5));
            painter->drawLine(cx, tr.y() + 6, cx, tr.bottom() - 6);
        }
        painter->restore();
    } else {
        painter->drawText(tr, Qt::AlignVCenter | Qt::AlignLeft, displayText);
    }

    // 按钮分隔线
    int bw = buttonWidth();
    painter->setPen(QPen(t->dividerColor(), 1));
    painter->drawLine(width() - bw, 0, width() - bw, HEIGHT);

    // ---- 上按钮 ▲ ----
    QRect upR = upButtonRect();
    bool upHovered = (m_hoveredArea == HoverArea::Up);
    bool upPressed = (m_pressedArea == HoverArea::Up);
    if (upPressed) {
        painter->fillRect(upR, QColor(t->textColorPrimary()).lighter(180));
        QColor pc = t->primaryColor(); pc.setAlphaF(0.12); painter->fillRect(upR, pc);
    } else if (upHovered) {
        QColor hc = t->textColorPrimary(); hc.setAlphaF(0.06); painter->fillRect(upR, hc);
    }
    // 圆角裁剪: 只对右上角
    painter->setPen(t->textColorSecondary());
    painter->setFont(t->captionFont());
    painter->drawText(upR, Qt::AlignCenter, u8"▲");

    // ---- 下按钮 ▼ ----
    QRect downR = downButtonRect();
    bool downHovered = (m_hoveredArea == HoverArea::Down);
    bool downPressed = (m_pressedArea == HoverArea::Down);
    if (downPressed) {
        QColor pc = t->primaryColor(); pc.setAlphaF(0.12); painter->fillRect(downR, pc);
    } else if (downHovered) {
        QColor hc = t->textColorPrimary(); hc.setAlphaF(0.06); painter->fillRect(downR, hc);
    }
    painter->setPen(t->textColorSecondary());
    painter->drawText(downR, Qt::AlignCenter, u8"▼");
}

// ============================================================
// 事件
// ============================================================

void FluentSpinBox::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    if (upButtonRect().contains(event->pos())) {
        m_pressedArea = HoverArea::Up;
        stepUp();
        m_repeatStep = true;
        m_repeatTimerId = startTimer(400);
    } else if (downButtonRect().contains(event->pos())) {
        m_pressedArea = HoverArea::Down;
        stepDown();
        m_repeatStep = false;
        m_repeatTimerId = startTimer(400);
    } else if (textRect().contains(event->pos()) && !m_readOnly) {
        // 光标定位
        commitText();
        setFocus();
        QString displayText = formatValue(m_value);
        QFontMetrics fm(theme()->bodyFont());
        int clickX = event->pos().x() - textRect().x() + m_scrollOffset;
        m_cursorPos = 0;
        for (int i = 1; i <= displayText.length(); ++i) {
            if (fm.horizontalAdvance(displayText.left(i)) >= clickX) {
                m_cursorPos = i;
                break;
            }
        }
        m_selectionStart = m_cursorPos;
        m_selectionEnd = m_cursorPos;
        m_selecting = true;
        m_cursorBlink = true;
        if (m_blinkTimerId) killTimer(m_blinkTimerId);
        m_blinkTimerId = startTimer(500);
        update();
    }
    update();
}

void FluentSpinBox::mouseReleaseEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    m_pressedArea = HoverArea::None;
    m_selecting = false;
    if (m_repeatTimerId) { killTimer(m_repeatTimerId); m_repeatTimerId = 0; }
    update();
}

void FluentSpinBox::mouseMoveEvent(QMouseEvent* event) {
    HoverArea oldArea = m_hoveredArea;
    if (upButtonRect().contains(event->pos())) m_hoveredArea = HoverArea::Up;
    else if (downButtonRect().contains(event->pos())) m_hoveredArea = HoverArea::Down;
    else m_hoveredArea = HoverArea::None;

    // 拖拽选中文本
    if (m_selecting && !m_readOnly) {
        QString displayText = m_editText.isEmpty() ? formatValue(m_value) : m_editText;
        QFontMetrics fm(theme()->bodyFont());
        int clickX = event->pos().x() - textRect().x() + m_scrollOffset;
        int newPos = 0;
        for (int i = 1; i <= displayText.length(); ++i) {
            if (fm.horizontalAdvance(displayText.left(i)) >= clickX) { newPos = i; break; }
        }
        m_cursorPos = newPos;
        m_selectionEnd = newPos;
    }

    if (oldArea != m_hoveredArea) update();
}

void FluentSpinBox::leaveEvent(QEvent* event) {
    m_hoveredArea = HoverArea::None;
    m_pressedArea = HoverArea::None;
    update();
    FluentWidget::leaveEvent(event);
}

void FluentSpinBox::wheelEvent(QWheelEvent* event) {
    if (!hasFocus()) return;
    if (event->angleDelta().y() > 0) stepUp();
    else if (event->angleDelta().y() < 0) stepDown();
    event->accept();
}

void FluentSpinBox::keyPressEvent(QKeyEvent* event) {
    if (m_readOnly) { FluentWidget::keyPressEvent(event); return; }

    if (m_editText.isEmpty()) {
        m_editText = formatValue(m_value);
        m_cursorPos = m_editText.length();
        m_selectionStart = 0;
        m_selectionEnd = m_editText.length();
    }

    switch (event->key()) {
    case Qt::Key_Up:
        commitText();
        stepUp();
        break;
    case Qt::Key_Down:
        commitText();
        stepDown();
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        commitText();
        break;
    case Qt::Key_Backspace:
        if (m_cursorPos > 0) {
            if (m_selectionStart != m_selectionEnd) {
                int sMin = qMin(m_selectionStart, m_selectionEnd);
                int sMax = qMax(m_selectionStart, m_selectionEnd);
                m_editText.remove(sMin, sMax - sMin);
                m_cursorPos = sMin;
            } else {
                m_editText.remove(m_cursorPos - 1, 1);
                m_cursorPos--;
            }
            m_selectionStart = m_selectionEnd = m_cursorPos;
        }
        break;
    case Qt::Key_Delete:
        if (m_cursorPos < m_editText.length()) {
            if (m_selectionStart != m_selectionEnd) {
                int sMin = qMin(m_selectionStart, m_selectionEnd);
                int sMax = qMax(m_selectionStart, m_selectionEnd);
                m_editText.remove(sMin, sMax - sMin);
                m_cursorPos = sMin;
            } else {
                m_editText.remove(m_cursorPos, 1);
            }
            m_selectionStart = m_selectionEnd = m_cursorPos;
        }
        break;
    case Qt::Key_Left:
        if (m_cursorPos > 0) m_cursorPos--;
        m_selectionStart = m_selectionEnd = m_cursorPos;
        break;
    case Qt::Key_Right:
        if (m_cursorPos < m_editText.length()) m_cursorPos++;
        m_selectionStart = m_selectionEnd = m_cursorPos;
        break;
    case Qt::Key_Home:
        m_cursorPos = 0;
        m_selectionStart = m_selectionEnd = m_cursorPos;
        break;
    case Qt::Key_End:
        m_cursorPos = m_editText.length();
        m_selectionStart = m_selectionEnd = m_cursorPos;
        break;
    default:
        if (event->text().length() == 1) {
            QChar ch = event->text()[0];
            if (ch.isDigit() || ch == '.' || ch == '-' || ch == '+') {
                if (m_selectionStart != m_selectionEnd) {
                    int sMin = qMin(m_selectionStart, m_selectionEnd);
                    int sMax = qMax(m_selectionStart, m_selectionEnd);
                    m_editText.remove(sMin, sMax - sMin);
                    m_cursorPos = sMin;
                }
                m_editText.insert(m_cursorPos, ch);
                m_cursorPos++;
                m_selectionStart = m_selectionEnd = m_cursorPos;
            }
        }
    }

    m_cursorBlink = true;
    if (m_blinkTimerId) killTimer(m_blinkTimerId);
    m_blinkTimerId = startTimer(500);
    update();
}

void FluentSpinBox::focusInEvent(QFocusEvent* event) {
    if (!m_readOnly) {
        m_editText = formatValue(m_value);
        m_cursorPos = 0;
        m_selectionStart = 0;
        m_selectionEnd = m_editText.length();
        m_cursorBlink = true;
        if (m_blinkTimerId) killTimer(m_blinkTimerId);
        m_blinkTimerId = startTimer(500);
    }
    FluentWidget::focusInEvent(event);
}

void FluentSpinBox::focusOutEvent(QFocusEvent* event) {
    commitText();
    if (m_blinkTimerId) { killTimer(m_blinkTimerId); m_blinkTimerId = 0; }
    FluentWidget::focusOutEvent(event);
}

void FluentSpinBox::timerEvent(QTimerEvent* event) {
    if (event->timerId() == m_blinkTimerId) {
        m_cursorBlink = !m_cursorBlink;
        update();
    } else if (event->timerId() == m_repeatTimerId) {
        if (m_repeatStep) stepUp(); else stepDown();
        // 加速重复
        killTimer(m_repeatTimerId);
        m_repeatTimerId = startTimer(80);
    }
}

void FluentSpinBox::inputMethodEvent(QInputMethodEvent* event) {
    if (m_readOnly) return;
    QString commit = event->commitString();
    if (!commit.isEmpty()) {
        for (QChar ch : commit) {
            if (ch.isDigit() || ch == '.' || ch == '-' || ch == '+') {
                if (m_editText.isEmpty()) m_editText = formatValue(m_value);
                m_editText.insert(m_cursorPos, ch);
                m_cursorPos++;
                m_selectionStart = m_selectionEnd = m_cursorPos;
            }
        }
    }
    event->accept();
    update();
}

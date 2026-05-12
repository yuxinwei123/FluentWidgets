#include "FluentComboBox.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QApplication>
#include <QScreen>
#include <QIcon>
// ============================================================
// FluentComboBox
// ============================================================

FluentComboBox::FluentComboBox(QWidget* parent)
    : FluentWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setFixedSize(sizeHint());

    // 创建弹出列表
    m_popup = new QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint);
    m_popup->setAttribute(Qt::WA_TranslucentBackground);
    m_popup->installEventFilter(this);

    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

FluentComboBox::~FluentComboBox() {
    delete m_popup;
}

// ============================================================
// 属性
// ============================================================

int FluentComboBox::count() const { return m_items.size(); }

int FluentComboBox::currentIndex() const { return m_currentIndex; }

void FluentComboBox::setCurrentIndex(int index) {
    if (index < -1 || index >= m_items.size()) index = -1;
    if (m_currentIndex == index) return;
    m_currentIndex = index;
    m_editText.clear();
    update();
    emit currentIndexChanged(index);
    emit currentTextChanged(currentText());
}

QString FluentComboBox::currentText() const {
    if (m_currentIndex >= 0 && m_currentIndex < m_items.size())
        return m_items[m_currentIndex].text;
    return m_editText;
}

QString FluentComboBox::itemText(int index) const {
    if (index >= 0 && index < m_items.size()) return m_items[index].text;
    return QString();
}

QVariant FluentComboBox::itemData(int index) const {
    if (index >= 0 && index < m_items.size()) return m_items[index].userData;
    return QVariant();
}

QIcon FluentComboBox::itemIcon(int index) const {
    if (index >= 0 && index < m_items.size()) return m_items[index].icon;
    return QIcon();
}

bool FluentComboBox::editable() const { return m_editable; }
void FluentComboBox::setEditable(bool on) { m_editable = on; update(); emit editableChanged(); }

QString FluentComboBox::placeholder() const { return m_placeholder; }
void FluentComboBox::setPlaceholder(const QString& text) { m_placeholder = text; update(); }

int FluentComboBox::maxVisibleItems() const { return m_maxVisibleItems; }
void FluentComboBox::setMaxVisibleItems(int n) { m_maxVisibleItems = n; }

QSize FluentComboBox::sizeHint() const { return QSize(200, HEIGHT); }
QSize FluentComboBox::minimumSizeHint() const { return QSize(100, HEIGHT); }

// ============================================================
// 项操作
// ============================================================

void FluentComboBox::addItem(const QString& text, const QVariant& userData) {
    ComboItem item{text, userData, QIcon()};
    m_items.append(item);
    if (m_currentIndex == -1) setCurrentIndex(0);
    emit countChanged();
}

void FluentComboBox::addItem(const QIcon& icon, const QString& text, const QVariant& userData) {
    ComboItem item{text, userData, icon};
    m_items.append(item);
    if (m_currentIndex == -1) setCurrentIndex(0);
    emit countChanged();
}

void FluentComboBox::addItems(const QStringList& texts) {
    for (const auto& t : texts) addItem(t);
}

void FluentComboBox::insertItem(int index, const QString& text, const QVariant& userData) {
    if (index < 0) index = 0;
    if (index > m_items.size()) index = m_items.size();
    m_items.insert(index, ComboItem{text, userData, QIcon()});
    if (m_currentIndex >= index) m_currentIndex++;
    emit countChanged();
}

void FluentComboBox::removeItem(int index) {
    if (index < 0 || index >= m_items.size()) return;
    m_items.removeAt(index);
    if (m_currentIndex == index) setCurrentIndex(qMin(index, m_items.size() - 1));
    else if (m_currentIndex > index) m_currentIndex--;
    emit countChanged();
}

void FluentComboBox::clear() {
    m_items.clear();
    m_currentIndex = -1;
    m_editText.clear();
    update();
    emit countChanged();
    emit currentIndexChanged(-1);
}

// ============================================================
// 弹出列表
// ============================================================

QRect FluentComboBox::arrowRect() const {
    return QRect(width() - ARROW_WIDTH, 0, ARROW_WIDTH, HEIGHT);
}

void FluentComboBox::showPopup() {
    if (m_popupVisible || m_items.isEmpty()) return;

    m_popupVisible = true;
    m_popupHoveredRow = -1;
    m_scrollOffsetY = 0;

    updatePopupGeometry();

    // 定位
    QPoint pos = mapToGlobal(QPoint(0, HEIGHT));
    // 确保不超出屏幕
    QRect screen = QApplication::screenAt(pos)->availableGeometry();
    if (pos.y() + m_popup->height() > screen.bottom()) {
        pos.setY(mapToGlobal(QPoint(0, 0)).y() - m_popup->height());
    }
    if (pos.x() + m_popup->width() > screen.right()) {
        pos.setX(screen.right() - m_popup->width());
    }

    m_popup->move(pos);
    m_popup->show();
    m_popup->raise();
    update();
}

void FluentComboBox::hidePopup() {
    if (!m_popupVisible) return;
    m_popupVisible = false;
    m_popup->hide();
    update();
}

void FluentComboBox::updatePopupGeometry() {
    int visibleCount = qMin(m_items.size(), m_maxVisibleItems);
    int popupH = visibleCount * POPUP_ITEM_HEIGHT + 8; // 上下各4px padding
    m_maxPopupHeight = popupH;
    m_popup->setFixedSize(width(), popupH);
    m_popup->update();
}

// ============================================================
// 绘制主控件
// ============================================================

void FluentComboBox::paintFluent(QPainter* painter) {
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
                         (underMouse() ? t->textColorSecondary() : t->dividerColor());
    painter->setPen(QPen(borderColor, focused ? 2 : 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(bgPath);

    // 文本
    QRect tr(LEFT_PADDING, 0, width() - ARROW_WIDTH - LEFT_PADDING, HEIGHT);
    if (m_editable && hasFocus()) {
        QString displayText = m_editText;
        painter->setPen(t->textColorPrimary());
        painter->setFont(t->bodyFont());

        QFontMetrics fm(t->bodyFont());

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

        painter->save();
        painter->setClipRect(tr);
        painter->drawText(tr.adjusted(-m_scrollOffset, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, displayText);
        // 光标
        if (m_cursorBlink) {
            int cx = fm.horizontalAdvance(displayText.left(m_cursorPos)) - m_scrollOffset + tr.x();
            painter->setPen(QPen(t->primaryColor(), 1.5));
            painter->drawLine(cx, tr.y() + 6, cx, tr.bottom() - 6);
        }
        painter->restore();
    } else {
        QString displayText;
        if (m_currentIndex >= 0 && m_currentIndex < m_items.size()) {
            displayText = m_items[m_currentIndex].text;
            painter->setPen(t->textColorPrimary());
        } else if (!m_placeholder.isEmpty()) {
            displayText = m_placeholder;
            painter->setPen(t->textColorSecondary());
        }
        painter->setFont(t->bodyFont());
        painter->drawText(tr, Qt::AlignVCenter | Qt::AlignLeft, displayText);
    }

    // 箭头
    QRect ar = arrowRect();
    painter->setPen(t->textColorSecondary());
    painter->setFont(t->captionFont());
    painter->drawText(ar, Qt::AlignCenter, m_popupVisible ? u8"▲" : u8"▼");
}

// ============================================================
// 弹出列表绘制 (通过 eventFilter)
// ============================================================

void paintPopup(QWidget* popup, FluentComboBox* combo, int hoveredRow, int scrollOffset) {
    auto* t = FluentThemeManager::instance().currentTheme();
    int itemH = FluentComboBox::POPUP_ITEM_HEIGHT;

    QPainter painter(popup);
    painter.setRenderHint(QPainter::Antialiasing);

    // 背景 + 圆角
    QPainterPath bg;
    bg.addRoundedRect(popup->rect(), FluentComboBox::POPUP_BORDER_RADIUS, FluentComboBox::POPUP_BORDER_RADIUS);
    painter.setPen(Qt::NoPen);
    painter.setBrush(t->cardColor());
    painter.drawPath(bg);

    // 阴影边框
    painter.setPen(QPen(t->dividerColor(), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(bg);

    // 绘制项
    const auto& items = combo->m_items;
    int visibleCount = qMin(items.size(), combo->m_maxVisibleItems);
    int currentIdx = combo->currentIndex();

    painter.setClipRect(popup->rect().adjusted(2, 4, -2, -4));

    for (int i = 0; i < items.size(); ++i) {
        int y = 4 + (i * itemH) - scrollOffset;
        if (y + itemH < 4 || y > popup->height() - 4) continue;

        QRect itemR(4, y, popup->width() - 8, itemH);

        // 悬停
        if (i == hoveredRow) {
            QColor hoverColor = t->textColorPrimary();
            hoverColor.setAlphaF(0.06);
            painter.setPen(Qt::NoPen);
            painter.setBrush(hoverColor);
            painter.drawRoundedRect(itemR, 4, 4);
        }

        // 选中标记
        if (i == currentIdx) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(t->primaryColor());
            painter.drawRoundedRect(QRect(6, y + itemH / 2 - 2, 3, 4), 1.5, 1.5);
        }

        // 图标
        int textX = 16;
        if (!items[i].icon.isNull()) {
            QRect iconR(textX, y + (itemH - 16) / 2, 16, 16);
            items[i].icon.paint(&painter, iconR);
            textX += 22;
        }

        // 文字
        if (i == currentIdx) {
            painter.setPen(t->primaryColor());
            painter.setFont(t->bodyStrongFont());
        } else {
            painter.setPen(t->textColorPrimary());
            painter.setFont(t->bodyFont());
        }
        QRect textR(textX, y, itemR.width() - textX - 4, itemH);
        painter.drawText(textR, Qt::AlignVCenter | Qt::AlignLeft, items[i].text);
    }

    // 滚动条
    int totalH = items.size() * itemH;
    int viewH = popup->height() - 8;
    if (totalH > viewH) {
        int barH = qMax(20, viewH * viewH / totalH);
        int barY = 4 + (scrollOffset * (viewH - barH) / (totalH - viewH));
        painter.setPen(Qt::NoPen);
        QColor barColor = t->textColorSecondary();
        barColor.setAlphaF(0.3);
        painter.setBrush(barColor);
        painter.drawRoundedRect(QRect(popup->width() - FluentComboBox::SCROLL_BAR_WIDTH - 2, barY, FluentComboBox::SCROLL_BAR_WIDTH, barH), 3, 3);
    }
}

// ============================================================
// 事件
// ============================================================

void FluentComboBox::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    if (m_popupVisible) {
        // 点击弹出列表中的项
        int itemH = POPUP_ITEM_HEIGHT;
        int y = event->globalY() - m_popup->y() - 4 + m_scrollOffsetY;
        int row = y / itemH;
        if (row >= 0 && row < m_items.size()) {
            setCurrentIndex(row);
            emit activated(row);
        }
        hidePopup();
    } else {
        if (arrowRect().contains(event->pos()) || !m_editable) {
            m_pressed = true;
            showPopup();
        } else if (m_editable) {
            setFocus();
            // 光标定位
            QString displayText = currentText();
            QFontMetrics fm(theme()->bodyFont());
            int clickX = event->pos().x() - LEFT_PADDING + m_scrollOffset;
            m_cursorPos = 0;
            for (int i = 1; i <= displayText.length(); ++i) {
                if (fm.horizontalAdvance(displayText.left(i)) >= clickX) { m_cursorPos = i; break; }
            }
            m_selectionStart = m_cursorPos;
            m_selectionEnd = m_cursorPos;
            m_selecting = true;
            m_editText = displayText;
            m_cursorBlink = true;
            if (m_blinkTimerId) killTimer(m_blinkTimerId);
            m_blinkTimerId = startTimer(500);
        }
    }
    update();
}

void FluentComboBox::mouseReleaseEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    m_pressed = false;
    m_selecting = false;
    update();
}

void FluentComboBox::mouseMoveEvent(QMouseEvent* event) {
    if (m_popupVisible) {
        // 弹出列表悬停
        int itemH = POPUP_ITEM_HEIGHT;
        int y = event->globalY() - m_popup->y() - 4 + m_scrollOffsetY;
        int row = y / itemH;
        m_popupHoveredRow = (row >= 0 && row < m_items.size()) ? row : -1;
        m_popup->update();
    }
}

void FluentComboBox::leaveEvent(QEvent* event) {
    m_pressed = false;
    FluentWidget::leaveEvent(event);
}

void FluentComboBox::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_Up:
        if (m_popupVisible) {
            setCurrentIndex(qMax(0, m_currentIndex - 1));
        } else {
            setCurrentIndex(qMax(0, m_currentIndex - 1));
        }
        break;
    case Qt::Key_Down:
        if (m_popupVisible) {
            setCurrentIndex(qMin(m_items.size() - 1, m_currentIndex + 1));
        } else if (event->modifiers() == Qt::NoModifier) {
            showPopup();
        } else {
            setCurrentIndex(qMin(m_items.size() - 1, m_currentIndex + 1));
        }
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if (m_popupVisible) {
            emit activated(m_currentIndex);
            hidePopup();
        }
        break;
    case Qt::Key_Escape:
        if (m_popupVisible) hidePopup();
        break;
    case Qt::Key_F4:
        if (m_popupVisible) hidePopup();
        else showPopup();
        break;
    default:
        if (m_editable && event->text().length() == 1) {
            if (!hasFocus()) setFocus();
            if (m_editText.isEmpty()) m_editText = currentText();
            QChar ch = event->text()[0];
            m_editText.insert(m_cursorPos, ch);
            m_cursorPos++;
            m_selectionStart = m_selectionEnd = m_cursorPos;
            m_cursorBlink = true;
            if (m_blinkTimerId) killTimer(m_blinkTimerId);
            m_blinkTimerId = startTimer(500);

            // 过滤匹配项
            for (int i = 0; i < m_items.size(); ++i) {
                if (m_items[i].text.startsWith(m_editText, Qt::CaseInsensitive)) {
                    setCurrentIndex(i);
                    break;
                }
            }
        }
        FluentWidget::keyPressEvent(event);
    }
    update();
}

void FluentComboBox::focusInEvent(QFocusEvent* event) {
    if (m_editable) {
        m_editText = currentText();
        m_cursorPos = 0;
        m_selectionStart = 0;
        m_selectionEnd = m_editText.length();
        m_cursorBlink = true;
        if (m_blinkTimerId) killTimer(m_blinkTimerId);
        m_blinkTimerId = startTimer(500);
    }
    FluentWidget::focusInEvent(event);
}

void FluentComboBox::focusOutEvent(QFocusEvent* event) {
    if (m_popupVisible) hidePopup();
    if (m_editable) {
        m_editText.clear();
    }
    if (m_blinkTimerId) { killTimer(m_blinkTimerId); m_blinkTimerId = 0; }
    FluentWidget::focusOutEvent(event);
}

bool FluentComboBox::eventFilter(QObject* obj, QEvent* event) {
    if (obj == m_popup) {
        if (event->type() == QEvent::Paint) {
            paintPopup(m_popup, this, m_popupHoveredRow, m_scrollOffsetY);
            return true;
        }
        if (event->type() == QEvent::MouseButtonPress) {
            auto* me = static_cast<QMouseEvent*>(event);
            int itemH = POPUP_ITEM_HEIGHT;
            int y = me->pos().y() - 4 + m_scrollOffsetY;
            int row = y / itemH;
            if (row >= 0 && row < m_items.size() && me->button() == Qt::LeftButton) {
                setCurrentIndex(row);
                emit activated(row);
            }
            hidePopup();
            return true;
        }
        if (event->type() == QEvent::MouseMove) {
            auto* me = static_cast<QMouseEvent*>(event);
            int itemH = POPUP_ITEM_HEIGHT;
            int y = me->pos().y() - 4 + m_scrollOffsetY;
            int row = y / itemH;
            m_popupHoveredRow = (row >= 0 && row < m_items.size()) ? row : -1;
            m_popup->update();
            return true;
        }
        if (event->type() == QEvent::Leave) {
            m_popupHoveredRow = -1;
            m_popup->update();
            return true;
        }
        if (event->type() == QEvent::Wheel) {
            auto* we = static_cast<QWheelEvent*>(event);
            int itemH = POPUP_ITEM_HEIGHT;
            int maxScroll = qMax(0, m_items.size() * itemH - (m_popup->height() - 8));
            m_scrollOffsetY -= we->angleDelta().y() / 2;
            m_scrollOffsetY = qBound(0, m_scrollOffsetY, maxScroll);
            m_popup->update();
            return true;
        }
        if (event->type() == QEvent::Hide) {
            m_popupVisible = false;
            update();
        }
    }
    return FluentWidget::eventFilter(obj, event);
}

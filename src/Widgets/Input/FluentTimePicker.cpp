#include "FluentTimePicker.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QScreen>

// ========== WheelColumn ==========

WheelColumn::WheelColumn(QWidget* parent)
    : QWidget(parent)
{
    setFixedWidth(56);
    setFixedHeight(m_itemHeight * m_visibleItems);
    setFocusPolicy(Qt::StrongFocus);
}

void WheelColumn::setRange(int min, int max) {
    m_min = min;
    m_max = max;
    m_labels.clear();
    for (int i = min; i <= max; ++i) {
        m_labels << QString::number(i).rightJustified(2, '0');
    }
    update();
}

int WheelColumn::value() const { return m_value; }

void WheelColumn::setValue(int val, bool emitSignal) {
    val = qBound(m_min, val, m_max);
    if (m_value != val) {
        m_value = val;
        if (emitSignal) emit valueChanged(val);
        update();
    }
}

void WheelColumn::setLabels(const QStringList& labels) {
    m_labels = labels;
    update();
}

QSize WheelColumn::sizeHint() const { return QSize(56, m_itemHeight * m_visibleItems); }
QSize WheelColumn::minimumSizeHint() const { return QSize(56, m_itemHeight * m_visibleItems); }

void WheelColumn::paintEvent(QPaintEvent* /*event*/) {
    auto* theme = FluentThemeManager::instance().currentTheme();
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int centerY = height() / 2;
    int centerRow = m_visibleItems / 2;

    // 选中行高亮背景
    QRectF selRect(2, centerY - m_itemHeight / 2.0, width() - 4, m_itemHeight);
    p.setPen(Qt::NoPen);
    p.setBrush(theme->surfaceColor());
    p.drawRoundedRect(selRect, 6, 6);

    // 上下渐隐遮罩
    QLinearGradient topGrad(0, 0, 0, centerY - m_itemHeight / 2);
    topGrad.setColorAt(0, theme->cardColor());
    topGrad.setColorAt(1, QColor(theme->cardColor().red(), theme->cardColor().green(),
                                  theme->cardColor().blue(), 0));
    p.fillRect(QRect(0, 0, width(), centerY - m_itemHeight / 2), topGrad);

    QLinearGradient botGrad(0, centerY + m_itemHeight / 2, 0, height());
    botGrad.setColorAt(0, QColor(theme->cardColor().red(), theme->cardColor().green(),
                                  theme->cardColor().blue(), 0));
    botGrad.setColorAt(1, theme->cardColor());
    p.fillRect(QRect(0, centerY + m_itemHeight / 2, width(), centerY - m_itemHeight / 2), botGrad);

    // 绘制项目
    QFont normalFont = theme->bodyFont();
    QFont selectedFont = theme->bodyStrongFont();
    int count = m_max - m_min + 1;

    int dragOffset = m_dragging ? m_dragDelta : 0;
    int dragValueOffset = qRound(static_cast<qreal>(dragOffset) / m_itemHeight);

    for (int i = -centerRow; i <= centerRow; ++i) {
        int idx = m_value - m_min + i - dragValueOffset;
        // 循环
        while (idx < 0) idx += count;
        while (idx >= count) idx -= count;

        int y = centerY + i * m_itemHeight - dragOffset + dragValueOffset * m_itemHeight;
        QRectF itemRect(0, y - m_itemHeight / 2, width(), m_itemHeight);

        if (itemRect.bottom() < 0 || itemRect.top() > height()) continue;

        bool isSelected = (i == 0 && !m_dragging);

        p.setFont(isSelected ? selectedFont : normalFont);

        // 距离中心越远越淡
        qreal dist = qAbs(y - centerY) / (m_itemHeight * 2.5);
        qreal alpha = qMax(0.15, 1.0 - dist);

        if (isSelected) {
            p.setPen(theme->primaryColor());
        } else {
            QColor c = theme->textColorPrimary();
            c.setAlphaF(alpha);
            p.setPen(c);
        }

        QString text = (idx >= 0 && idx < m_labels.size()) ? m_labels[idx] : QString();
        p.drawText(itemRect, Qt::AlignCenter, text);
    }
}

void WheelColumn::wheelEvent(QWheelEvent* event) {
    int delta = event->angleDelta().y() > 0 ? -1 : 1;
    setValue(m_value + delta);
    event->accept();
}

void WheelColumn::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartY = event->pos().y();
        m_dragDelta = 0;
        m_dragStartValue = m_value;
    }
}

void WheelColumn::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        m_dragDelta = event->pos().y() - m_dragStartY;
        update();
    }
}

void WheelColumn::mouseReleaseEvent(QMouseEvent* /*event*/) {
    if (m_dragging) {
        int steps = qRound(static_cast<qreal>(m_dragDelta) / m_itemHeight);
        setValue(m_dragStartValue - steps);
        m_dragging = false;
        m_dragDelta = 0;
        update();
    }
}

// ========== FluentTimePicker ==========

FluentTimePicker::FluentTimePicker(QWidget* parent)
    : FluentWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumHeight(HEIGHT);
    setMinimumWidth(120);
    setMouseTracking(true);

    // 弹出面板
    m_popup = new QWidget(nullptr);
    m_popup->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    m_popup->setAttribute(Qt::WA_TranslucentBackground, false);
    m_popup->installEventFilter(this);

    auto* popupLayout = new QHBoxLayout(m_popup);
    popupLayout->setContentsMargins(8, 8, 8, 8);
    popupLayout->setSpacing(4);

    // 时
    m_hourWheel = new WheelColumn(m_popup);
    m_hourWheel->setRange(0, 23);

    auto* colonLabel1 = new QLabel(u8":", m_popup);
    colonLabel1->setAlignment(Qt::AlignCenter);
    colonLabel1->setFixedWidth(12);

    // 分
    m_minuteWheel = new WheelColumn(m_popup);
    m_minuteWheel->setRange(0, 59);

    // 秒
    m_secondWheel = new WheelColumn(m_popup);
    m_secondWheel->setRange(0, 59);
    m_secondWheel->hide();

    // AM/PM
    m_ampmWheel = new WheelColumn(m_popup);
    m_ampmWheel->setRange(0, 1);
    m_ampmWheel->setLabels(QStringList() << u8"AM" << u8"PM");
    m_ampmWheel->hide();

    popupLayout->addWidget(m_hourWheel);
    popupLayout->addWidget(colonLabel1);
    popupLayout->addWidget(m_minuteWheel);
    popupLayout->addWidget(m_secondWheel);
    popupLayout->addWidget(m_ampmWheel);

    // 连接信号
    connect(m_hourWheel, &WheelColumn::valueChanged, this, [this](int val) {
        QTime t = m_time;
        m_time = QTime(val, t.minute(), t.second());
        m_hasTime = true;
        emit timeChanged(m_time);
        update();
    });

    connect(m_minuteWheel, &WheelColumn::valueChanged, this, [this](int val) {
        QTime t = m_time;
        m_time = QTime(t.hour(), val, t.second());
        m_hasTime = true;
        emit timeChanged(m_time);
        update();
    });

    connect(m_secondWheel, &WheelColumn::valueChanged, this, [this](int val) {
        QTime t = m_time;
        m_time = QTime(t.hour(), t.minute(), val);
        m_hasTime = true;
        emit timeChanged(m_time);
        update();
    });

    connect(m_ampmWheel, &WheelColumn::valueChanged, this, [this](int val) {
        QTime t = m_time;
        int h = t.hour();
        if (val == 0) { // AM
            if (h >= 12) h -= 12;
        } else { // PM
            if (h < 12) h += 12;
        }
        m_time = QTime(h, t.minute(), t.second());
        m_hasTime = true;
        emit timeChanged(m_time);
        update();
    });

    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [this]() {
        update();
        updatePopupTheme();
    });
}

FluentTimePicker::~FluentTimePicker() {
    if (m_popup) {
        m_popup->hide();
        m_popup->deleteLater();
    }
}

// === 属性 ===

QTime FluentTimePicker::time() const { return m_time; }
void FluentTimePicker::setTime(const QTime& time) {
    if (m_time != time || !m_hasTime) {
        m_time = time;
        m_hasTime = time.isValid();
        if (m_hourWheel) m_hourWheel->setValue(time.hour(), false);
        if (m_minuteWheel) m_minuteWheel->setValue(time.minute(), false);
        if (m_secondWheel) m_secondWheel->setValue(time.second(), false);
        if (m_ampmWheel) m_ampmWheel->setValue(time.hour() >= 12 ? 1 : 0, false);
        emit timeChanged(time);
        update();
    }
}

void FluentTimePicker::clear() {
    m_hasTime = false;
    m_time = QTime();
    update();
}

bool FluentTimePicker::is24Hour() const { return m_is24Hour; }
void FluentTimePicker::setIs24Hour(bool on) {
    if (m_is24Hour != on) {
        m_is24Hour = on;
        if (m_hourWheel) {
            m_hourWheel->setRange(0, on ? 23 : 11);
            QStringList labels;
            int max = on ? 23 : 11;
            for (int i = 0; i <= max; ++i) {
                labels << QString::number(i).rightJustified(2, '0');
            }
            m_hourWheel->setLabels(labels);
        }
        if (m_ampmWheel) {
            m_ampmWheel->setVisible(!on);
        }
        emit is24HourChanged();
        emit formatChanged();
        update();
    }
}

bool FluentTimePicker::showSeconds() const { return m_showSeconds; }
void FluentTimePicker::setShowSeconds(bool on) {
    if (m_showSeconds != on) {
        m_showSeconds = on;
        if (m_secondWheel) m_secondWheel->setVisible(on);
        emit showSecondsChanged();
        emit formatChanged();
        update();
    }
}

QString FluentTimePicker::placeholder() const { return m_placeholder; }
void FluentTimePicker::setPlaceholder(const QString& text) {
    if (m_placeholder != text) { m_placeholder = text; emit placeholderChanged(); update(); }
}

QString FluentTimePicker::format() const {
    if (m_is24Hour) {
        return m_showSeconds ? u8"HH:mm:ss" : u8"HH:mm";
    } else {
        return m_showSeconds ? u8"hh:mm:ss AP" : u8"hh:mm AP";
    }
}

QSize FluentTimePicker::sizeHint() const { return QSize(160, HEIGHT); }
QSize FluentTimePicker::minimumSizeHint() const { return QSize(120, HEIGHT); }

// === 布局 ===

QRect FluentTimePicker::iconRect() const {
    return QRect(width() - RIGHT_PADDING - ICON_SIZE, (height() - ICON_SIZE) / 2,
                 ICON_SIZE, ICON_SIZE);
}

QRect FluentTimePicker::textRect() const {
    return QRect(LEFT_PADDING, 0, width() - LEFT_PADDING - RIGHT_PADDING - ICON_SIZE - 4, height());
}

// === 格式化 ===

QString FluentTimePicker::formatTime() const {
    if (!m_hasTime || !m_time.isValid()) return QString();
    return m_time.toString(format());
}

// === 绘制 ===

void FluentTimePicker::paintFluent(QPainter* painter) {
    auto* theme = FluentThemeManager::instance().currentTheme();

    QRectF bgRect(0, 0, width(), height());
    painter->setRenderHint(QPainter::Antialiasing);

    QColor borderColor = hasFocus() ? theme->primaryColor() : theme->borderColor();
    qreal borderWidth = hasFocus() ? 2.0 : 1.0;

    if (isHovered() && !hasFocus()) {
        borderColor = theme->primaryColor().lighter(150);
    }

    painter->setPen(QPen(borderColor, borderWidth));
    painter->setBrush(theme->cardColor());
    painter->drawRoundedRect(bgRect.adjusted(0.5, 0.5, -0.5, -0.5), 6, 6);

    // 底部强调线
    if (hasFocus()) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(theme->primaryColor());
        painter->drawRoundedRect(QRectF(2, height() - 2.5, width() - 4, 2), 1, 1);
    }

    // 文本
    QFont textFont = theme->bodyFont();
    painter->setFont(textFont);

    QString display = formatTime();
    if (!display.isEmpty()) {
        painter->setPen(theme->textColorPrimary());
        painter->drawText(textRect(), Qt::AlignLeft | Qt::AlignVCenter, display);
    } else if (!m_placeholder.isEmpty()) {
        painter->setPen(theme->textColorDisabled());
        painter->drawText(textRect(), Qt::AlignLeft | Qt::AlignVCenter, m_placeholder);
    }

    // 时钟图标
    QRect iconR = iconRect();
    qreal cx = iconR.center().x();
    qreal cy = iconR.center().y();
    qreal r = ICON_SIZE / 2.0 - 1;

    QColor iconColor = hasFocus() ? theme->primaryColor() : theme->textColorSecondary();
    painter->setPen(QPen(iconColor, 1.3));
    painter->setBrush(Qt::NoBrush);

    // 圆
    painter->drawEllipse(QPointF(cx, cy), r, r);

    // 时针
    painter->drawLine(QPointF(cx, cy), QPointF(cx, cy - r * 0.5));

    // 分针
    painter->drawLine(QPointF(cx, cy), QPointF(cx + r * 0.35, cy + r * 0.1));

    // 中心点
    painter->setBrush(iconColor);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(QPointF(cx, cy), 1.2, 1.2);
}

// === 弹出 ===

void FluentTimePicker::showPopup() {
    if (m_popupVisible) return;

    // 同步滚轮值
    if (m_hasTime && m_time.isValid()) {
        m_hourWheel->setValue(m_time.hour(), false);
        m_minuteWheel->setValue(m_time.minute(), false);
        m_secondWheel->setValue(m_time.second(), false);
        m_ampmWheel->setValue(m_time.hour() >= 12 ? 1 : 0, false);
    }

    updatePopupTheme();

    // 计算位置
    QPoint globalPos = mapToGlobal(QPoint(0, height() + 4));
    QSize popupSize = m_popup->sizeHint();

    QScreen* screen = QApplication::screenAt(globalPos);
    if (screen) {
        QRect screenRect = screen->availableGeometry();
        if (globalPos.y() + popupSize.height() > screenRect.bottom()) {
            globalPos = mapToGlobal(QPoint(0, -popupSize.height() - 4));
        }
        if (globalPos.x() + popupSize.width() > screenRect.right()) {
            globalPos.setX(screenRect.right() - popupSize.width() - 4);
        }
    }

    m_popup->move(globalPos);
    m_popup->adjustSize();
    m_popup->show();
    m_popupVisible = true;
    update();
}

void FluentTimePicker::hidePopup() {
    if (m_popup) m_popup->hide();
    m_popupVisible = false;
    update();
}

void FluentTimePicker::updatePopupTheme() {
    if (!m_popup) return;
    auto* theme = FluentThemeManager::instance().currentTheme();
    m_popup->setAutoFillBackground(true);
    QPalette pal = m_popup->palette();
    pal.setColor(QPalette::Window, theme->cardColor());
    m_popup->setPalette(pal);

    m_popup->setStyleSheet(QString(
        "QWidget { background-color: %1; }"
        "QLabel { color: %2; font-size: 14px; font-weight: bold; }"
    ).arg(theme->cardColor().name(), theme->textColorPrimary().name()));
}

// === 事件 ===

void FluentTimePicker::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (m_popupVisible) hidePopup();
        else showPopup();
    }
    FluentWidget::mousePressEvent(event);
}

void FluentTimePicker::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_Space:
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if (m_popupVisible) hidePopup();
        else showPopup();
        break;
    case Qt::Key_Escape:
        if (m_popupVisible) hidePopup();
        break;
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        clear();
        break;
    default:
        FluentWidget::keyPressEvent(event);
    }
}

void FluentTimePicker::focusInEvent(QFocusEvent* event) {
    FluentWidget::focusInEvent(event);
    update();
}

void FluentTimePicker::focusOutEvent(QFocusEvent* event) {
    FluentWidget::focusOutEvent(event);
    QTimer::singleShot(150, this, [this]() {
        if (!hasFocus() && m_popupVisible && !m_popup->hasFocus()) {
            QWidget* focusW = QApplication::focusWidget();
            if (!m_popup->isAncestorOf(focusW)) {
                hidePopup();
            }
        }
    });
    update();
}

bool FluentTimePicker::eventFilter(QObject* obj, QEvent* event) {
    if (obj == m_popup) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            QPoint globalPos = me->globalPos();
            QPoint localPos = m_popup->mapFromGlobal(globalPos);
            if (!m_popup->rect().contains(localPos)) {
                hidePopup();
                return true;
            }
        } else if (event->type() == QEvent::Hide) {
            m_popupVisible = false;
            update();
        }
    }
    return FluentWidget::eventFilter(obj, event);
}

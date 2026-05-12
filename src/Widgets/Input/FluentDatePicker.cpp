#include "FluentDatePicker.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include "Widgets/Data/FluentCalendar.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QApplication>
#include <QScreen>

FluentDatePicker::FluentDatePicker(QWidget* parent)
    : FluentWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumHeight(HEIGHT);
    setMinimumWidth(140);
    setMouseTracking(true);

    // 弹出日历面板
    m_popup = new QWidget(nullptr);
    m_popup->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    m_popup->setAttribute(Qt::WA_TranslucentBackground);
    m_popup->installEventFilter(this);

    auto* popupLayout = new QVBoxLayout(m_popup);
    popupLayout->setContentsMargins(0, 0, 0, 0);

    m_calendar = new FluentCalendar(m_popup);
    m_calendar->setSelectionMode(FluentCalendar::SingleDate);
    popupLayout->addWidget(m_calendar);

    connect(m_calendar, &FluentCalendar::selectedDateChanged, this, [this](const QDate& date) {
        setDate(date);
        hidePopup();
    });

    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [this]() {
        update();
        updateCalendarTheme();
    });
}

FluentDatePicker::~FluentDatePicker() {
    if (m_popup) {
        m_popup->hide();
        m_popup->deleteLater();
    }
}

// === 属性 ===

QDate FluentDatePicker::date() const { return m_date; }
void FluentDatePicker::setDate(const QDate& date) {
    if (m_date != date) {
        m_date = date;
        if (m_calendar) m_calendar->setSelectedDate(date);
        emit dateChanged(date);
        update();
    }
}

void FluentDatePicker::clear() {
    m_date = QDate();
    update();
}

QString FluentDatePicker::format() const { return m_format; }
void FluentDatePicker::setFormat(const QString& fmt) {
    if (m_format != fmt) { m_format = fmt; emit formatChanged(); update(); }
}

QString FluentDatePicker::placeholder() const { return m_placeholder; }
void FluentDatePicker::setPlaceholder(const QString& text) {
    if (m_placeholder != text) { m_placeholder = text; emit placeholderChanged(); update(); }
}

bool FluentDatePicker::calendarPopup() const { return m_calendarPopup; }
void FluentDatePicker::setCalendarPopup(bool on) {
    if (m_calendarPopup != on) { m_calendarPopup = on; emit calendarPopupChanged(); }
}

QDate FluentDatePicker::minimumDate() const { return m_minDate; }
void FluentDatePicker::setMinimumDate(const QDate& date) {
    if (m_minDate != date) {
        m_minDate = date;
        if (m_calendar) m_calendar->setMinimumDate(date);
        emit minimumDateChanged();
    }
}

QDate FluentDatePicker::maximumDate() const { return m_maxDate; }
void FluentDatePicker::setMaximumDate(const QDate& date) {
    if (m_maxDate != date) {
        m_maxDate = date;
        if (m_calendar) m_calendar->setMaximumDate(date);
        emit maximumDateChanged();
    }
}

QSize FluentDatePicker::sizeHint() const { return QSize(180, HEIGHT); }
QSize FluentDatePicker::minimumSizeHint() const { return QSize(140, HEIGHT); }

// === 布局 ===

QRect FluentDatePicker::iconRect() const {
    return QRect(width() - RIGHT_PADDING - ICON_SIZE, (height() - ICON_SIZE) / 2,
                 ICON_SIZE, ICON_SIZE);
}

QRect FluentDatePicker::textRect() const {
    return QRect(LEFT_PADDING, 0, width() - LEFT_PADDING - RIGHT_PADDING - ICON_SIZE - 4, height());
}

// === 绘制 ===

void FluentDatePicker::paintFluent(QPainter* painter) {
    auto* theme = FluentThemeManager::instance().currentTheme();

    // 背景
    QRectF bgRect(0, 0, width(), height());
    painter->setRenderHint(QPainter::Antialiasing);

    QColor bgColor = theme->cardColor();
    QColor borderColor = hasFocus() ? theme->primaryColor() : theme->borderColor();
    qreal borderWidth = hasFocus() ? 2.0 : 1.0;

    // 悬停边框
    if (isHovered() && !hasFocus()) {
        borderColor = theme->primaryColor().lighter(150);
    }

    painter->setPen(QPen(borderColor, borderWidth));
    painter->setBrush(bgColor);
    painter->drawRoundedRect(bgRect.adjusted(0.5, 0.5, -0.5, -0.5), 6, 6);

    // 底部强调线（聚焦时）
    if (hasFocus()) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(theme->primaryColor());
        painter->drawRoundedRect(QRectF(2, height() - 2.5, width() - 4, 2), 1, 1);
    }

    // 文本
    QFont textFont = theme->bodyFont();
    painter->setFont(textFont);

    if (m_date.isValid()) {
        painter->setPen(theme->textColorPrimary());
        painter->drawText(textRect(), Qt::AlignLeft | Qt::AlignVCenter,
                          m_date.toString(m_format));
    } else if (!m_placeholder.isEmpty()) {
        painter->setPen(theme->textColorDisabled());
        painter->drawText(textRect(), Qt::AlignLeft | Qt::AlignVCenter, m_placeholder);
    }

    // 日历图标（简易绘制）
    QRect iconR = iconRect();
    qreal s = ICON_SIZE;
    qreal ix = iconR.x();
    qreal iy = iconR.y();

    // 日历主体
    QColor iconColor = hasFocus() ? theme->primaryColor() : theme->textColorSecondary();
    painter->setPen(QPen(iconColor, 1.2));
    painter->setBrush(Qt::NoBrush);

    // 外框
    QRectF calBody(ix + 1, iy + 3, s - 2, s - 4);
    painter->drawRoundedRect(calBody, 2, 2);

    // 顶部横线
    painter->drawLine(QPointF(ix + 1, iy + 6), QPointF(ix + s - 1, iy + 6));

    // 挂钩
    painter->drawLine(QPointF(ix + 4, iy + 1), QPointF(ix + 4, iy + 5));
    painter->drawLine(QPointF(ix + s - 4, iy + 1), QPointF(ix + s - 4, iy + 5));

    // 日期点 (3x2 grid)
    painter->setPen(Qt::NoPen);
    painter->setBrush(iconColor);
    qreal dotR = 1.2;
    qreal dotStartX = ix + 4;
    qreal dotStartY = iy + 9;
    qreal dotSpacingX = (s - 8) / 2.0;
    qreal dotSpacingY = (s - 12) / 2.0;

    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 3; ++col) {
            qreal dx = dotStartX + col * dotSpacingX;
            qreal dy = dotStartY + row * dotSpacingY;
            painter->drawEllipse(QPointF(dx, dy), dotR, dotR);
        }
    }
}

// === 弹出 ===

void FluentDatePicker::showPopup() {
    if (!m_calendarPopup || m_popupVisible) return;

    // 更新日历状态
    if (m_date.isValid()) {
        m_calendar->setSelectedDate(m_date);
        m_calendar->setYearMonth(m_date.year(), m_date.month());
    }
    updateCalendarTheme();

    // 计算弹出位置
    QPoint globalPos = mapToGlobal(QPoint(0, height() + 4));
    QSize calSize = m_calendar->sizeHint();

    // 检查是否超出屏幕底部
    QScreen* screen = QApplication::screenAt(globalPos);
    if (screen) {
        QRect screenRect = screen->availableGeometry();
        if (globalPos.y() + calSize.height() > screenRect.bottom()) {
            globalPos = mapToGlobal(QPoint(0, -calSize.height() - 4));
        }
        if (globalPos.x() + calSize.width() > screenRect.right()) {
            globalPos.setX(screenRect.right() - calSize.width() - 4);
        }
    }

    m_popup->move(globalPos);
    m_popup->resize(calSize);
    m_popup->show();
    m_popupVisible = true;
    update();
}

void FluentDatePicker::hidePopup() {
    if (m_popup) {
        m_popup->hide();
    }
    m_popupVisible = false;
    update();
}

void FluentDatePicker::updateCalendarTheme() {
    if (!m_popup) return;
    // 弹出面板背景色跟随主题
    auto* theme = FluentThemeManager::instance().currentTheme();
    m_popup->setStyleSheet(QString(
        "QWidget { background-color: %1; border-radius: 8px; }"
    ).arg(theme->cardColor().name()));
}

// === 事件 ===

void FluentDatePicker::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (m_calendarPopup) {
            if (m_popupVisible) {
                hidePopup();
            } else {
                showPopup();
            }
        }
    }
    FluentWidget::mousePressEvent(event);
}

void FluentDatePicker::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_Space:
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if (m_calendarPopup) {
            if (m_popupVisible) hidePopup();
            else showPopup();
        }
        break;
    case Qt::Key_Escape:
        if (m_popupVisible) hidePopup();
        break;
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        if (!m_date.isNull()) {
            clear();
        }
        break;
    default:
        FluentWidget::keyPressEvent(event);
    }
}

void FluentDatePicker::focusInEvent(QFocusEvent* event) {
    FluentWidget::focusInEvent(event);
    update();
}

void FluentDatePicker::focusOutEvent(QFocusEvent* event) {
    FluentWidget::focusOutEvent(event);
    // 延迟隐藏，避免点击日历时立刻关闭
    QTimer::singleShot(150, this, [this]() {
        if (!hasFocus() && m_popupVisible && !m_popup->hasFocus()) {
            // 检查日历内部是否有焦点
            QWidget* focusW = QApplication::focusWidget();
            if (!m_popup->isAncestorOf(focusW)) {
                hidePopup();
            }
        }
    });
    update();
}

bool FluentDatePicker::eventFilter(QObject* obj, QEvent* event) {
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

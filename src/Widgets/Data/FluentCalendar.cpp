#include "FluentCalendar.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QDate>
#include <QFontMetrics>

// ============================================================
// FluentCalendar
// ============================================================

FluentCalendar::FluentCalendar(QWidget* parent)
    : FluentWidget(parent)
    , m_year(QDate::currentDate().year())
    , m_month(QDate::currentDate().month())
    , m_selectedDate(QDate::currentDate())
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(minimumSizeHint());

    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

FluentCalendar::~FluentCalendar() = default;

// ============================================================
// 属性
// ============================================================

int FluentCalendar::year() const { return m_year; }
int FluentCalendar::month() const { return m_month; }

void FluentCalendar::setYearMonth(int year, int month) {
    if (m_year == year && m_month == month) return;
    if (month < 1) { month = 12; year--; }
    if (month > 12) { month = 1; year++; }
    m_year = year;
    m_month = month;
    update();
    emit monthChanged(m_year, m_month);
}

QDate FluentCalendar::selectedDate() const { return m_selectedDate; }

void FluentCalendar::setSelectedDate(const QDate& date) {
    if (m_selectedDate == date || !date.isValid()) return;
    m_selectedDate = date;
    if (date.year() != m_year || date.month() != m_month) {
        m_year = date.year();
        m_month = date.month();
    }
    update();
    emit selectedDateChanged(date);
}

QDate FluentCalendar::rangeStart() const { return m_rangeStart; }
QDate FluentCalendar::rangeEnd() const { return m_rangeEnd; }

void FluentCalendar::setDateRange(const QDate& start, const QDate& end) {
    m_rangeStart = start;
    m_rangeEnd = end;
    update();
    emit dateRangeChanged(start, end);
}

FluentCalendar::SelectionMode FluentCalendar::selectionMode() const { return m_selectionMode; }

void FluentCalendar::setSelectionMode(SelectionMode mode) {
    if (m_selectionMode == mode) return;
    m_selectionMode = mode;
    update();
    emit selectionModeChanged();
}

QDate FluentCalendar::minimumDate() const { return m_minDate; }
void FluentCalendar::setMinimumDate(const QDate& date) { m_minDate = date; update(); }
QDate FluentCalendar::maximumDate() const { return m_maxDate; }
void FluentCalendar::setMaximumDate(const QDate& date) { m_maxDate = date; update(); }

void FluentCalendar::setMarkedDates(const QSet<QDate>& dates) { m_markedDates = dates; update(); }
QSet<QDate> FluentCalendar::markedDates() const { return m_markedDates; }

QSize FluentCalendar::sizeHint() const { return QSize(320, HEADER_HEIGHT + WEEKDAY_HEIGHT + ROW_COUNT * 40 + 8); }
QSize FluentCalendar::minimumSizeHint() const { return QSize(260, HEADER_HEIGHT + WEEKDAY_HEIGHT + ROW_COUNT * 32 + 8); }

// ============================================================
// 布局计算
// ============================================================

QRect FluentCalendar::headerRect() const {
    return QRect(0, 0, width(), HEADER_HEIGHT);
}

QRect FluentCalendar::weekdayRect(int col) const {
    int cellW = width() / 7;
    int y = HEADER_HEIGHT;
    return QRect(col * cellW, y, cellW, WEEKDAY_HEIGHT);
}

QRect FluentCalendar::dayCellRect(int row, int col) const {
    int cellW = width() / 7;
    int gridTop = HEADER_HEIGHT + WEEKDAY_HEIGHT;
    int gridH = height() - gridTop - 8;
    int cellH = gridH / ROW_COUNT;
    return QRect(col * cellW, gridTop + row * cellH, cellW, cellH);
}

QRect FluentCalendar::dayCircleRect(int row, int col) const {
    QRect cell = dayCellRect(row, col);
    int side = qMin(cell.width(), cell.height()) - CELL_MARGIN * 2;
    side = qMin(side, 36); // 最大圆形直径
    int x = cell.x() + (cell.width() - side) / 2;
    int y = cell.y() + (cell.height() - side) / 2;
    return QRect(x, y, side, side);
}

QDate FluentCalendar::dateAtCell(int row, int col) const {
    QDate firstDay(m_year, m_month, 1);
    int startWeekday = firstDay.dayOfWeek(); // 1=Mon, 7=Sun
    // 以周日为第一列
    int offset = startWeekday % 7; // Sun=0, Mon=1, ..., Sat=6
    int dayIndex = row * 7 + col - offset + 1;
    return firstDay.addDays(dayIndex - 1);
}

int FluentCalendar::cellAtPos(const QPoint& pos) const {
    int gridTop = HEADER_HEIGHT + WEEKDAY_HEIGHT;
    if (pos.y() < gridTop) return -1;

    int cellW = width() / 7;
    int gridH = height() - gridTop - 8;
    int cellH = gridH / ROW_COUNT;

    int col = pos.x() / cellW;
    int row = (pos.y() - gridTop) / cellH;
    if (col < 0 || col > 6 || row < 0 || row >= ROW_COUNT) return -1;

    return row * 7 + col;
}

bool FluentCalendar::isDateEnabled(const QDate& date) const {
    if (!date.isValid()) return false;
    if (m_minDate.isValid() && date < m_minDate) return false;
    if (m_maxDate.isValid() && date > m_maxDate) return false;
    return true;
}

QRect FluentCalendar::prevMonthRect() const {
    return QRect(8, (HEADER_HEIGHT - NAV_BUTTON_SIZE) / 2, NAV_BUTTON_SIZE, NAV_BUTTON_SIZE);
}

QRect FluentCalendar::nextMonthRect() const {
    return QRect(width() - NAV_BUTTON_SIZE - 8, (HEADER_HEIGHT - NAV_BUTTON_SIZE) / 2,
                 NAV_BUTTON_SIZE, NAV_BUTTON_SIZE);
}

QRect FluentCalendar::todayRect() const {
    QFontMetrics fm(theme()->captionFont());
    int textW = fm.horizontalAdvance(u8"今天");
    int x = width() - NAV_BUTTON_SIZE - 8 - textW - 24;
    int y = (HEADER_HEIGHT - TODAY_BUTTON_H) / 2;
    return QRect(x, y, textW + 16, TODAY_BUTTON_H);
}

void FluentCalendar::goToPrevMonth() {
    int y = m_year, m = m_month - 1;
    if (m < 1) { m = 12; y--; }
    setYearMonth(y, m);
}

void FluentCalendar::goToNextMonth() {
    int y = m_year, m = m_month + 1;
    if (m > 12) { m = 1; y++; }
    setYearMonth(y, m);
}

void FluentCalendar::goToToday() {
    QDate today = QDate::currentDate();
    setYearMonth(today.year(), today.month());
    setSelectedDate(today);
}

// ============================================================
// 绘制
// ============================================================

void FluentCalendar::paintFluent(QPainter* painter) {
    auto* t = theme();

    // 背景
    painter->fillRect(rect(), t->cardColor());

    // ---- 头部 ----
    QRect hdr = headerRect();
    painter->setPen(t->textColorPrimary());
    painter->setFont(t->subtitleFont());
    QString monthTitle = QString(u8"%1年 %2月").arg(m_year).arg(m_month);
    painter->drawText(hdr.adjusted(12, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, monthTitle);

    // 上月按钮 ◀
    QRect prevR = prevMonthRect();
    QColor prevBg = m_pressedPrevMonth ? t->textColorPrimary() :
                    (prevR.contains(mapFromGlobal(QCursor::pos())) ?
                        QColor(t->textColorPrimary()).lighter(180) : Qt::transparent);
    if (prevBg != Qt::transparent) {
        prevBg.setAlphaF(m_pressedPrevMonth ? 0.12 : 0.06);
        painter->setPen(Qt::NoPen);
        painter->setBrush(prevBg);
        painter->drawRoundedRect(prevR, 4, 4);
    }
    painter->setPen(t->textColorSecondary());
    painter->setFont(t->bodyFont());
    painter->drawText(prevR, Qt::AlignCenter, u8"‹");

    // 下月按钮 ▶
    QRect nextR = nextMonthRect();
    QColor nextBg = m_pressedNextMonth ? t->textColorPrimary() :
                    (nextR.contains(mapFromGlobal(QCursor::pos())) ?
                        QColor(t->textColorPrimary()).lighter(180) : Qt::transparent);
    if (nextBg != Qt::transparent) {
        nextBg.setAlphaF(m_pressedNextMonth ? 0.12 : 0.06);
        painter->setPen(Qt::NoPen);
        painter->setBrush(nextBg);
        painter->drawRoundedRect(nextR, 4, 4);
    }
    painter->setPen(t->textColorSecondary());
    painter->drawText(nextR, Qt::AlignCenter, u8"›");

    // 今天按钮
    QRect todayR = todayRect();
    bool todayHovered = todayR.contains(mapFromGlobal(QCursor::pos()));
    if (todayHovered || m_pressedToday) {
        QColor bg = t->primaryColor();
        bg.setAlphaF(m_pressedToday ? 0.12 : 0.06);
        painter->setPen(Qt::NoPen);
        painter->setBrush(bg);
        painter->drawRoundedRect(todayR, 4, 4);
    }
    painter->setPen(t->primaryColor());
    painter->setFont(t->captionFont());
    painter->drawText(todayR, Qt::AlignCenter, u8"今天");

    // ---- 分隔线 ----
    painter->setPen(QPen(t->dividerColor(), 1));
    painter->drawLine(0, hdr.bottom(), width(), hdr.bottom());

    // ---- 星期标题 ----
    QStringList weekLabels = {u8"日", u8"一", u8"二", u8"三", u8"四", u8"五", u8"六"};
    painter->setFont(t->captionFont());
    painter->setPen(t->textColorSecondary());
    for (int col = 0; col < 7; ++col) {
        QRect wr = weekdayRect(col);
        painter->drawText(wr, Qt::AlignCenter, weekLabels[col]);
    }

    // ---- 日期网格 ----
    QDate today = QDate::currentDate();
    QDate firstDay(m_year, m_month, 1);

    // 计算范围高亮区域
    QDate rangeMin, rangeMax;
    if (m_selectionMode == DateRange && m_rangeStart.isValid() && m_rangeEnd.isValid()) {
        if (m_rangeStart <= m_rangeEnd) {
            rangeMin = m_rangeStart;
            rangeMax = m_rangeEnd;
        } else {
            rangeMin = m_rangeEnd;
            rangeMax = m_rangeStart;
        }
    }

    for (int row = 0; row < ROW_COUNT; ++row) {
        for (int col = 0; col < 7; ++col) {
            QDate cellDate = dateAtCell(row, col);
            bool isCurrentMonth = (cellDate.month() == m_month && cellDate.year() == m_year);
            bool isToday = (cellDate == today);
            bool isSelected = (cellDate == m_selectedDate);
            bool isEnabled = isDateEnabled(cellDate) && isCurrentMonth;
            bool isHovered = (m_hoveredCell == row * 7 + col) && isEnabled;
            bool isMarked = m_markedDates.contains(cellDate);
            bool inRange = rangeMin.isValid() && rangeMax.isValid() &&
                           cellDate >= rangeMin && cellDate <= rangeMax;

            QRect cell = dayCellRect(row, col);
            QRect circle = dayCircleRect(row, col);

            // 范围高亮背景
            if (m_selectionMode == DateRange && inRange && isCurrentMonth) {
                QColor rangeBg = t->primaryColor();
                rangeBg.setAlphaF(0.08);

                // 扩展背景到单元格边缘实现连续效果
                int bgLeft = cell.x() + 1;
                int bgRight = cell.x() + cell.width() - 1;
                int bgTop = circle.y() + 2;
                int bgH = circle.height() - 4;

                // 如果不是范围起点，向左延伸
                if (cellDate != rangeMin && col > 0) {
                    QDate prevDate = dateAtCell(row, col - 1);
                    if (prevDate >= rangeMin) bgLeft = cell.x();
                }
                // 如果不是范围终点，向右延伸
                if (cellDate != rangeMax && col < 6) {
                    QDate nextDate = dateAtCell(row, col + 1);
                    if (nextDate <= rangeMax) bgRight = cell.x() + cell.width();
                }

                painter->setPen(Qt::NoPen);
                painter->setBrush(rangeBg);
                painter->drawRect(QRect(bgLeft, bgTop, bgRight - bgLeft, bgH));

                // 范围起点圆形
                if (cellDate == rangeMin) {
                    painter->setBrush(t->primaryColor());
                    painter->drawRoundedRect(QRect(bgLeft, bgTop, circle.x() + circle.width() / 2 - bgLeft, bgH), 0, 0);
                    painter->drawRoundedRect(circle, circle.width() / 2.0, circle.height() / 2.0);
                }
                // 范围终点圆形
                if (cellDate == rangeMax) {
                    painter->setBrush(t->primaryColor());
                    painter->drawRoundedRect(QRect(circle.x() + circle.width() / 2, bgTop, bgRight - circle.x() - circle.width() / 2, bgH), 0, 0);
                    painter->drawRoundedRect(circle, circle.width() / 2.0, circle.height() / 2.0);
                }
            }

            // 悬停背景
            if (isHovered && !isSelected && !(m_selectionMode == DateRange && (cellDate == rangeMin || cellDate == rangeMax))) {
                QColor hoverBg = t->textColorPrimary();
                hoverBg.setAlphaF(0.06);
                painter->setPen(Qt::NoPen);
                painter->setBrush(hoverBg);
                painter->drawRoundedRect(circle, circle.width() / 2.0, circle.height() / 2.0);
            }

            // 选中圆形
            if (isSelected && m_selectionMode == SingleDate && isCurrentMonth) {
                painter->setPen(Qt::NoPen);
                painter->setBrush(t->primaryColor());
                painter->drawRoundedRect(circle, circle.width() / 2.0, circle.height() / 2.0);
            }

            // 日期文字
            if (isSelected && isCurrentMonth) {
                painter->setPen(Qt::white);
                painter->setFont(t->bodyStrongFont());
            } else if (isToday && isCurrentMonth) {
                painter->setPen(t->primaryColor());
                painter->setFont(t->bodyStrongFont());
            } else if (!isEnabled) {
                painter->setPen(t->textColorDisabled());
                painter->setFont(t->bodyFont());
            } else if (inRange && isCurrentMonth) {
                painter->setPen(t->primaryColor());
                painter->setFont(t->bodyFont());
            } else if (isHovered && isCurrentMonth) {
                painter->setPen(t->textColorPrimary());
                painter->setFont(t->bodyFont());
            } else {
                painter->setPen(isCurrentMonth ? t->textColorPrimary() : t->textColorDisabled());
                painter->setFont(t->bodyFont());
            }

            painter->drawText(cell, Qt::AlignCenter, QString::number(cellDate.day()));

            // 今日标记圆环（未选中时）
            if (isToday && !isSelected && isCurrentMonth) {
                painter->setPen(QPen(t->primaryColor(), 2));
                painter->setBrush(Qt::NoBrush);
                painter->drawRoundedRect(circle.adjusted(1, 1, -1, -1),
                                         (circle.width() - 2) / 2.0, (circle.height() - 2) / 2.0);
            }

            // 标记小圆点
            if (isMarked && isCurrentMonth && !isSelected) {
                painter->setPen(Qt::NoPen);
                painter->setBrush(t->primaryColor());
                int dotR = 3;
                painter->drawEllipse(QPoint(circle.center().x(), circle.bottom() - 3), dotR, dotR);
            }
        }
    }

    // 底部分隔线
    painter->setPen(QPen(t->dividerColor(), 1));
    painter->drawLine(0, height() - 1, width(), height() - 1);
}

// ============================================================
// 事件处理
// ============================================================

void FluentCalendar::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    QPoint pos = event->pos();
    m_pressedPrevMonth = prevMonthRect().contains(pos);
    m_pressedNextMonth = nextMonthRect().contains(pos);
    m_pressedToday = todayRect().contains(pos);

    // 日期点击
    int cell = cellAtPos(pos);
    if (cell >= 0) {
        int row = cell / 7, col = cell % 7;
        QDate date = dateAtCell(row, col);
        if (isDateEnabled(date) && date.month() == m_month) {
            if (m_selectionMode == SingleDate) {
                setSelectedDate(date);
            } else {
                // 范围选择：第一次点击设起点，第二次点击设终点
                if (!m_rangeAnchor.isValid() || (m_rangeStart.isValid() && m_rangeEnd.isValid())) {
                    m_rangeAnchor = date;
                    m_rangeStart = date;
                    m_rangeEnd = date;
                    m_isDraggingRange = true;
                } else {
                    m_rangeEnd = date;
                    m_isDraggingRange = false;
                    emit dateRangeChanged(m_rangeStart, m_rangeEnd);
                }
                setSelectedDate(date);
                update();
            }
        }
    }

    update();
}

void FluentCalendar::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    QPoint pos = event->pos();
    if (m_pressedPrevMonth && prevMonthRect().contains(pos)) goToPrevMonth();
    if (m_pressedNextMonth && nextMonthRect().contains(pos)) goToNextMonth();
    if (m_pressedToday && todayRect().contains(pos)) goToToday();

    m_pressedPrevMonth = false;
    m_pressedNextMonth = false;
    m_pressedToday = false;
    m_isDraggingRange = false;
    update();
}

void FluentCalendar::mouseMoveEvent(QMouseEvent* event) {
    int oldHover = m_hoveredCell;
    int cell = cellAtPos(event->pos());
    if (cell >= 0) {
        int row = cell / 7, col = cell % 7;
        QDate date = dateAtCell(row, col);
        if (!isDateEnabled(date) || date.month() != m_month) {
            m_hoveredCell = -1;
        } else {
            m_hoveredCell = cell;
        }
    } else {
        m_hoveredCell = -1;
    }

    // 范围拖拽
    if (m_isDraggingRange && cell >= 0) {
        int row = cell / 7, col = cell % 7;
        QDate date = dateAtCell(row, col);
        if (isDateEnabled(date)) {
            m_rangeEnd = date;
            emit dateRangeChanged(m_rangeStart, m_rangeEnd);
        }
    }

    if (oldHover != m_hoveredCell || m_isDraggingRange) {
        setCursor(m_hoveredCell >= 0 || prevMonthRect().contains(event->pos()) ||
                  nextMonthRect().contains(event->pos()) || todayRect().contains(event->pos())
                  ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
    }
}

void FluentCalendar::leaveEvent(QEvent* event) {
    m_hoveredCell = -1;
    m_pressedPrevMonth = false;
    m_pressedNextMonth = false;
    m_pressedToday = false;
    setCursor(Qt::ArrowCursor);
    update();
    FluentWidget::leaveEvent(event);
}

void FluentCalendar::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() > 0) {
        goToPrevMonth();
    } else if (event->angleDelta().y() < 0) {
        goToNextMonth();
    }
    event->accept();
}

void FluentCalendar::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_Left:
        setSelectedDate(m_selectedDate.addDays(-1));
        break;
    case Qt::Key_Right:
        setSelectedDate(m_selectedDate.addDays(1));
        break;
    case Qt::Key_Up:
        setSelectedDate(m_selectedDate.addDays(-7));
        break;
    case Qt::Key_Down:
        setSelectedDate(m_selectedDate.addDays(7));
        break;
    case Qt::Key_PageUp:
        goToPrevMonth();
        break;
    case Qt::Key_PageDown:
        goToNextMonth();
        break;
    case Qt::Key_Home:
        goToToday();
        break;
    default:
        FluentWidget::keyPressEvent(event);
    }
}

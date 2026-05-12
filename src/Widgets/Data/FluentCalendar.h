#ifndef FLUENTCALENDAR_H
#define FLUENTCALENDAR_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QDate>
#include <QSet>

// Fluent UI 风格日历组件
// 支持: 月份导航、日期选择(单选/范围)、今日标记、主题适配
// 特性: 月份切换动画、悬停效果、选中范围高亮

class FLUENT_EXPORT FluentCalendar : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(QDate selectedDate READ selectedDate WRITE setSelectedDate NOTIFY selectedDateChanged)
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)

public:
    // 选择模式
    enum SelectionMode {
        SingleDate,     // 单日期选择
        DateRange       // 日期范围选择
    };
    Q_ENUM(SelectionMode)

    explicit FluentCalendar(QWidget* parent = nullptr);
    ~FluentCalendar() override;

    // 当前显示的月份
    int year() const;
    int month() const;
    void setYearMonth(int year, int month);

    // 选中日期
    QDate selectedDate() const;
    void setSelectedDate(const QDate& date);

    // 日期范围选择
    QDate rangeStart() const;
    QDate rangeEnd() const;
    void setDateRange(const QDate& start, const QDate& end);

    // 选择模式
    SelectionMode selectionMode() const;
    void setSelectionMode(SelectionMode mode);

    // 最小/最大可选日期
    QDate minimumDate() const;
    void setMinimumDate(const QDate& date);
    QDate maximumDate() const;
    void setMaximumDate(const QDate& date);

    // 标记日期（小圆点）
    void setMarkedDates(const QSet<QDate>& dates);
    QSet<QDate> markedDates() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void selectedDateChanged(const QDate& date);
    void dateRangeChanged(const QDate& start, const QDate& end);
    void selectionModeChanged();
    void monthChanged(int year, int month);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    // 布局计算
    QRect headerRect() const;
    QRect weekdayRect(int col) const;
    QRect dayCellRect(int row, int col) const;
    QRect dayCircleRect(int row, int col) const;
    QDate dateAtCell(int row, int col) const;
    int cellAtPos(const QPoint& pos) const;  // 返回 row*7+col, -1 无效
    bool isDateEnabled(const QDate& date) const;

    // 导航按钮区域
    QRect prevMonthRect() const;
    QRect nextMonthRect() const;
    QRect todayRect() const;

    // 月份导航
    void goToPrevMonth();
    void goToNextMonth();
    void goToToday();

    // 数据
    int m_year;
    int m_month;
    QDate m_selectedDate;
    QDate m_rangeStart;
    QDate m_rangeEnd;
    SelectionMode m_selectionMode = SingleDate;
    QDate m_minDate;
    QDate m_maxDate;
    QSet<QDate> m_markedDates;

    // 交互状态
    int m_hoveredCell = -1;       // row*7+col
    bool m_pressedPrevMonth = false;
    bool m_pressedNextMonth = false;
    bool m_pressedToday = false;
    bool m_isDraggingRange = false;
    QDate m_rangeAnchor;          // 范围选择起始锚点

    // 常量
    enum { HEADER_HEIGHT = 48, WEEKDAY_HEIGHT = 28, ROW_COUNT = 6,
           CELL_MARGIN = 2, NAV_BUTTON_SIZE = 28, TODAY_BUTTON_H = 24 };
};

#endif // FLUENTCALENDAR_H

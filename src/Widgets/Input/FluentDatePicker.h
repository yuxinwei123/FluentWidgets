#ifndef FLUENTDATEPICKER_H
#define FLUENTDATEPICKER_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QDate>
#include <QTimer>
class FluentCalendar;
class QToolButton;

// Fluent UI 风格日期选择器
// 支持: 弹出日历、日期格式、最小/最大可选日期、占位文本、主题适配

class FLUENT_EXPORT FluentDatePicker : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(QDate date READ date WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(QString format READ format WRITE setFormat NOTIFY formatChanged)
    Q_PROPERTY(QString placeholder READ placeholder WRITE setPlaceholder NOTIFY placeholderChanged)
    Q_PROPERTY(bool calendarPopup READ calendarPopup WRITE setCalendarPopup NOTIFY calendarPopupChanged)
    Q_PROPERTY(QDate minimumDate READ minimumDate WRITE setMinimumDate NOTIFY minimumDateChanged)
    Q_PROPERTY(QDate maximumDate READ maximumDate WRITE setMaximumDate NOTIFY maximumDateChanged)

public:
    explicit FluentDatePicker(QWidget* parent = nullptr);
    ~FluentDatePicker() override;

    // 日期
    QDate date() const;
    void setDate(const QDate& date);
    void clear();

    // 格式 (QDate::toString 格式)
    QString format() const;
    void setFormat(const QString& fmt);

    // 占位文本
    QString placeholder() const;
    void setPlaceholder(const QString& text);

    // 弹出日历
    bool calendarPopup() const;
    void setCalendarPopup(bool on);

    // 最小/最大可选日期
    QDate minimumDate() const;
    void setMinimumDate(const QDate& date);
    QDate maximumDate() const;
    void setMaximumDate(const QDate& date);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void dateChanged(const QDate& date);
    void formatChanged();
    void placeholderChanged();
    void calendarPopupChanged();
    void minimumDateChanged();
    void maximumDateChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void showPopup();
    void hidePopup();
    void updateCalendarTheme();

    QDate m_date;
    QString m_format = u8"yyyy-MM-dd";
    QString m_placeholder;
    bool m_calendarPopup = true;
    QDate m_minDate;
    QDate m_maxDate;

    // 弹出日历
    QWidget* m_popup = nullptr;
    FluentCalendar* m_calendar = nullptr;
    bool m_popupVisible = false;

    // 布局
    QRect iconRect() const;
    QRect textRect() const;

    enum { HEIGHT = 32, ICON_SIZE = 16, LEFT_PADDING = 10, RIGHT_PADDING = 32 };
};

#endif // FLUENTDATEPICKER_H

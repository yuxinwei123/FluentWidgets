#ifndef FLUENTTIMEPICKER_H
#define FLUENTTIMEPICKER_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QTime>
#include <QTimer>
// Fluent UI 风格时间选择器
// 支持: 弹出滚轮面板、12/24小时制、时/分/秒选择、主题适配

class FLUENT_EXPORT FluentTimePicker : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(QTime time READ time WRITE setTime NOTIFY timeChanged)
    Q_PROPERTY(bool is24Hour READ is24Hour WRITE setIs24Hour NOTIFY is24HourChanged)
    Q_PROPERTY(bool showSeconds READ showSeconds WRITE setShowSeconds NOTIFY showSecondsChanged)
    Q_PROPERTY(QString placeholder READ placeholder WRITE setPlaceholder NOTIFY placeholderChanged)
    Q_PROPERTY(QString format READ format NOTIFY formatChanged)

public:
    explicit FluentTimePicker(QWidget* parent = nullptr);
    ~FluentTimePicker() override;

    // 时间
    QTime time() const;
    void setTime(const QTime& time);
    void clear();

    // 12/24小时制
    bool is24Hour() const;
    void setIs24Hour(bool on);

    // 显示秒
    bool showSeconds() const;
    void setShowSeconds(bool on);

    // 占位文本
    QString placeholder() const;
    void setPlaceholder(const QString& text);

    // 格式字符串
    QString format() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void timeChanged(const QTime& time);
    void is24HourChanged();
    void showSecondsChanged();
    void placeholderChanged();
    void formatChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void showPopup();
    void hidePopup();
    void updatePopupTheme();
    QString formatTime() const;

    QTime m_time;
    bool m_is24Hour = true;
    bool m_showSeconds = false;
    QString m_placeholder;
    bool m_hasTime = false;

    // 弹出面板
    QWidget* m_popup = nullptr;
    class WheelColumn* m_hourWheel = nullptr;
    class WheelColumn* m_minuteWheel = nullptr;
    class WheelColumn* m_secondWheel = nullptr;
    class WheelColumn* m_ampmWheel = nullptr;
    bool m_popupVisible = false;

    // 布局
    QRect iconRect() const;
    QRect textRect() const;

    enum { HEIGHT = 32, ICON_SIZE = 16, LEFT_PADDING = 10, RIGHT_PADDING = 32 };
};

// 内部滚轮列控件
class WheelColumn : public QWidget {
    Q_OBJECT
public:
    explicit WheelColumn(QWidget* parent = nullptr);

    void setRange(int min, int max);
    int value() const;
    void setValue(int val, bool emitSignal = true);

    void setLabels(const QStringList& labels);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    void valueChanged(int val);

private:
    void scrollTo(int val);
    int m_min = 0;
    int m_max = 23;
    int m_value = 0;
    QStringList m_labels;
    int m_itemHeight = 32;
    int m_visibleItems = 5;

    // 拖拽
    bool m_dragging = false;
    int m_dragStartY = 0;
    int m_dragDelta = 0;
    int m_dragStartValue = 0;
};

#endif // FLUENTTIMEPICKER_H

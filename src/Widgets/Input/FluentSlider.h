#ifndef FLUENTSLIDER_H
#define FLUENTSLIDER_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"

class QPropertyAnimation;

// Fluent UI 风格滑块控件
// 支持: 水平/垂直、单值/范围、刻度、气泡提示、标签、主题适配、动画

class FLUENT_EXPORT FluentSlider : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(qreal minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(qreal maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(qreal value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(qreal secondValue READ secondValue WRITE setSecondValue NOTIFY secondValueChanged)
    Q_PROPERTY(qreal singleStep READ singleStep WRITE setSingleStep NOTIFY singleStepChanged)
    Q_PROPERTY(qreal pageStep READ pageStep WRITE setPageStep NOTIFY pageStepChanged)
    Q_PROPERTY(bool rangeMode READ rangeMode WRITE setRangeMode NOTIFY rangeModeChanged)
    Q_PROPERTY(bool showTicks READ showTicks WRITE setShowTicks NOTIFY showTicksChanged)
    Q_PROPERTY(bool showValueLabel READ showValueLabel WRITE setShowValueLabel NOTIFY showValueLabelChanged)
    Q_PROPERTY(bool invertedAppearance READ invertedAppearance WRITE setInvertedAppearance NOTIFY invertedAppearanceChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(qreal thumbProgress READ thumbProgress WRITE setThumbProgress NOTIFY thumbProgressChanged)

public:
    enum TickPosition {
        NoTicks,       // 无刻度
        TicksAbove,    // 上方/左侧
        TicksBelow,    // 下方/右侧
        TicksBothSides // 双侧
    };
    Q_ENUM(TickPosition)

    explicit FluentSlider(Qt::Orientation orient = Qt::Horizontal, QWidget* parent = nullptr);
    ~FluentSlider() override;

    // 方向
    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orient);

    // 范围
    qreal minimum() const;
    void setMinimum(qreal min);
    qreal maximum() const;
    void setMaximum(qreal max);
    void setRange(qreal min, qreal max);

    // 值
    qreal value() const;
    void setValue(qreal val);
    qreal secondValue() const;
    void setSecondValue(qreal val);
    qreal firstValue() const;  // alias for value()

    // 步进
    qreal singleStep() const;
    void setSingleStep(qreal step);
    qreal pageStep() const;
    void setPageStep(qreal step);

    // 范围模式
    bool rangeMode() const;
    void setRangeMode(bool on);

    // 刻度
    bool showTicks() const;
    void setShowTicks(bool on);
    TickPosition tickPosition() const;
    void setTickPosition(TickPosition pos);
    int tickInterval() const;
    void setTickInterval(int interval);

    // 值标签
    bool showValueLabel() const;
    void setShowValueLabel(bool on);

    // 反转
    bool invertedAppearance() const;
    void setInvertedAppearance(bool inv);

    // 标签
    QString label() const;
    void setLabel(const QString& text);

    // 滑块动画进度
    qreal thumbProgress() const;
    void setThumbProgress(qreal p);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void orientationChanged();
    void minimumChanged();
    void maximumChanged();
    void valueChanged(qreal val);
    void secondValueChanged(qreal val);
    void singleStepChanged();
    void pageStepChanged();
    void rangeModeChanged();
    void showTicksChanged();
    void showValueLabelChanged();
    void invertedAppearanceChanged();
    void labelChanged();
    void thumbProgressChanged();
    void sliderPressed();
    void sliderReleased();
    void rangeChanged(qreal first, qreal second);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    // 布局计算
    QRectF trackRect() const;
    QRectF firstThumbRect() const;
    QRectF secondThumbRect() const;
    QRectF activeTrackRect() const;
    QRectF valueLabelRect() const;
    qreal valueToPos(qreal val) const;
    qreal posToValue(qreal pos) const;
    qreal clampedValue(qreal val) const;
    int effectiveTickInterval() const;

    // 交互
    enum DragTarget { None, FirstThumb, SecondThumb, Track };
    DragTarget m_dragTarget = None;

    // 属性
    Qt::Orientation m_orientation = Qt::Horizontal;
    qreal m_minimum = 0.0;
    qreal m_maximum = 100.0;
    qreal m_value = 0.0;       // first (or single) value
    qreal m_secondValue = 100.0;
    qreal m_singleStep = 1.0;
    qreal m_pageStep = 10.0;
    bool m_rangeMode = false;
    bool m_showTicks = false;
    TickPosition m_tickPosition = TicksBelow;
    int m_tickInterval = 0;    // 0 = auto
    bool m_showValueLabel = true;
    bool m_invertedAppearance = false;
    QString m_label;
    qreal m_thumbProgress = 0.0;  // 动画用

    // 悬停
    DragTarget m_hoverTarget = None;

    // 常量
    enum {
        TRACK_HEIGHT = 4,
        TRACK_HOVER_HEIGHT = 8,
        THUMB_SIZE = 20,
        THUMB_SMALL = 8,
        LABEL_SPACING = 6,
        TICK_LENGTH = 6,
        MIN_SLIDER_LENGTH = 50
    };
};

#endif // FLUENTSLIDER_H

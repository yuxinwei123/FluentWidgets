#ifndef FLUENTPROGRESSBAR_H
#define FLUENTPROGRESSBAR_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QTimerEvent>
// Fluent UI 风格进度条控件
// 支持: 确定进度/不确定模式、条纹动画、多种样式(线形/环形)、主题适配

class FLUENT_EXPORT FluentProgressBar : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(bool indeterminate READ indeterminate WRITE setIndeterminate NOTIFY indeterminateChanged)
    Q_PROPERTY(BarStyle barStyle READ barStyle WRITE setBarStyle NOTIFY barStyleChanged)
    Q_PROPERTY(bool striped READ striped WRITE setStriped NOTIFY stripedChanged)
    Q_PROPERTY(bool stripedAnimated READ stripedAnimated WRITE setStripedAnimated NOTIFY stripedAnimatedChanged)
    Q_PROPERTY(QColor customColor READ customColor WRITE setCustomColor NOTIFY customColorChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(bool showValue READ showValue WRITE setShowValue NOTIFY showValueChanged)

public:
    // 进度条样式
    enum BarStyle {
        LineBar,        // 线形进度条
        RingBar         // 环形进度条
    };
    Q_ENUM(BarStyle)

    explicit FluentProgressBar(QWidget* parent = nullptr);
    ~FluentProgressBar() override;

    // 值与范围
    int value() const;
    void setValue(int val);
    int maximum() const;
    void setMaximum(int max);
    int minimum() const;
    void setMinimum(int min);
    void setRange(int min, int max);
    qreal percentage() const;

    // 不确定模式
    bool indeterminate() const;
    void setIndeterminate(bool on);

    // 样式
    BarStyle barStyle() const;
    void setBarStyle(BarStyle style);

    // 条纹
    bool striped() const;
    void setStriped(bool on);
    bool stripedAnimated() const;
    void setStripedAnimated(bool on);

    // 自定义颜色（覆盖主题色）
    QColor customColor() const;
    void setCustomColor(const QColor& color);

    // 标签文字
    QString label() const;
    void setLabel(const QString& text);

    // 显示数值
    bool showValue() const;
    void setShowValue(bool on);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void valueChanged(int val);
    void maximumChanged();
    void minimumChanged();
    void indeterminateChanged();
    void barStyleChanged();
    void stripedChanged();
    void stripedAnimatedChanged();
    void customColorChanged();
    void labelChanged();
    void showValueChanged();

protected:
    void timerEvent(QTimerEvent* event) override;

private:
    void drawLineBar(QPainter* painter);
    void drawRingBar(QPainter* painter);
    QColor progressColor() const;

    int m_value = 0;
    int m_minimum = 0;
    int m_maximum = 100;
    bool m_indeterminate = false;
    BarStyle m_barStyle = LineBar;
    bool m_striped = false;
    bool m_stripedAnimated = false;
    QColor m_customColor;
    QString m_label;
    bool m_showValue = false;

    // 不确定模式动画
    qreal m_indeterminateOffset = 0.0;
    int m_animTimerId = 0;

    // 条纹动画
    int m_stripeOffset = 0;
    int m_stripeTimerId = 0;

    // 常量
    enum { LINE_BAR_HEIGHT = 4, LINE_BAR_RADIUS = 2, RING_SIZE = 80,
           RING_STROKE_WIDTH = 6, INDETERMINATE_SPEED = 2, STRIPE_SPEED = 1,
           STRIPE_WIDTH = 12, ANIM_FPS = 60 };
};

#endif // FLUENTPROGRESSBAR_H

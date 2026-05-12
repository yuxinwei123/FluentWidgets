#ifndef FLUENTPROGRESSRING_H
#define FLUENTPROGRESSRING_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QConicalGradient>
#include <QTimerEvent>
// Fluent UI 风格环形进度指示器
// 支持: 确定/不确定模式、自定义线宽、内嵌文字、主题适配、渐变色

class FLUENT_EXPORT FluentProgressRing : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(bool indeterminate READ indeterminate WRITE setIndeterminate NOTIFY indeterminateChanged)
    Q_PROPERTY(int strokeWidth READ strokeWidth WRITE setStrokeWidth NOTIFY strokeWidthChanged)
    Q_PROPERTY(QColor customColor READ customColor WRITE setCustomColor NOTIFY customColorChanged)
    Q_PROPERTY(bool showValue READ showValue WRITE setShowValue NOTIFY showValueChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(bool useGradient READ useGradient WRITE setUseGradient NOTIFY useGradientChanged)

public:
    explicit FluentProgressRing(QWidget* parent = nullptr);
    ~FluentProgressRing() override;

    // 值与范围
    int value() const;
    void setValue(int val);
    int maximum() const;
    void setMaximum(int max);
    int minimum() const;
    void setMinimum(int min);
    void setRange(int min, int max);
    qreal percentage() const;

    // 不确定模式（旋转动画）
    bool indeterminate() const;
    void setIndeterminate(bool on);

    // 线宽
    int strokeWidth() const;
    void setStrokeWidth(int width);

    // 自定义颜色
    QColor customColor() const;
    void setCustomColor(const QColor& color);

    // 显示数值
    bool showValue() const;
    void setShowValue(bool on);

    // 标签文字
    QString label() const;
    void setLabel(const QString& text);

    // 渐变色
    bool useGradient() const;
    void setUseGradient(bool on);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void valueChanged(int val);
    void maximumChanged();
    void minimumChanged();
    void indeterminateChanged();
    void strokeWidthChanged();
    void customColorChanged();
    void showValueChanged();
    void labelChanged();
    void useGradientChanged();

protected:
    void timerEvent(QTimerEvent* event) override;

private:
    QColor progressColor() const;

    int m_value = 0;
    int m_minimum = 0;
    int m_maximum = 100;
    bool m_indeterminate = false;
    int m_strokeWidth = 6;
    QColor m_customColor;
    bool m_showValue = false;
    QString m_label;
    bool m_useGradient = false;

    // 不确定动画
    qreal m_rotationAngle = 0.0;
    int m_timerId = 0;

    // 常量
    enum { DEFAULT_SIZE = 80, MIN_SIZE = 32, ANIM_FPS = 60 };
};

#endif // FLUENTPROGRESSRING_H

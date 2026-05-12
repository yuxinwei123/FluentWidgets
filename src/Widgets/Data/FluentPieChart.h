#ifndef FLUENTPIECHART_H
#define FLUENTPIECHART_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QTimerEvent>

// Fluent UI 风格饼图控件
// 支持: 饼图/环形图、悬停突出、图例、点击选中、动画、标签

class FLUENT_EXPORT FluentPieChart : public FluentWidget {
    Q_OBJECT

    Q_PROPERTY(PieStyle pieStyle READ pieStyle WRITE setPieStyle NOTIFY pieStyleChanged)
    Q_PROPERTY(bool showLegend READ showLegend WRITE setShowLegend NOTIFY showLegendChanged)
    Q_PROPERTY(bool showLabels READ showLabels WRITE setShowLabels NOTIFY showLabelsChanged)
    Q_PROPERTY(bool showPercent READ showPercent WRITE setShowPercent NOTIFY showPercentChanged)
    Q_PROPERTY(bool showTooltip READ showTooltip WRITE setShowTooltip NOTIFY showTooltipChanged)
    Q_PROPERTY(bool animate READ animate WRITE setAnimate NOTIFY animateChanged)
    Q_PROPERTY(qreal holeRatio READ holeRatio WRITE setHoleRatio NOTIFY holeRatioChanged)
    Q_PROPERTY(qreal explodeDistance READ explodeDistance WRITE setExplodeDistance NOTIFY explodeDistanceChanged)

public:
    // 饼图样式
    enum PieStyle {
        Pie,       // 实心饼图
        Donut      // 环形图（甜甜圈）
    };
    Q_ENUM(PieStyle)

    // 数据切片
    struct Slice {
        QString name;
        qreal value = 0.0;
        QColor color;    // 为空则自动分配
    };

    explicit FluentPieChart(QWidget* parent = nullptr);
    ~FluentPieChart() override;

    // 样式
    PieStyle pieStyle() const;
    void setPieStyle(PieStyle style);

    // 显示选项
    bool showLegend() const;
    void setShowLegend(bool on);
    bool showLabels() const;
    void setShowLabels(bool on);
    bool showPercent() const;
    void setShowPercent(bool on);
    bool showTooltip() const;
    void setShowTooltip(bool on);
    bool animate() const;
    void setAnimate(bool on);

    // 环形图内圈比例 (0~0.9)
    qreal holeRatio() const;
    void setHoleRatio(qreal ratio);

    // 悬停时切片弹出距离
    qreal explodeDistance() const;
    void setExplodeDistance(qreal dist);

    // 数据管理
    int addSlice(const Slice& slice);
    int addSlice(const QString& name, qreal value, const QColor& color = QColor());
    void removeSlice(int index);
    void clearSlices();
    int sliceCount() const;
    Slice slice(int index) const;
    void updateSlice(int index, const Slice& slice);

    // 选中
    int selectedSlice() const;
    void setSelectedSlice(int index);

    // 标题
    void setTitle(const QString& title);
    QString title() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void pieStyleChanged();
    void showLegendChanged();
    void showLabelsChanged();
    void showPercentChanged();
    void showTooltipChanged();
    void animateChanged();
    void holeRatioChanged();
    void explodeDistanceChanged();
    void sliceClicked(int index);
    void selectionChanged(int index);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void timerEvent(QTimerEvent* event) override;

private:
    void drawPie(QPainter* painter, const QRectF& pieRect);
    void drawLabels(QPainter* painter, const QRectF& pieRect);
    void drawLegend(QPainter* painter);
    void drawTooltip(QPainter* painter, const QRectF& pieRect);
    void drawCenterText(QPainter* painter, const QRectF& pieRect);

    int sliceAtAngle(qreal angle) const;
    int sliceAtPos(const QPoint& pos) const;
    int legendItemAt(const QPoint& pos) const;
    qreal totalValue() const;
    QColor sliceColor(int index) const;

    // 属性
    PieStyle m_pieStyle = Pie;
    bool m_showLegend = true;
    bool m_showLabels = true;
    bool m_showPercent = true;
    bool m_showTooltip = true;
    bool m_animate = true;
    qreal m_holeRatio = 0.55;
    qreal m_explodeDistance = 12;
    QString m_title;

    // 数据
    QVector<Slice> m_slices;

    // 悬停
    int m_hoverSlice = -1;
    int m_selectedSlice = -1;

    // 图例
    QVector<QRectF> m_legendRects;
    int m_legendHoverIdx = -1;

    // 动画
    qreal m_animProgress = 0.0;
    int m_animTimerId = 0;
    static constexpr int ANIM_DURATION = 700;
    static constexpr int ANIM_FPS = 60;

    // 内置调色板
    static constexpr int PALETTE_SIZE = 12;
};

#endif // FLUENTPIECHART_H

#ifndef FLUENTBARCHART_H
#define FLUENTBARCHART_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QTimerEvent>
// Fluent UI 风格柱状图控件
// 支持: 竖直/水平柱状图、分组/堆叠模式、多系列、坐标轴、网格、图例、悬停提示、动画、圆角柱体

class FLUENT_EXPORT FluentBarChart : public FluentWidget {
    Q_OBJECT

    // 图表方向
    Q_PROPERTY(Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    // 柱状模式
    Q_PROPERTY(BarMode barMode READ barMode WRITE setBarMode NOTIFY barModeChanged)
    // 显示选项
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid NOTIFY showGridChanged)
    Q_PROPERTY(bool showLegend READ showLegend WRITE setShowLegend NOTIFY showLegendChanged)
    Q_PROPERTY(bool showTooltip READ showTooltip WRITE setShowTooltip NOTIFY showTooltipChanged)
    Q_PROPERTY(bool showValues READ showValues WRITE setShowValues NOTIFY showValuesChanged)
    Q_PROPERTY(bool animate READ animate WRITE setAnimate NOTIFY animateChanged)
    Q_PROPERTY(int barRadius READ barRadius WRITE setBarRadius NOTIFY barRadiusChanged)
    Q_PROPERTY(qreal barWidth READ barWidth WRITE setBarWidth NOTIFY barWidthChanged)
    // 边距
    Q_PROPERTY(int chartMarginLeft READ chartMarginLeft WRITE setChartMarginLeft NOTIFY chartMarginLeftChanged)
    Q_PROPERTY(int chartMarginRight READ chartMarginRight WRITE setChartMarginRight NOTIFY chartMarginRightChanged)
    Q_PROPERTY(int chartMarginTop READ chartMarginTop WRITE setChartMarginTop NOTIFY chartMarginTopChanged)
    Q_PROPERTY(int chartMarginBottom READ chartMarginBottom WRITE setChartMarginBottom NOTIFY chartMarginBottomChanged)

public:
    // 图表方向
    enum Orientation {
        Vertical,      // 竖直柱状图（默认）
        Horizontal     // 水平柱状图
    };
    Q_ENUM(Orientation)

    // 柱状模式
    enum BarMode {
        Grouped,       // 分组并排
        Stacked        // 堆叠
    };
    Q_ENUM(BarMode)

    // 数据系列
    struct Series {
        QString name;
        QStringList categories;  // 类别标签
        QVector<qreal> values;   // 对应类别的值
        QColor color;            // 为空则自动分配
    };

    explicit FluentBarChart(QWidget* parent = nullptr);
    ~FluentBarChart() override;

    // 方向与模式
    Orientation orientation() const;
    void setOrientation(Orientation orient);
    BarMode barMode() const;
    void setBarMode(BarMode mode);

    // 显示选项
    bool showGrid() const;
    void setShowGrid(bool on);
    bool showLegend() const;
    void setShowLegend(bool on);
    bool showTooltip() const;
    void setShowTooltip(bool on);
    bool showValues() const;
    void setShowValues(bool on);
    bool animate() const;
    void setAnimate(bool on);
    int barRadius() const;
    void setBarRadius(int r);
    qreal barWidth() const;
    void setBarWidth(qreal w);

    // 边距
    int chartMarginLeft() const;
    void setChartMarginLeft(int m);
    int chartMarginRight() const;
    void setChartMarginRight(int m);
    int chartMarginTop() const;
    void setChartMarginTop(int m);
    int chartMarginBottom() const;
    void setChartMarginBottom(int m);

    // 数据管理
    int addSeries(const Series& series);
    void removeSeries(int index);
    void clearSeries();
    int seriesCount() const;
    Series series(int index) const;
    void updateSeries(int index, const Series& series);

    // 系列可见性
    bool isSeriesVisible(int index) const;
    void setSeriesVisible(int index, bool visible);
    void toggleSeriesVisible(int index);

    // 坐标轴范围
    void setAutoAxisRange(bool autoRange);
    void setValueAxisRange(qreal min, qreal max);

    // 坐标轴标题
    void setCategoryAxisTitle(const QString& title);
    void setValueAxisTitle(const QString& title);

    // 刻度数量（值轴）
    void setValueTickCount(int count);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void orientationChanged();
    void barModeChanged();
    void showGridChanged();
    void showLegendChanged();
    void showTooltipChanged();
    void showValuesChanged();
    void animateChanged();
    void barRadiusChanged();
    void barWidthChanged();
    void chartMarginLeftChanged();
    void chartMarginRightChanged();
    void chartMarginTopChanged();
    void chartMarginBottomChanged();
    void seriesChanged();

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void timerEvent(QTimerEvent* event) override;

private:
    // 绘制子模块
    void drawBackground(QPainter* painter, const QRectF& plotArea);
    void drawGrid(QPainter* painter, const QRectF& plotArea);
    void drawAxes(QPainter* painter, const QRectF& plotArea);
    void drawBars(QPainter* painter, const QRectF& plotArea);
    void drawTooltip(QPainter* painter, const QRectF& plotArea);
    void drawLegend(QPainter* painter);

    // 布局计算
    QRectF plotArea() const;
    void computeAxisRange();
    QColor seriesColor(int index) const;
    int categoryCount() const;
    QString categoryLabel(int index) const;

    // 柱体区域缓存（悬停/点击检测）
    void rebuildBarRects(const QRectF& plotArea);

    // 图例点击检测
    int legendItemAt(const QPoint& pos) const;

    // 属性
    Orientation m_orientation = Vertical;
    BarMode m_barMode = Grouped;
    bool m_showGrid = true;
    bool m_showLegend = true;
    bool m_showTooltip = true;
    bool m_showValues = false;
    bool m_animate = true;
    int m_barRadius = 4;
    qreal m_barWidth = 0.6;  // 柱体占类别宽度的比例 (0~1)
    int m_marginLeft = 56;
    int m_marginRight = 24;
    int m_marginTop = 24;
    int m_marginBottom = 48;

    // 数据
    QVector<Series> m_series;
    QVector<bool> m_seriesVisible;

    // 坐标轴
    bool m_autoAxisRange = true;
    qreal m_valueMin = 0, m_valueMax = 100;
    QString m_categoryAxisTitle;
    QString m_valueAxisTitle;
    int m_valueTickCount = 5;

    // 悬停
    bool m_hovered = false;
    int m_hoverSeriesIdx = -1;
    int m_hoverCategoryIdx = -1;

    // 图例
    QVector<QRectF> m_legendRects;
    int m_legendHoverIdx = -1;

    // 柱体区域缓存
    struct BarInfo { int seriesIdx; int categoryIdx; QRectF rect; };
    QVector<BarInfo> m_barRects;

    // 动画
    qreal m_animProgress = 0.0;
    int m_animTimerId = 0;
    static constexpr int ANIM_DURATION = 600;
    static constexpr int ANIM_FPS = 60;

    // 内置调色板
    static constexpr int PALETTE_SIZE = 8;
};

#endif // FLUENTBARCHART_H

#ifndef FLUENTLINECHART_H
#define FLUENTLINECHART_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QTimerEvent>
#include <QMap>
// Fluent UI 风格线图控件
// 支持: 折线、曲线、散点、面积填充、多系列、坐标轴、网格、图例、悬停提示、动画

class FLUENT_EXPORT FluentLineChart : public FluentWidget {
    Q_OBJECT

    // 图表类型
    Q_PROPERTY(ChartType chartType READ chartType WRITE setChartType NOTIFY chartTypeChanged)
    // 坐标轴
    Q_PROPERTY(AxisPosition xAxisPosition READ xAxisPosition WRITE setXAxisPosition NOTIFY xAxisPositionChanged)
    Q_PROPERTY(AxisPosition yAxisPosition READ yAxisPosition WRITE setYAxisPosition NOTIFY yAxisPositionChanged)
    // 显示选项
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid NOTIFY showGridChanged)
    Q_PROPERTY(bool showLegend READ showLegend WRITE setShowLegend NOTIFY showLegendChanged)
    Q_PROPERTY(bool showTooltip READ showTooltip WRITE setShowTooltip NOTIFY showTooltipChanged)
    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth NOTIFY smoothChanged)
    Q_PROPERTY(bool areaFill READ areaFill WRITE setAreaFill NOTIFY areaFillChanged)
    Q_PROPERTY(bool showDots READ showDots WRITE setShowDots NOTIFY showDotsChanged)
    Q_PROPERTY(bool animate READ animate WRITE setAnimate NOTIFY animateChanged)
    // 边距
    Q_PROPERTY(int chartMarginLeft READ chartMarginLeft WRITE setChartMarginLeft NOTIFY chartMarginLeftChanged)
    Q_PROPERTY(int chartMarginRight READ chartMarginRight WRITE setChartMarginRight NOTIFY chartMarginRightChanged)
    Q_PROPERTY(int chartMarginTop READ chartMarginTop WRITE setChartMarginTop NOTIFY chartMarginTopChanged)
    Q_PROPERTY(int chartMarginBottom READ chartMarginBottom WRITE setChartMarginBottom NOTIFY chartMarginBottomChanged)

public:
    // 图表类型
    enum ChartType {
        LineChart,       // 折线图
        SplineChart,     // 曲线图（样条插值）
        ScatterChart     // 散点图
    };
    Q_ENUM(ChartType)

    // 坐标轴位置
    enum AxisPosition {
        AxisLeft,        // Y轴在左侧
        AxisRight,       // Y轴在右侧
        AxisBottom,      // X轴在底部
        AxisTop          // X轴在顶部
    };
    Q_ENUM(AxisPosition)

    // 数据系列
    struct Series {
        QString name;
        QVector<QPointF> points;  // (x, y)
        QColor color;             // 为空则自动分配
        Qt::PenStyle lineStyle = Qt::SolidLine;
        qreal lineWidth = 2.0;
        int dotSize = 6;
        bool areaFill = false;    // 单系列面积填充覆盖
    };

    explicit FluentLineChart(QWidget* parent = nullptr);
    ~FluentLineChart() override;

    // 图表类型
    ChartType chartType() const;
    void setChartType(ChartType type);

    // 坐标轴
    AxisPosition xAxisPosition() const;
    void setXAxisPosition(AxisPosition pos);
    AxisPosition yAxisPosition() const;
    void setYAxisPosition(AxisPosition pos);

    // 显示选项
    bool showGrid() const;
    void setShowGrid(bool on);
    bool showLegend() const;
    void setShowLegend(bool on);
    bool showTooltip() const;
    void setShowTooltip(bool on);
    bool smooth() const;
    void setSmooth(bool on);
    bool areaFill() const;
    void setAreaFill(bool on);
    bool showDots() const;
    void setShowDots(bool on);
    bool animate() const;
    void setAnimate(bool on);

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

    // 坐标轴范围（自动或手动）
    void setAutoAxisRange(bool autoRange);
    void setXAxisRange(qreal min, qreal max);
    void setYAxisRange(qreal min, qreal max);

    // 坐标轴标题
    void setXAxisTitle(const QString& title);
    void setYAxisTitle(const QString& title);

    // 刻度数量
    void setXTickCount(int count);
    void setYTickCount(int count);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void chartTypeChanged();
    void xAxisPositionChanged();
    void yAxisPositionChanged();
    void showGridChanged();
    void showLegendChanged();
    void showTooltipChanged();
    void smoothChanged();
    void areaFillChanged();
    void showDotsChanged();
    void animateChanged();
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
    void drawSeries(QPainter* painter, const QRectF& plotArea);
    void drawSplineSeries(QPainter* painter, const QRectF& plotArea, const Series& s, const QColor& color);
    void drawLineSeries(QPainter* painter, const QRectF& plotArea, const Series& s, const QColor& color);
    void drawScatterSeries(QPainter* painter, const QRectF& plotArea, const Series& s, const QColor& color);
    void drawAreaFill(QPainter* painter, const QRectF& plotArea, const QVector<QPointF>& screenPoints, const QColor& color);
    void drawTooltip(QPainter* painter, const QRectF& plotArea);
    void drawLegend(QPainter* painter);

    // 坐标映射
    QPointF mapToScreen(const QPointF& dataPoint, const QRectF& plotArea) const;
    QPointF mapFromScreen(const QPointF& screenPoint, const QRectF& plotArea) const;
    QRectF plotArea() const;

    // 数据范围计算
    void computeAxisRange();

    // 颜色分配
    QColor seriesColor(int index) const;

    // 悬停检测
    int findNearestPoint(const QPoint& mousePos, int& seriesIdx) const;

    // 图例点击检测
    int legendItemAt(const QPoint& pos) const;

    // 属性
    ChartType m_chartType = LineChart;
    AxisPosition m_xAxisPos = AxisBottom;
    AxisPosition m_yAxisPos = AxisLeft;
    bool m_showGrid = true;
    bool m_showLegend = true;
    bool m_showTooltip = true;
    bool m_smooth = true;
    bool m_areaFill = false;
    bool m_showDots = true;
    bool m_animate = true;
    int m_marginLeft = 56;
    int m_marginRight = 24;
    int m_marginTop = 24;
    int m_marginBottom = 40;

    // 数据
    QVector<Series> m_series;
    QVector<bool> m_seriesVisible;  // 每个系列的可见性

    // 坐标轴
    bool m_autoAxisRange = true;
    qreal m_xMin = 0, m_xMax = 10;
    qreal m_yMin = 0, m_yMax = 100;
    QString m_xAxisTitle;
    QString m_yAxisTitle;
    int m_xTickCount = 6;
    int m_yTickCount = 5;

    // 悬停
    QPoint m_hoverPos;
    bool m_hovered = false;
    int m_hoverSeriesIdx = -1;
    int m_hoverPointIdx = -1;

    // 动画
    qreal m_animProgress = 0.0;
    int m_animTimerId = 0;
    static constexpr int ANIM_DURATION = 600;  // ms
    static constexpr int ANIM_FPS = 60;

    // 图例区域缓存（用于点击检测）
    QVector<QRectF> m_legendRects;

    // 图例悬停
    int m_legendHoverIdx = -1;

    // 内置调色板
    static constexpr int PALETTE_SIZE = 8;
};

#endif // FLUENTLINECHART_H

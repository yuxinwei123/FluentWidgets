               #include "FluentLineChart.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QtMath>
#include <algorithm>

// ============================================================
// FluentLineChart
// ============================================================

// 内置调色板（Fluent Design 风格色系）
static const QColor s_palette[] = {
    QColor(0, 120, 212),    // 蓝色 (Primary)
    QColor(16, 137, 108),   // 绿色 (Teal)
    QColor(218, 59, 59),    // 红色
    QColor(255, 140, 0),    // 橙色
    QColor(116, 77, 169),   // 紫色
    QColor(0, 153, 188),    // 青色
    QColor(230, 126, 34),   // 深橙
    QColor(142, 68, 173),   // 深紫
};

FluentLineChart::FluentLineChart(QWidget* parent)
    : FluentWidget(parent)
{
    setMinimumSize(300, 200);
    setMouseTracking(true);
    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

FluentLineChart::~FluentLineChart() {
    if (m_animTimerId) killTimer(m_animTimerId);
}

// ============================================================
// 属性
// ============================================================

FluentLineChart::ChartType FluentLineChart::chartType() const { return m_chartType; }
void FluentLineChart::setChartType(ChartType type) {
    if (m_chartType == type) return;
    m_chartType = type;
    update(); emit chartTypeChanged();
}

FluentLineChart::AxisPosition FluentLineChart::xAxisPosition() const { return m_xAxisPos; }
void FluentLineChart::setXAxisPosition(AxisPosition pos) { m_xAxisPos = pos; update(); emit xAxisPositionChanged(); }

FluentLineChart::AxisPosition FluentLineChart::yAxisPosition() const { return m_yAxisPos; }
void FluentLineChart::setYAxisPosition(AxisPosition pos) { m_yAxisPos = pos; update(); emit yAxisPositionChanged(); }

bool FluentLineChart::showGrid() const { return m_showGrid; }
void FluentLineChart::setShowGrid(bool on) { m_showGrid = on; update(); emit showGridChanged(); }

bool FluentLineChart::showLegend() const { return m_showLegend; }
void FluentLineChart::setShowLegend(bool on) { m_showLegend = on; update(); emit showLegendChanged(); }

bool FluentLineChart::showTooltip() const { return m_showTooltip; }
void FluentLineChart::setShowTooltip(bool on) { m_showTooltip = on; update(); emit showTooltipChanged(); }

bool FluentLineChart::smooth() const { return m_smooth; }
void FluentLineChart::setSmooth(bool on) { m_smooth = on; update(); emit smoothChanged(); }

bool FluentLineChart::areaFill() const { return m_areaFill; }
void FluentLineChart::setAreaFill(bool on) { m_areaFill = on; update(); emit areaFillChanged(); }

bool FluentLineChart::showDots() const { return m_showDots; }
void FluentLineChart::setShowDots(bool on) { m_showDots = on; update(); emit showDotsChanged(); }

bool FluentLineChart::animate() const { return m_animate; }
void FluentLineChart::setAnimate(bool on) { m_animate = on; emit animateChanged(); }

int FluentLineChart::chartMarginLeft() const { return m_marginLeft; }
void FluentLineChart::setChartMarginLeft(int m) { m_marginLeft = m; update(); emit chartMarginLeftChanged(); }
int FluentLineChart::chartMarginRight() const { return m_marginRight; }
void FluentLineChart::setChartMarginRight(int m) { m_marginRight = m; update(); emit chartMarginRightChanged(); }
int FluentLineChart::chartMarginTop() const { return m_marginTop; }
void FluentLineChart::setChartMarginTop(int m) { m_marginTop = m; update(); emit chartMarginTopChanged(); }
int FluentLineChart::chartMarginBottom() const { return m_marginBottom; }
void FluentLineChart::setChartMarginBottom(int m) { m_marginBottom = m; update(); emit chartMarginBottomChanged(); }

// ============================================================
// 数据管理
// ============================================================

int FluentLineChart::addSeries(const Series& series) {
    m_series.append(series);
    m_seriesVisible.append(true);
    computeAxisRange();
    if (m_animate) {
        m_animProgress = 0.0;
        if (m_animTimerId) killTimer(m_animTimerId);
        m_animTimerId = startTimer(1000 / ANIM_FPS);
    }
    update();
    emit seriesChanged();
    return m_series.size() - 1;
}

void FluentLineChart::removeSeries(int index) {
    if (index < 0 || index >= m_series.size()) return;
    m_series.removeAt(index);
    m_seriesVisible.removeAt(index);
    computeAxisRange();
    update();
    emit seriesChanged();
}

void FluentLineChart::clearSeries() {
    m_series.clear();
    m_seriesVisible.clear();
    m_hovered = false;
    m_hoverSeriesIdx = -1;
    m_hoverPointIdx = -1;
    computeAxisRange();
    update();
    emit seriesChanged();
}

int FluentLineChart::seriesCount() const { return m_series.size(); }

FluentLineChart::Series FluentLineChart::series(int index) const {
    if (index < 0 || index >= m_series.size()) return Series();
    return m_series[index];
}

void FluentLineChart::updateSeries(int index, const Series& series) {
    if (index < 0 || index >= m_series.size()) return;
    m_series[index] = series;
    computeAxisRange();
    update();
    emit seriesChanged();
}

bool FluentLineChart::isSeriesVisible(int index) const {
    if (index < 0 || index >= m_seriesVisible.size()) return false;
    return m_seriesVisible[index];
}

void FluentLineChart::setSeriesVisible(int index, bool visible) {
    if (index < 0 || index >= m_seriesVisible.size()) return;
    if (m_seriesVisible[index] == visible) return;
    m_seriesVisible[index] = visible;
    computeAxisRange();
    update();
}

void FluentLineChart::toggleSeriesVisible(int index) {
    if (index < 0 || index >= m_seriesVisible.size()) return;
    m_seriesVisible[index] = !m_seriesVisible[index];
    computeAxisRange();
    update();
}

void FluentLineChart::setAutoAxisRange(bool autoRange) {
    m_autoAxisRange = autoRange;
    if (autoRange) computeAxisRange();
    update();
}

void FluentLineChart::setXAxisRange(qreal min, qreal max) {
    m_autoAxisRange = false;
    m_xMin = min; m_xMax = max;
    update();
}

void FluentLineChart::setYAxisRange(qreal min, qreal max) {
    m_autoAxisRange = false;
    m_yMin = min; m_yMax = max;
    update();
}

void FluentLineChart::setXAxisTitle(const QString& title) { m_xAxisTitle = title; update(); }
void FluentLineChart::setYAxisTitle(const QString& title) { m_yAxisTitle = title; update(); }

void FluentLineChart::setXTickCount(int count) { m_xTickCount = qMax(2, count); update(); }
void FluentLineChart::setYTickCount(int count) { m_yTickCount = qMax(2, count); update(); }

QSize FluentLineChart::sizeHint() const { return QSize(600, 400); }
QSize FluentLineChart::minimumSizeHint() const { return QSize(300, 200); }

// ============================================================
// 坐标映射与范围
// ============================================================

QRectF FluentLineChart::plotArea() const {
    int left = m_marginLeft;
    int right = m_marginRight;
    int top = m_marginTop;
    int bottom = m_marginBottom;

    // 图例空间
    if (m_showLegend && !m_series.isEmpty()) {
        top += 28;
    }
    return QRectF(left, top, width() - left - right, height() - top - bottom);
}

QPointF FluentLineChart::mapToScreen(const QPointF& dataPoint, const QRectF& pa) const {
    qreal xRange = m_xMax - m_xMin;
    qreal yRange = m_yMax - m_yMin;
    if (xRange <= 0) xRange = 1.0;
    if (yRange <= 0) yRange = 1.0;

    qreal sx = pa.left() + (dataPoint.x() - m_xMin) / xRange * pa.width();
    qreal sy = pa.bottom() - (dataPoint.y() - m_yMin) / yRange * pa.height();
    return QPointF(sx, sy);
}

QPointF FluentLineChart::mapFromScreen(const QPointF& screenPoint, const QRectF& pa) const {
    qreal xRange = m_xMax - m_xMin;
    qreal yRange = m_yMax - m_yMin;
    if (xRange <= 0) xRange = 1.0;
    if (yRange <= 0) yRange = 1.0;

    qreal dx = m_xMin + (screenPoint.x() - pa.left()) / pa.width() * xRange;
    qreal dy = m_yMin + (pa.bottom() - screenPoint.y()) / pa.height() * yRange;
    return QPointF(dx, dy);
}

void FluentLineChart::computeAxisRange() {
    if (!m_autoAxisRange) return;

    bool hasData = false;
    qreal xMin = 1e18, xMax = -1e18, yMin = 1e18, yMax = -1e18;

    for (int i = 0; i < m_series.size(); ++i) {
        if (i < m_seriesVisible.size() && !m_seriesVisible[i]) continue;
        for (const auto& p : m_series[i].points) {
            hasData = true;
            if (p.x() < xMin) xMin = p.x();
            if (p.x() > xMax) xMax = p.x();
            if (p.y() < yMin) yMin = p.y();
            if (p.y() > yMax) yMax = p.y();
        }
    }

    if (!hasData) {
        m_xMin = 0; m_xMax = 10;
        m_yMin = 0; m_yMax = 100;
        return;
    }

    // 添加 5% 的边距
    qreal xPad = (xMax - xMin) * 0.05;
    qreal yPad = (yMax - yMin) * 0.08;
    if (xPad < 0.5) xPad = 0.5;
    if (yPad < 1.0) yPad = 1.0;

    m_xMin = xMin - xPad;
    m_xMax = xMax + xPad;
    m_yMin = qMin(yMin - yPad, 0.0);  // Y轴默认从0开始
    m_yMax = yMax + yPad;

    // 保证范围有效
    if (m_xMax <= m_xMin) m_xMax = m_xMin + 1;
    if (m_yMax <= m_yMin) m_yMax = m_yMin + 1;
}

QColor FluentLineChart::seriesColor(int index) const {
    if (index >= 0 && index < m_series.size() && m_series[index].color.isValid())
        return m_series[index].color;
    return s_palette[index % PALETTE_SIZE];
}

// ============================================================
// 绘制
// ============================================================

void FluentLineChart::paintFluent(QPainter* painter) {
    painter->setRenderHint(QPainter::Antialiasing);

    QRectF pa = plotArea();

    drawBackground(painter, pa);
    drawGrid(painter, pa);
    drawAxes(painter, pa);
    drawSeries(painter, pa);
    drawTooltip(painter, pa);
    if (m_showLegend) drawLegend(painter);
}

void FluentLineChart::drawBackground(QPainter* painter, const QRectF& pa) {
    auto* t = theme();
    // 绘图区域背景
    painter->setPen(Qt::NoPen);
    painter->setBrush(t->cardColor());
    painter->drawRoundedRect(pa.adjusted(-1, -1, 1, 1), 4, 4);
}

void FluentLineChart::drawGrid(QPainter* painter, const QRectF& pa) {
    if (!m_showGrid) return;

    auto* t = theme();
    QColor gridColor = t->dividerColor();
    gridColor.setAlphaF(0.5);
    QPen gridPen(gridColor, 1, Qt::DashLine);
    painter->setPen(gridPen);

    // Y轴网格线
    qreal yRange = m_yMax - m_yMin;
    if (yRange <= 0) return;
    for (int i = 0; i <= m_yTickCount; ++i) {
        qreal val = m_yMin + yRange * i / m_yTickCount;
        QPointF pt = mapToScreen(QPointF(0, val), pa);
        painter->drawLine(QPointF(pa.left(), pt.y()), QPointF(pa.right(), pt.y()));
    }

    // X轴网格线
    qreal xRange = m_xMax - m_xMin;
    if (xRange <= 0) return;
    for (int i = 0; i <= m_xTickCount; ++i) {
        qreal val = m_xMin + xRange * i / m_xTickCount;
        QPointF pt = mapToScreen(QPointF(val, 0), pa);
        painter->drawLine(QPointF(pt.x(), pa.top()), QPointF(pt.x(), pa.bottom()));
    }
}

void FluentLineChart::drawAxes(QPainter* painter, const QRectF& pa) {
    auto* t = theme();
    painter->setPen(QPen(t->textColorSecondary(), 1));
    painter->setFont(t->captionFont());

    // 坐标轴线
    painter->setPen(QPen(t->dividerColor(), 1, Qt::SolidLine));
    // X 轴
    painter->drawLine(QPointF(pa.left(), pa.bottom()), QPointF(pa.right(), pa.bottom()));
    // Y 轴
    painter->drawLine(QPointF(pa.left(), pa.top()), QPointF(pa.left(), pa.bottom()));

    painter->setPen(t->textColorSecondary());

    // Y轴刻度标签
    qreal yRange = m_yMax - m_yMin;
    if (yRange > 0) {
        for (int i = 0; i <= m_yTickCount; ++i) {
            qreal val = m_yMin + yRange * i / m_yTickCount;
            QPointF pt = mapToScreen(QPointF(0, val), pa);
            QString label = QString::number(val, 'g', 4);
            QRectF labelRect(pa.left() - m_marginLeft, pt.y() - 8, m_marginLeft - 6, 16);
            painter->drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, label);
        }
    }

    // X轴刻度标签
    qreal xRange = m_xMax - m_xMin;
    if (xRange > 0) {
        for (int i = 0; i <= m_xTickCount; ++i) {
            qreal val = m_xMin + xRange * i / m_xTickCount;
            QPointF pt = mapToScreen(QPointF(val, 0), pa);
            QString label = QString::number(val, 'g', 4);
            QRectF labelRect(pt.x() - 40, pa.bottom() + 4, 80, 16);
            painter->drawText(labelRect, Qt::AlignCenter | Qt::AlignTop, label);
        }
    }

    // 轴标题
    if (!m_xAxisTitle.isEmpty()) {
        painter->setFont(t->captionFont());
        QRectF titleRect(pa.left(), height() - 16, pa.width(), 14);
        painter->drawText(titleRect, Qt::AlignCenter, m_xAxisTitle);
    }
    if (!m_yAxisTitle.isEmpty()) {
        painter->save();
        painter->translate(12, pa.top() + pa.height() / 2);
        painter->rotate(-90);
        painter->drawText(QRectF(-60, -7, 120, 14), Qt::AlignCenter, m_yAxisTitle);
        painter->restore();
    }
}

void FluentLineChart::drawSeries(QPainter* painter, const QRectF& pa) {
    for (int i = 0; i < m_series.size(); ++i) {
        if (i < m_seriesVisible.size() && !m_seriesVisible[i]) continue;
        const auto& s = m_series[i];
        if (s.points.isEmpty()) continue;
        QColor color = seriesColor(i);

        switch (m_chartType) {
        case ScatterChart:
            drawScatterSeries(painter, pa, s, color);
            break;
        case SplineChart:
            drawSplineSeries(painter, pa, s, color);
            break;
        case LineChart:
        default:
            drawLineSeries(painter, pa, s, color);
            break;
        }
    }
}

void FluentLineChart::drawLineSeries(QPainter* painter, const QRectF& pa,
                                      const Series& s, const QColor& color) {
    // 计算屏幕坐标点（考虑动画进度）
    QVector<QPointF> screenPts;
    int totalPts = s.points.size();
    int visiblePts = m_animate ? qMax(1, qRound(totalPts * m_animProgress)) : totalPts;

    for (int i = 0; i < visiblePts; ++i) {
        screenPts.append(mapToScreen(s.points[i], pa));
    }
    if (screenPts.isEmpty()) return;

    // 面积填充
    bool fill = s.areaFill || m_areaFill;
    if (fill && m_chartType != ScatterChart) {
        drawAreaFill(painter, pa, screenPts, color);
    }

    // 折线
    if (m_chartType != ScatterChart) {
        QPainterPath path;
        if (m_smooth && screenPts.size() >= 3) {
            path.moveTo(screenPts[0]);
            for (int i = 0; i < screenPts.size() - 1; ++i) {
                QPointF p0 = (i > 0) ? screenPts[i - 1] : screenPts[i];
                QPointF p1 = screenPts[i];
                QPointF p2 = screenPts[i + 1];
                QPointF p3 = (i + 2 < screenPts.size()) ? screenPts[i + 2] : p2;

                qreal tension = 0.3;
                QPointF cp1 = p1 + (p2 - p0) * tension;
                QPointF cp2 = p2 - (p3 - p1) * tension;
                path.cubicTo(cp1, cp2, p2);
            }
        } else {
            path.moveTo(screenPts[0]);
            for (int i = 1; i < screenPts.size(); ++i) {
                path.lineTo(screenPts[i]);
            }
        }

        QPen pen(color, s.lineWidth, s.lineStyle, Qt::RoundCap, Qt::RoundJoin);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(path);
    }

    // 数据点
    if (m_showDots || m_chartType == ScatterChart) {
        for (int i = 0; i < screenPts.size(); ++i) {
            bool isHovered = (m_hovered && m_hoverSeriesIdx >= 0 &&
                              &s == &m_series[m_hoverSeriesIdx] &&
                              i == m_hoverPointIdx);
            int dotR = isHovered ? s.dotSize + 3 : s.dotSize;

            // 光晕效果
            if (isHovered) {
                QColor glowColor = color;
                glowColor.setAlphaF(0.25);
                painter->setPen(Qt::NoPen);
                painter->setBrush(glowColor);
                painter->drawEllipse(screenPts[i], dotR + 4, dotR + 4);
            }

            painter->setPen(QPen(Qt::white, 2));
            painter->setBrush(color);
            painter->drawEllipse(screenPts[i], dotR / 2.0, dotR / 2.0);
        }
    }
}

void FluentLineChart::drawSplineSeries(QPainter* painter, const QRectF& pa,
                                        const Series& s, const QColor& color) {
    // SplineChart 和 LineChart 的平滑曲线逻辑一致，直接复用
    drawLineSeries(painter, pa, s, color);
}

void FluentLineChart::drawScatterSeries(QPainter* painter, const QRectF& pa,
                                         const Series& s, const QColor& color) {
    int totalPts = s.points.size();
    int visiblePts = m_animate ? qMax(1, qRound(totalPts * m_animProgress)) : totalPts;

    for (int i = 0; i < visiblePts; ++i) {
        QPointF pt = mapToScreen(s.points[i], pa);
        bool isHovered = (m_hovered && m_hoverSeriesIdx >= 0 &&
                          &s == &m_series[m_hoverSeriesIdx] &&
                          i == m_hoverPointIdx);
        int dotR = isHovered ? s.dotSize + 4 : s.dotSize;

        // 光晕
        if (isHovered) {
            QColor glowColor = color;
            glowColor.setAlphaF(0.2);
            painter->setPen(Qt::NoPen);
            painter->setBrush(glowColor);
            painter->drawEllipse(pt, dotR + 6, dotR + 6);
        }

        // 外圈
        painter->setPen(QPen(color, 2));
        painter->setBrush(color.lighter(140));
        painter->drawEllipse(pt, dotR / 2.0, dotR / 2.0);
    }
}

void FluentLineChart::drawAreaFill(QPainter* painter, const QRectF& pa,
                                    const QVector<QPointF>& screenPoints, const QColor& color) {
    if (screenPoints.size() < 2) return;

    QPainterPath fillPath;

    if (m_smooth && screenPoints.size() >= 3) {
        fillPath.moveTo(screenPoints[0]);
        for (int i = 0; i < screenPoints.size() - 1; ++i) {
            QPointF p0 = (i > 0) ? screenPoints[i - 1] : screenPoints[i];
            QPointF p1 = screenPoints[i];
            QPointF p2 = screenPoints[i + 1];
            QPointF p3 = (i + 2 < screenPoints.size()) ? screenPoints[i + 2] : p2;
            qreal tension = 0.3;
            QPointF cp1 = p1 + (p2 - p0) * tension;
            QPointF cp2 = p2 - (p3 - p1) * tension;
            fillPath.cubicTo(cp1, cp2, p2);
        }
    } else {
        fillPath.moveTo(screenPoints[0]);
        for (int i = 1; i < screenPoints.size(); ++i) {
            fillPath.lineTo(screenPoints[i]);
        }
    }

    // 闭合到底部
    fillPath.lineTo(screenPoints.last().x(), pa.bottom());
    fillPath.lineTo(screenPoints.first().x(), pa.bottom());
    fillPath.closeSubpath();

    // 渐变填充
    QLinearGradient grad(0, pa.top(), 0, pa.bottom());
    QColor topColor = color;
    topColor.setAlphaF(0.35);
    QColor bottomColor = color;
    bottomColor.setAlphaF(0.03);
    grad.setColorAt(0.0, topColor);
    grad.setColorAt(1.0, bottomColor);

    painter->setPen(Qt::NoPen);
    painter->setBrush(grad);
    painter->drawPath(fillPath);
}

void FluentLineChart::drawTooltip(QPainter* painter, const QRectF& pa) {
    if (!m_showTooltip || !m_hovered || m_hoverSeriesIdx < 0 || m_hoverPointIdx < 0) return;
    if (m_hoverSeriesIdx >= m_series.size()) return;
    if (m_hoverSeriesIdx < m_seriesVisible.size() && !m_seriesVisible[m_hoverSeriesIdx]) return;

    const auto& s = m_series[m_hoverSeriesIdx];
    if (m_hoverPointIdx >= s.points.size()) return;

    QPointF dataPt = s.points[m_hoverPointIdx];
    QPointF screenPt = mapToScreen(dataPt, pa);
    QColor color = seriesColor(m_hoverSeriesIdx);

    // 竖直参考线
    painter->setPen(QPen(color, 1, Qt::DashLine));
    painter->drawLine(QPointF(screenPt.x(), pa.top()), QPointF(screenPt.x(), pa.bottom()));

    // 提示框
    QString xLabel = QString::number(dataPt.x(), 'g', 4);
    QString yLabel = QString::number(dataPt.y(), 'g', 4);
    QString text = s.name.isEmpty()
        ? QString("(%1, %2)").arg(xLabel, yLabel)
        : QString("%1: (%2, %3)").arg(s.name, xLabel, yLabel);

    auto* t = theme();
    QFontMetrics fm(t->captionFont());
    int textW = fm.horizontalAdvance(text) + 20;
    int textH = fm.height() + 12;

    // 提示框位置（避免超出边界）
    qreal tipX = screenPt.x() + 12;
    qreal tipY = screenPt.y() - textH - 8;
    if (tipX + textW > pa.right()) tipX = screenPt.x() - textW - 12;
    if (tipY < pa.top()) tipY = screenPt.y() + 12;

    QRectF tipRect(tipX, tipY, textW, textH);

    // 阴影
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 25));
    painter->drawRoundedRect(tipRect.translated(2, 2), 6, 6);

    // 背景
    painter->setBrush(t->cardColor());
    painter->setPen(QPen(t->dividerColor(), 1));
    painter->drawRoundedRect(tipRect, 6, 6);

    // 左侧色条
    QRectF colorBar(tipRect.left(), tipRect.top() + 4, 3, tipRect.height() - 8);
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->drawRoundedRect(colorBar, 1, 1);

    // 文字
    painter->setPen(t->textColorPrimary());
    painter->setFont(t->captionFont());
    painter->drawText(tipRect.adjusted(10, 0, -5, 0), Qt::AlignVCenter | Qt::AlignLeft, text);
}

void FluentLineChart::drawLegend(QPainter* painter) {
    if (m_series.isEmpty()) return;

    auto* t = theme();
    painter->setFont(t->captionFont());
    QFontMetrics fm(t->captionFont());

    m_legendRects.clear();
    int itemHeight = 20;
    int spacing = 20;
    int dotSize = 8;
    int totalWidth = 0;
    for (int i = 0; i < m_series.size(); ++i) {
        QString name = m_series[i].name.isEmpty() ? QString("Series %1").arg(i + 1) : m_series[i].name;
        totalWidth += fm.horizontalAdvance(name) + dotSize + 8 + spacing;
    }
    totalWidth -= spacing;

    qreal startX = (width() - totalWidth) / 2.0;
    qreal y = m_marginTop - 24;

    for (int i = 0; i < m_series.size(); ++i) {
        QColor color = seriesColor(i);
        bool visible = (i < m_seriesVisible.size()) ? m_seriesVisible[i] : true;
        QString name = m_series[i].name.isEmpty() ? QString("Series %1").arg(i + 1) : m_series[i].name;
        int nameW = fm.horizontalAdvance(name);
        qreal itemW = dotSize + 6 + nameW;

        // 缓存图例项矩形
        QRectF itemRect(startX, y, itemW + 4, itemHeight);
        m_legendRects.append(itemRect);

        // 悬停高亮背景
        if (m_legendHoverIdx == i) {
            QColor hoverBg = t->dividerColor();
            hoverBg.setAlphaF(0.15);
            painter->setPen(Qt::NoPen);
            painter->setBrush(hoverBg);
            painter->drawRoundedRect(itemRect.adjusted(-2, 0, 2, 0), 4, 4);
        }

        // 色点
        painter->setPen(Qt::NoPen);
        if (visible) {
            painter->setBrush(color);
        } else {
            painter->setBrush(t->textColorDisabled());
        }
        painter->drawEllipse(QPointF(startX + dotSize / 2.0, y + itemHeight / 2.0), dotSize / 2.0, dotSize / 2.0);

        // 文字
        if (visible) {
            painter->setPen(t->textColorSecondary());
        } else {
            painter->setPen(t->textColorDisabled());
        }
        qreal textX = startX + dotSize + 6;
        QRectF textRect(textX, y, nameW, itemHeight);
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, name);

        // 隐藏时画删除线
        if (!visible) {
            QPen strikePen(t->textColorDisabled(), 1);
            painter->setPen(strikePen);
            qreal lineY = y + itemHeight / 2.0;
            painter->drawLine(QPointF(textX, lineY), QPointF(textX + nameW, lineY));
        }

        startX += nameW + dotSize + 8 + spacing;
    }
}

// ============================================================
// 交互
// ============================================================

int FluentLineChart::findNearestPoint(const QPoint& mousePos, int& seriesIdx) const {
    QRectF pa = plotArea();
    qreal minDist = 20.0; // 最大检测半径
    int bestPoint = -1;
    seriesIdx = -1;

    for (int si = 0; si < m_series.size(); ++si) {
        if (si < m_seriesVisible.size() && !m_seriesVisible[si]) continue;
        const auto& s = m_series[si];
        for (int pi = 0; pi < s.points.size(); ++pi) {
            QPointF screenPt = mapToScreen(s.points[pi], pa);
            qreal dist = qSqrt(qPow(screenPt.x() - mousePos.x(), 2) +
                                qPow(screenPt.y() - mousePos.y(), 2));
            if (dist < minDist) {
                minDist = dist;
                bestPoint = pi;
                seriesIdx = si;
            }
        }
    }
    return bestPoint;
}

int FluentLineChart::legendItemAt(const QPoint& pos) const {
    for (int i = 0; i < m_legendRects.size(); ++i) {
        if (m_legendRects[i].adjusted(-2, -2, 2, 2).contains(pos)) {
            return i;
        }
    }
    return -1;
}

void FluentLineChart::mouseMoveEvent(QMouseEvent* event) {
    // 图例悬停检测
    int legendIdx = legendItemAt(event->pos());
    if (legendIdx != m_legendHoverIdx) {
        m_legendHoverIdx = legendIdx;
    }

    // 数据点悬停检测
    int seriesIdx = -1;
    int pointIdx = findNearestPoint(event->pos(), seriesIdx);

    if (pointIdx >= 0) {
        m_hovered = true;
        m_hoverSeriesIdx = seriesIdx;
        m_hoverPointIdx = pointIdx;
        m_hoverPos = event->pos();
    } else {
        m_hovered = false;
        m_hoverSeriesIdx = -1;
        m_hoverPointIdx = -1;
    }
    update();
    FluentWidget::mouseMoveEvent(event);
}

void FluentLineChart::leaveEvent(QEvent* event) {
    m_hovered = false;
    m_hoverSeriesIdx = -1;
    m_hoverPointIdx = -1;
    m_legendHoverIdx = -1;
    update();
    FluentWidget::leaveEvent(event);
}

void FluentLineChart::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int idx = legendItemAt(event->pos());
        if (idx >= 0) {
            toggleSeriesVisible(idx);
            return;
        }
    }
    FluentWidget::mousePressEvent(event);
}

// ============================================================
// 动画
// ============================================================

void FluentLineChart::timerEvent(QTimerEvent* event) {
    if (event->timerId() == m_animTimerId) {
        m_animProgress += 1.0 / (ANIM_DURATION / (1000.0 / ANIM_FPS));
        if (m_animProgress >= 1.0) {
            m_animProgress = 1.0;
            killTimer(m_animTimerId);
            m_animTimerId = 0;
        }
        update();
    }
}

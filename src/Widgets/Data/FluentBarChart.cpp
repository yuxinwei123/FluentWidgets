#include "FluentBarChart.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QtMath>
#include <algorithm>

// ============================================================
// FluentBarChart
// ============================================================

static const QColor s_palette[] = {
    QColor(0, 120, 212),
    QColor(16, 137, 108),
    QColor(218, 59, 59),
    QColor(255, 140, 0),
    QColor(116, 77, 169),
    QColor(0, 153, 188),
    QColor(230, 126, 34),
    QColor(142, 68, 173),
};

FluentBarChart::FluentBarChart(QWidget* parent)
    : FluentWidget(parent)
{
    setMinimumSize(300, 200);
    setMouseTracking(true);
    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

FluentBarChart::~FluentBarChart() {
    if (m_animTimerId) killTimer(m_animTimerId);
}

// ============================================================
// 属性
// ============================================================

FluentBarChart::Orientation FluentBarChart::orientation() const { return m_orientation; }
void FluentBarChart::setOrientation(Orientation orient) {
    if (m_orientation == orient) return;
    m_orientation = orient; update(); emit orientationChanged();
}

FluentBarChart::BarMode FluentBarChart::barMode() const { return m_barMode; }
void FluentBarChart::setBarMode(BarMode mode) {
    if (m_barMode == mode) return;
    m_barMode = mode; update(); emit barModeChanged();
}

bool FluentBarChart::showGrid() const { return m_showGrid; }
void FluentBarChart::setShowGrid(bool on) { m_showGrid = on; update(); emit showGridChanged(); }

bool FluentBarChart::showLegend() const { return m_showLegend; }
void FluentBarChart::setShowLegend(bool on) { m_showLegend = on; update(); emit showLegendChanged(); }

bool FluentBarChart::showTooltip() const { return m_showTooltip; }
void FluentBarChart::setShowTooltip(bool on) { m_showTooltip = on; update(); emit showTooltipChanged(); }

bool FluentBarChart::showValues() const { return m_showValues; }
void FluentBarChart::setShowValues(bool on) { m_showValues = on; update(); emit showValuesChanged(); }

bool FluentBarChart::animate() const { return m_animate; }
void FluentBarChart::setAnimate(bool on) { m_animate = on; emit animateChanged(); }

int FluentBarChart::barRadius() const { return m_barRadius; }
void FluentBarChart::setBarRadius(int r) { m_barRadius = r; update(); emit barRadiusChanged(); }

qreal FluentBarChart::barWidth() const { return m_barWidth; }
void FluentBarChart::setBarWidth(qreal w) { m_barWidth = qBound(0.1, w, 1.0); update(); emit barWidthChanged(); }

int FluentBarChart::chartMarginLeft() const { return m_marginLeft; }
void FluentBarChart::setChartMarginLeft(int m) { m_marginLeft = m; update(); emit chartMarginLeftChanged(); }
int FluentBarChart::chartMarginRight() const { return m_marginRight; }
void FluentBarChart::setChartMarginRight(int m) { m_marginRight = m; update(); emit chartMarginRightChanged(); }
int FluentBarChart::chartMarginTop() const { return m_marginTop; }
void FluentBarChart::setChartMarginTop(int m) { m_marginTop = m; update(); emit chartMarginTopChanged(); }
int FluentBarChart::chartMarginBottom() const { return m_marginBottom; }
void FluentBarChart::setChartMarginBottom(int m) { m_marginBottom = m; update(); emit chartMarginBottomChanged(); }

// ============================================================
// 数据管理
// ============================================================

int FluentBarChart::addSeries(const Series& series) {
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

void FluentBarChart::removeSeries(int index) {
    if (index < 0 || index >= m_series.size()) return;
    m_series.removeAt(index);
    m_seriesVisible.removeAt(index);
    computeAxisRange();
    update();
    emit seriesChanged();
}

void FluentBarChart::clearSeries() {
    m_series.clear();
    m_seriesVisible.clear();
    m_hovered = false;
    m_hoverSeriesIdx = -1;
    m_hoverCategoryIdx = -1;
    computeAxisRange();
    update();
    emit seriesChanged();
}

int FluentBarChart::seriesCount() const { return m_series.size(); }

FluentBarChart::Series FluentBarChart::series(int index) const {
    if (index < 0 || index >= m_series.size()) return Series();
    return m_series[index];
}

void FluentBarChart::updateSeries(int index, const Series& series) {
    if (index < 0 || index >= m_series.size()) return;
    m_series[index] = series;
    computeAxisRange();
    update();
    emit seriesChanged();
}

bool FluentBarChart::isSeriesVisible(int index) const {
    if (index < 0 || index >= m_seriesVisible.size()) return false;
    return m_seriesVisible[index];
}

void FluentBarChart::setSeriesVisible(int index, bool visible) {
    if (index < 0 || index >= m_seriesVisible.size()) return;
    if (m_seriesVisible[index] == visible) return;
    m_seriesVisible[index] = visible;
    computeAxisRange();
    update();
}

void FluentBarChart::toggleSeriesVisible(int index) {
    if (index < 0 || index >= m_seriesVisible.size()) return;
    m_seriesVisible[index] = !m_seriesVisible[index];
    computeAxisRange();
    update();
}

void FluentBarChart::setAutoAxisRange(bool autoRange) {
    m_autoAxisRange = autoRange;
    if (autoRange) computeAxisRange();
    update();
}

void FluentBarChart::setValueAxisRange(qreal min, qreal max) {
    m_autoAxisRange = false;
    m_valueMin = min; m_valueMax = max;
    update();
}

void FluentBarChart::setCategoryAxisTitle(const QString& title) { m_categoryAxisTitle = title; update(); }
void FluentBarChart::setValueAxisTitle(const QString& title) { m_valueAxisTitle = title; update(); }
void FluentBarChart::setValueTickCount(int count) { m_valueTickCount = qMax(2, count); update(); }

QSize FluentBarChart::sizeHint() const { return QSize(600, 400); }
QSize FluentBarChart::minimumSizeHint() const { return QSize(300, 200); }

// ============================================================
// 辅助方法
// ============================================================

QRectF FluentBarChart::plotArea() const {
    int left = m_marginLeft;
    int right = m_marginRight;
    int top = m_marginTop;
    int bottom = m_marginBottom;
    if (m_showLegend && !m_series.isEmpty()) top += 28;
    return QRectF(left, top, width() - left - right, height() - top - bottom);
}

int FluentBarChart::categoryCount() const {
    int maxCat = 0;
    for (const auto& s : m_series) {
        maxCat = qMax(maxCat, s.categories.size());
    }
    return maxCat;
}

QString FluentBarChart::categoryLabel(int index) const {
    for (const auto& s : m_series) {
        if (index < s.categories.size() && !s.categories[index].isEmpty())
            return s.categories[index];
    }
    return QString();
}

void FluentBarChart::computeAxisRange() {
    if (!m_autoAxisRange) return;

    bool hasData = false;
    qreal maxVal = 0;

    for (int si = 0; si < m_series.size(); ++si) {
        if (si < m_seriesVisible.size() && !m_seriesVisible[si]) continue;
        const auto& s = m_series[si];
        if (m_barMode == Stacked) {
            // 堆叠模式：每个类别累加
            int cats = s.values.size();
            for (int ci = 0; ci < cats; ++ci) {
                qreal stacked = 0;
                for (int sj = 0; sj <= si; ++sj) {
                    if (sj < m_seriesVisible.size() && !m_seriesVisible[sj]) continue;
                    if (ci < m_series[sj].values.size())
                        stacked += m_series[sj].values[ci];
                }
                if (stacked > maxVal) { maxVal = stacked; hasData = true; }
            }
        } else {
            for (qreal v : s.values) {
                if (v > maxVal) { maxVal = v; hasData = true; }
            }
        }
    }

    if (!hasData) {
        m_valueMin = 0; m_valueMax = 100;
        return;
    }

    qreal pad = maxVal * 0.08;
    if (pad < 1.0) pad = 1.0;
    m_valueMin = 0;
    m_valueMax = maxVal + pad;
    if (m_valueMax <= m_valueMin) m_valueMax = m_valueMin + 1;
}

QColor FluentBarChart::seriesColor(int index) const {
    if (index >= 0 && index < m_series.size() && m_series[index].color.isValid())
        return m_series[index].color;
    return s_palette[index % PALETTE_SIZE];
}

int FluentBarChart::legendItemAt(const QPoint& pos) const {
    for (int i = 0; i < m_legendRects.size(); ++i) {
        if (m_legendRects[i].adjusted(-2, -2, 2, 2).contains(pos)) return i;
    }
    return -1;
}

// ============================================================
// 绘制
// ============================================================

void FluentBarChart::paintFluent(QPainter* painter) {
    painter->setRenderHint(QPainter::Antialiasing);
    QRectF pa = plotArea();
    drawBackground(painter, pa);
    drawGrid(painter, pa);
    drawAxes(painter, pa);
    drawBars(painter, pa);
    drawTooltip(painter, pa);
    if (m_showLegend) drawLegend(painter);
}

void FluentBarChart::drawBackground(QPainter* painter, const QRectF& pa) {
    auto* t = theme();
    painter->setPen(Qt::NoPen);
    painter->setBrush(t->cardColor());
    painter->drawRoundedRect(pa.adjusted(-1, -1, 1, 1), 4, 4);
}

void FluentBarChart::drawGrid(QPainter* painter, const QRectF& pa) {
    if (!m_showGrid) return;
    auto* t = theme();
    QColor gridColor = t->dividerColor();
    gridColor.setAlphaF(0.5);
    QPen gridPen(gridColor, 1, Qt::DashLine);
    painter->setPen(gridPen);

    qreal range = m_valueMax - m_valueMin;
    if (range <= 0) return;

    for (int i = 0; i <= m_valueTickCount; ++i) {
        qreal val = m_valueMin + range * i / m_valueTickCount;
        if (m_orientation == Vertical) {
            qreal y = pa.bottom() - (val - m_valueMin) / range * pa.height();
            painter->drawLine(QPointF(pa.left(), y), QPointF(pa.right(), y));
        } else {
            qreal x = pa.left() + (val - m_valueMin) / range * pa.width();
            painter->drawLine(QPointF(x, pa.top()), QPointF(x, pa.bottom()));
        }
    }
}

void FluentBarChart::drawAxes(QPainter* painter, const QRectF& pa) {
    auto* t = theme();
    painter->setFont(t->captionFont());

    // 坐标轴线
    painter->setPen(QPen(t->dividerColor(), 1, Qt::SolidLine));
    if (m_orientation == Vertical) {
        painter->drawLine(QPointF(pa.left(), pa.top()), QPointF(pa.left(), pa.bottom()));
        painter->drawLine(QPointF(pa.left(), pa.bottom()), QPointF(pa.right(), pa.bottom()));
    } else {
        painter->drawLine(QPointF(pa.left(), pa.bottom()), QPointF(pa.right(), pa.bottom()));
        painter->drawLine(QPointF(pa.left(), pa.top()), QPointF(pa.left(), pa.bottom()));
    }

    painter->setPen(t->textColorSecondary());

    qreal range = m_valueMax - m_valueMin;
    if (range <= 0) return;

    // 值轴刻度
    for (int i = 0; i <= m_valueTickCount; ++i) {
        qreal val = m_valueMin + range * i / m_valueTickCount;
        QString label = QString::number(val, 'g', 4);
        if (m_orientation == Vertical) {
            qreal y = pa.bottom() - (val - m_valueMin) / range * pa.height();
            QRectF labelRect(pa.left() - m_marginLeft, y - 8, m_marginLeft - 6, 16);
            painter->drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, label);
        } else {
            qreal x = pa.left() + (val - m_valueMin) / range * pa.width();
            QRectF labelRect(x - 40, pa.bottom() + 4, 80, 16);
            painter->drawText(labelRect, Qt::AlignCenter | Qt::AlignTop, label);
        }
    }

    // 类别轴标签
    int cats = categoryCount();
    if (cats == 0) return;
    qreal catWidth = (m_orientation == Vertical) ? pa.width() / cats : pa.height() / cats;

    for (int ci = 0; ci < cats; ++ci) {
        QString label = categoryLabel(ci);
        if (label.isEmpty()) continue;
        if (m_orientation == Vertical) {
            qreal cx = pa.left() + ci * catWidth + catWidth / 2;
            QRectF labelRect(cx - catWidth / 2, pa.bottom() + 4, catWidth, 16);
            painter->drawText(labelRect, Qt::AlignCenter | Qt::AlignTop, label);
        } else {
            qreal cy = pa.bottom() - ci * catWidth - catWidth / 2;
            painter->save();
            painter->translate(pa.left() - 6, cy);
            painter->rotate(-45);
            painter->drawText(QRectF(-60, -7, 120, 14), Qt::AlignRight | Qt::AlignVCenter, label);
            painter->restore();
        }
    }

    // 轴标题
    if (!m_valueAxisTitle.isEmpty()) {
        if (m_orientation == Vertical) {
            painter->save();
            painter->translate(12, pa.top() + pa.height() / 2);
            painter->rotate(-90);
            painter->drawText(QRectF(-60, -7, 120, 14), Qt::AlignCenter, m_valueAxisTitle);
            painter->restore();
        } else {
            QRectF titleRect(pa.left(), height() - 16, pa.width(), 14);
            painter->drawText(titleRect, Qt::AlignCenter, m_valueAxisTitle);
        }
    }
    if (!m_categoryAxisTitle.isEmpty()) {
        if (m_orientation == Vertical) {
            QRectF titleRect(pa.left(), height() - 16, pa.width(), 14);
            painter->drawText(titleRect, Qt::AlignCenter, m_categoryAxisTitle);
        } else {
            painter->save();
            painter->translate(12, pa.top() + pa.height() / 2);
            painter->rotate(-90);
            painter->drawText(QRectF(-60, -7, 120, 14), Qt::AlignCenter, m_categoryAxisTitle);
            painter->restore();
        }
    }
}

void FluentBarChart::drawBars(QPainter* painter, const QRectF& pa) {
    int cats = categoryCount();
    if (cats == 0) return;

    qreal range = m_valueMax - m_valueMin;
    if (range <= 0) range = 1.0;

    m_barRects.clear();

    int visibleSeries = 0;
    for (int si = 0; si < m_series.size(); ++si) {
        if (si < m_seriesVisible.size() && !m_seriesVisible[si]) continue;
        visibleSeries++;
    }
    if (visibleSeries == 0) return;

    if (m_orientation == Vertical) {
        qreal catWidth = pa.width() / cats;
        qreal groupWidth = catWidth * m_barWidth;
        int seriesIdx = 0;

        for (int si = 0; si < m_series.size(); ++si) {
            if (si < m_seriesVisible.size() && !m_seriesVisible[si]) continue;
            const auto& s = m_series[si];
            QColor color = seriesColor(si);
            qreal barW = (m_barMode == Grouped) ? groupWidth / visibleSeries : groupWidth;

            for (int ci = 0; ci < qMin(cats, s.values.size()); ++ci) {
                qreal val = s.values[ci] * m_animProgress;
                qreal barH = val / range * pa.height();

                qreal barX;
                if (m_barMode == Grouped) {
                    barX = pa.left() + ci * catWidth + (catWidth - groupWidth) / 2 + seriesIdx * barW;
                } else {
                    barX = pa.left() + ci * catWidth + (catWidth - groupWidth) / 2;
                    // 堆叠：累加前面系列的柱体高度
                    qreal stackedH = 0;
                    for (int sj = 0; sj < si; ++sj) {
                        if (sj < m_seriesVisible.size() && !m_seriesVisible[sj]) continue;
                        if (ci < m_series[sj].values.size())
                            stackedH += m_series[sj].values[ci] / range * pa.height();
                    }
                    // 从堆叠高度开始绘制
                    qreal barTop = pa.bottom() - stackedH - barH;
                    QRectF barRect(barX, barTop, barW, barH);

                    // 悬停高亮
                    bool isHovered = (m_hovered && m_hoverSeriesIdx == si && m_hoverCategoryIdx == ci);
                    QColor drawColor = isHovered ? color.lighter(120) : color;

                    painter->setPen(Qt::NoPen);
                    painter->setBrush(drawColor);
                    if (m_barRadius > 0 && barH > m_barRadius * 2) {
                        painter->drawRoundedRect(barRect, m_barRadius, m_barRadius);
                    } else {
                        painter->drawRect(barRect);
                    }

                    m_barRects.append({si, ci, barRect});

                    // 值标签
                    if (m_showValues && barH > 14) {
                        painter->setPen(theme()->textColorPrimary());
                        painter->setFont(theme()->captionFont());
                        painter->drawText(barRect, Qt::AlignHCenter | Qt::AlignTop,
                                          QString::number(qRound(s.values[ci])));
                    }
                    continue;
                }

                qreal barTop = pa.bottom() - barH;
                QRectF barRect(barX, barTop, barW, barH);

                bool isHovered = (m_hovered && m_hoverSeriesIdx == si && m_hoverCategoryIdx == ci);
                QColor drawColor = isHovered ? color.lighter(120) : color;

                painter->setPen(Qt::NoPen);
                painter->setBrush(drawColor);
                if (m_barRadius > 0 && barH > m_barRadius * 2) {
                    painter->drawRoundedRect(barRect, m_barRadius, m_barRadius);
                } else {
                    painter->drawRect(barRect);
                }

                m_barRects.append({si, ci, barRect});

                if (m_showValues && barH > 14) {
                    painter->setPen(theme()->textColorPrimary());
                    painter->setFont(theme()->captionFont());
                    painter->drawText(barRect, Qt::AlignHCenter | Qt::AlignTop,
                                      QString::number(qRound(s.values[ci])));
                }
            }
            seriesIdx++;
        }
    } else {
        // 水平柱状图
        qreal catHeight = pa.height() / cats;
        qreal groupHeight = catHeight * m_barWidth;
        int seriesIdx = 0;

        for (int si = 0; si < m_series.size(); ++si) {
            if (si < m_seriesVisible.size() && !m_seriesVisible[si]) continue;
            const auto& s = m_series[si];
            QColor color = seriesColor(si);
            qreal barH = (m_barMode == Grouped) ? groupHeight / visibleSeries : groupHeight;

            for (int ci = 0; ci < qMin(cats, s.values.size()); ++ci) {
                qreal val = s.values[ci] * m_animProgress;
                qreal barW = val / range * pa.width();

                qreal barY;
                if (m_barMode == Grouped) {
                    barY = pa.bottom() - (ci + 1) * catHeight + (catHeight - groupHeight) / 2 + seriesIdx * barH;
                } else {
                    barY = pa.bottom() - (ci + 1) * catHeight + (catHeight - groupHeight) / 2;
                    qreal stackedW = 0;
                    for (int sj = 0; sj < si; ++sj) {
                        if (sj < m_seriesVisible.size() && !m_seriesVisible[sj]) continue;
                        if (ci < m_series[sj].values.size())
                            stackedW += m_series[sj].values[ci] / range * pa.width();
                    }
                    QRectF barRect(pa.left() + stackedW, barY, barW, barH);

                    bool isHovered = (m_hovered && m_hoverSeriesIdx == si && m_hoverCategoryIdx == ci);
                    QColor drawColor = isHovered ? color.lighter(120) : color;

                    painter->setPen(Qt::NoPen);
                    painter->setBrush(drawColor);
                    if (m_barRadius > 0 && barW > m_barRadius * 2) {
                        painter->drawRoundedRect(barRect, m_barRadius, m_barRadius);
                    } else {
                        painter->drawRect(barRect);
                    }

                    m_barRects.append({si, ci, barRect});

                    if (m_showValues && barW > 30) {
                        painter->setPen(theme()->textColorPrimary());
                        painter->setFont(theme()->captionFont());
                        painter->drawText(barRect.adjusted(4, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft,
                                          QString::number(qRound(s.values[ci])));
                    }
                    continue;
                }

                QRectF barRect(pa.left(), barY, barW, barH);

                bool isHovered = (m_hovered && m_hoverSeriesIdx == si && m_hoverCategoryIdx == ci);
                QColor drawColor = isHovered ? color.lighter(120) : color;

                painter->setPen(Qt::NoPen);
                painter->setBrush(drawColor);
                if (m_barRadius > 0 && barW > m_barRadius * 2) {
                    painter->drawRoundedRect(barRect, m_barRadius, m_barRadius);
                } else {
                    painter->drawRect(barRect);
                }

                m_barRects.append({si, ci, barRect});

                if (m_showValues && barW > 30) {
                    painter->setPen(theme()->textColorPrimary());
                    painter->setFont(theme()->captionFont());
                    painter->drawText(barRect.adjusted(4, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft,
                                      QString::number(qRound(s.values[ci])));
                }
            }
            seriesIdx++;
        }
    }
}

void FluentBarChart::drawTooltip(QPainter* painter, const QRectF& pa) {
    if (!m_showTooltip || !m_hovered || m_hoverSeriesIdx < 0 || m_hoverCategoryIdx < 0) return;
    if (m_hoverSeriesIdx >= m_series.size()) return;
    if (m_hoverSeriesIdx < m_seriesVisible.size() && !m_seriesVisible[m_hoverSeriesIdx]) return;

    const auto& s = m_series[m_hoverSeriesIdx];
    if (m_hoverCategoryIdx >= s.values.size()) return;

    // 找到柱体矩形
    QRectF barRect;
    bool found = false;
    for (const auto& info : m_barRects) {
        if (info.seriesIdx == m_hoverSeriesIdx && info.categoryIdx == m_hoverCategoryIdx) {
            barRect = info.rect;
            found = true;
            break;
        }
    }
    if (!found) return;

    qreal val = s.values[m_hoverCategoryIdx];
    QString catLabel = categoryLabel(m_hoverCategoryIdx);
    QString seriesName = s.name.isEmpty() ? QString("Series %1").arg(m_hoverSeriesIdx + 1) : s.name;
    QString text = catLabel.isEmpty()
        ? QString("%1: %2").arg(seriesName).arg(val, 0, 'g', 4)
        : QString("%1 - %2: %3").arg(catLabel, seriesName).arg(val, 0, 'g', 4);

    auto* t = theme();
    QFontMetrics fm(t->captionFont());
    int textW = fm.horizontalAdvance(text) + 24;
    int textH = fm.height() + 12;
    QColor color = seriesColor(m_hoverSeriesIdx);

    qreal tipX = barRect.center().x() - textW / 2;
    qreal tipY = barRect.top() - textH - 6;
    if (tipX < pa.left()) tipX = pa.left();
    if (tipX + textW > pa.right()) tipX = pa.right() - textW;
    if (tipY < pa.top()) tipY = barRect.bottom() + 6;

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
    painter->drawText(tipRect.adjusted(12, 0, -5, 0), Qt::AlignVCenter | Qt::AlignLeft, text);
}

void FluentBarChart::drawLegend(QPainter* painter) {
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

        QRectF itemRect(startX, y, itemW + 4, itemHeight);
        m_legendRects.append(itemRect);

        // 悬停高亮
        if (m_legendHoverIdx == i) {
            QColor hoverBg = t->dividerColor();
            hoverBg.setAlphaF(0.15);
            painter->setPen(Qt::NoPen);
            painter->setBrush(hoverBg);
            painter->drawRoundedRect(itemRect.adjusted(-2, 0, 2, 0), 4, 4);
        }

        // 色块（小方块表示柱状）
        painter->setPen(Qt::NoPen);
        painter->setBrush(visible ? color : t->textColorDisabled());
        painter->drawRoundedRect(QRectF(startX, y + (itemHeight - dotSize) / 2.0, dotSize, dotSize), 2, 2);

        // 文字
        painter->setPen(visible ? t->textColorSecondary() : t->textColorDisabled());
        qreal textX = startX + dotSize + 6;
        QRectF textRect(textX, y, nameW, itemHeight);
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, name);

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

void FluentBarChart::mouseMoveEvent(QMouseEvent* event) {
    // 图例悬停
    int legendIdx = legendItemAt(event->pos());
    if (legendIdx != m_legendHoverIdx) {
        m_legendHoverIdx = legendIdx;
    }

    // 柱体悬停
    m_hovered = false;
    m_hoverSeriesIdx = -1;
    m_hoverCategoryIdx = -1;
    for (const auto& info : m_barRects) {
        if (info.rect.contains(event->pos())) {
            m_hovered = true;
            m_hoverSeriesIdx = info.seriesIdx;
            m_hoverCategoryIdx = info.categoryIdx;
            break;
        }
    }
    update();
    FluentWidget::mouseMoveEvent(event);
}

void FluentBarChart::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int idx = legendItemAt(event->pos());
        if (idx >= 0) {
            toggleSeriesVisible(idx);
            return;
        }
    }
    FluentWidget::mousePressEvent(event);
}

void FluentBarChart::leaveEvent(QEvent* event) {
    m_hovered = false;
    m_hoverSeriesIdx = -1;
    m_hoverCategoryIdx = -1;
    m_legendHoverIdx = -1;
    update();
    FluentWidget::leaveEvent(event);
}

// ============================================================
// 动画
// ============================================================

void FluentBarChart::timerEvent(QTimerEvent* event) {
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

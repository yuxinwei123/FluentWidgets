#include "FluentPieChart.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QtMath>

// 内置调色板
static const QColor s_palette[12] = {
    QColor("#0078D4"), QColor("#10896C"), QColor("#DA3B3B"), QColor("#FF8C00"),
    QColor("#744DA9"), QColor("#0099BC"), QColor("#E67E22"), QColor("#8E44AD"),
    QColor("#2ECC71"), QColor("#E74C3C"), QColor("#3498DB"), QColor("#F39C12"),
};

FluentPieChart::FluentPieChart(QWidget* parent)
    : FluentWidget(parent)
{
    setMouseTracking(true);
    setMinimumSize(200, 200);
}

FluentPieChart::~FluentPieChart() {
    if (m_animTimerId) killTimer(m_animTimerId);
}

// === 属性 ===

FluentPieChart::PieStyle FluentPieChart::pieStyle() const { return m_pieStyle; }
void FluentPieChart::setPieStyle(PieStyle style) {
    if (m_pieStyle != style) {
        m_pieStyle = style;
        emit pieStyleChanged();
        update();
    }
}

bool FluentPieChart::showLegend() const { return m_showLegend; }
void FluentPieChart::setShowLegend(bool on) { if (m_showLegend != on) { m_showLegend = on; emit showLegendChanged(); update(); } }

bool FluentPieChart::showLabels() const { return m_showLabels; }
void FluentPieChart::setShowLabels(bool on) { if (m_showLabels != on) { m_showLabels = on; emit showLabelsChanged(); update(); } }

bool FluentPieChart::showPercent() const { return m_showPercent; }
void FluentPieChart::setShowPercent(bool on) { if (m_showPercent != on) { m_showPercent = on; emit showPercentChanged(); update(); } }

bool FluentPieChart::showTooltip() const { return m_showTooltip; }
void FluentPieChart::setShowTooltip(bool on) { if (m_showTooltip != on) { m_showTooltip = on; emit showTooltipChanged(); update(); } }

bool FluentPieChart::animate() const { return m_animate; }
void FluentPieChart::setAnimate(bool on) { m_animate = on; emit animateChanged(); }

qreal FluentPieChart::holeRatio() const { return m_holeRatio; }
void FluentPieChart::setHoleRatio(qreal ratio) {
    m_holeRatio = qBound(0.0, ratio, 0.9);
    emit holeRatioChanged();
    update();
}

qreal FluentPieChart::explodeDistance() const { return m_explodeDistance; }
void FluentPieChart::setExplodeDistance(qreal dist) {
    m_explodeDistance = qMax(0.0, dist);
    emit explodeDistanceChanged();
    update();
}

// === 数据管理 ===

int FluentPieChart::addSlice(const Slice& slice) {
    m_slices.append(slice);
    if (m_animate) {
        m_animProgress = 0.0;
        if (m_animTimerId) killTimer(m_animTimerId);
        m_animTimerId = startTimer(1000 / ANIM_FPS);
    }
    update();
    return m_slices.size() - 1;
}

int FluentPieChart::addSlice(const QString& name, qreal value, const QColor& color) {
    Slice s;
    s.name = name;
    s.value = value;
    s.color = color;
    return addSlice(s);
}

void FluentPieChart::removeSlice(int index) {
    if (index >= 0 && index < m_slices.size()) {
        m_slices.removeAt(index);
        if (m_selectedSlice >= m_slices.size()) m_selectedSlice = -1;
        if (m_hoverSlice >= m_slices.size()) m_hoverSlice = -1;
        update();
    }
}

void FluentPieChart::clearSlices() {
    m_slices.clear();
    m_hoverSlice = -1;
    m_selectedSlice = -1;
    m_legendRects.clear();
    update();
}

int FluentPieChart::sliceCount() const { return m_slices.size(); }

FluentPieChart::Slice FluentPieChart::slice(int index) const {
    if (index >= 0 && index < m_slices.size()) return m_slices[index];
    return Slice();
}

void FluentPieChart::updateSlice(int index, const Slice& slice) {
    if (index >= 0 && index < m_slices.size()) {
        m_slices[index] = slice;
        update();
    }
}

int FluentPieChart::selectedSlice() const { return m_selectedSlice; }
void FluentPieChart::setSelectedSlice(int index) {
    if (m_selectedSlice != index) {
        m_selectedSlice = index;
        emit selectionChanged(index);
        update();
    }
}

void FluentPieChart::setTitle(const QString& title) { m_title = title; update(); }
QString FluentPieChart::title() const { return m_title; }

QSize FluentPieChart::sizeHint() const { return QSize(400, 350); }
QSize FluentPieChart::minimumSizeHint() const { return QSize(200, 200); }

// === 绘制 ===

void FluentPieChart::paintFluent(QPainter* painter) {
    auto* theme = FluentThemeManager::instance().currentTheme();

    // 绘图区域
    int legendW = m_showLegend ? 140 : 0;
    QRectF pieRect;
    if (m_showLegend) {
        qreal side = qMin(width() - legendW - 40, height() - 40);
        side = qMax(side, 100.0);
        pieRect = QRectF(20, 20 + (height() - 40 - side) / 2, side, side);
    } else {
        qreal side = qMin(width() - 40, height() - 40);
        side = qMax(side, 100.0);
        pieRect = QRectF(20 + (width() - 40 - side) / 2, 20 + (height() - 40 - side) / 2, side, side);
    }

    drawPie(painter, pieRect);

    if (m_showLabels)
        drawLabels(painter, pieRect);

    if (m_pieStyle == Donut)
        drawCenterText(painter, pieRect);

    if (m_showTooltip && m_hoverSlice >= 0)
        drawTooltip(painter, pieRect);

    if (m_showLegend)
        drawLegend(painter);
}

void FluentPieChart::drawPie(QPainter* painter, const QRectF& pieRect) {
    auto* theme = FluentThemeManager::instance().currentTheme();
    if (m_slices.isEmpty()) return;

    qreal total = totalValue();
    if (total <= 0) return;

    qreal startAngle = 90.0; // 从顶部开始（Qt 中 90° 是 12 点方向）

    QPointF center = pieRect.center();
    qreal outerR = pieRect.width() / 2.0;

    for (int i = 0; i < m_slices.size(); ++i) {
        const auto& s = m_slices[i];
        qreal span = (s.value / total) * 360.0;

        // 动画裁剪
        qreal drawnSpan = span * m_animProgress;

        if (drawnSpan <= 0) {
            startAngle -= span;
            continue;
        }

        bool isHover = (i == m_hoverSlice);
        bool isSelected = (i == m_selectedSlice);

        // 弹出偏移
        QPointF offset;
        if (isHover || isSelected) {
            qreal midAngle = qDegreesToRadians(startAngle - drawnSpan / 2.0);
            qreal dist = isHover ? m_explodeDistance : m_explodeDistance * 0.5;
            offset = QPointF(qCos(midAngle) * dist, -qSin(midAngle) * dist);
        }

        QColor c = sliceColor(i);

        // 悬停时颜色稍亮
        if (isHover) {
            c = c.lighter(115);
        }

        QRectF drawRect = pieRect.translated(offset);
        painter->setPen(Qt::NoPen);
        painter->setBrush(c);

        QPainterPath path;
        if (m_pieStyle == Donut) {
            qreal innerR = outerR * m_holeRatio;
            path.moveTo(drawRect.center().x() + (innerR)*qCos(qDegreesToRadians(startAngle)),
                        drawRect.center().y() - (innerR)*qSin(qDegreesToRadians(startAngle)));
            path.arcTo(drawRect, startAngle, -drawnSpan);
            qreal endAngle = startAngle - drawnSpan;
            QPointF endOuter(drawRect.center().x() + outerR * qCos(qDegreesToRadians(endAngle)),
                             drawRect.center().y() - outerR * qSin(qDegreesToRadians(endAngle)));
            path.lineTo(drawRect.center().x() + innerR * qCos(qDegreesToRadians(endAngle)),
                        drawRect.center().y() - innerR * qSin(qDegreesToRadians(endAngle)));
            path.arcTo(QRectF(drawRect.center().x() - innerR, drawRect.center().y() - innerR,
                              innerR * 2, innerR * 2),
                       endAngle, drawnSpan);
            path.closeSubpath();
            painter->drawPath(path);
        } else {
            // 实心饼图
            painter->drawPie(drawRect, static_cast<int>(startAngle * 16), static_cast<int>(-drawnSpan * 16));
        }

        // 选中描边
        if (isSelected) {
            painter->setPen(QPen(theme->primaryColor(), 2.5));
            painter->setBrush(Qt::NoBrush);
            if (m_pieStyle == Donut) {
                painter->drawPath(path);
            } else {
                painter->drawPie(drawRect, static_cast<int>(startAngle * 16), static_cast<int>(-drawnSpan * 16));
            }
        }

        // 切片间白色分隔线
        if (m_slices.size() > 1) {
            painter->setPen(QPen(theme->backgroundColor(), 1.5));
            painter->setBrush(Qt::NoBrush);
            QPointF p1 = drawRect.center();
            QPointF p2(drawRect.center().x() + outerR * qCos(qDegreesToRadians(startAngle)),
                       drawRect.center().y() - outerR * qSin(qDegreesToRadians(startAngle)));
            painter->drawLine(p1, p2);
        }

        startAngle -= drawnSpan;
    }

    // 环形图中心圆
    if (m_pieStyle == Donut) {
        qreal innerR = outerR * m_holeRatio;
        painter->setPen(Qt::NoPen);
        painter->setBrush(theme->backgroundColor());
        painter->drawEllipse(pieRect.center(), innerR, innerR);
    }
}

void FluentPieChart::drawLabels(QPainter* painter, const QRectF& pieRect) {
    auto* theme = FluentThemeManager::instance().currentTheme();
    qreal total = totalValue();
    if (total <= 0) return;

    qreal startAngle = 90.0;
    qreal outerR = pieRect.width() / 2.0;
    QPointF center = pieRect.center();
    QFont labelFont = theme->captionFont();
    painter->setFont(labelFont);

    for (int i = 0; i < m_slices.size(); ++i) {
        const auto& s = m_slices[i];
        qreal span = (s.value / total) * 360.0;
        qreal midAngle = startAngle - span / 2.0;

        // 标签位置在外圈之外
        qreal labelR = outerR + 20;
        qreal rad = qDegreesToRadians(midAngle);
        QPointF labelPos(center.x() + labelR * qCos(rad),
                         center.y() - labelR * qSin(rad));

        QString text = s.name;
        if (m_showPercent) {
            qreal pct = (s.value / total) * 100.0;
            text += QString(" %1%").arg(pct, 0, 'f', 1);
        }

        // 只显示占比较大（>5%）的标签，避免拥挤
        qreal pct = (s.value / total) * 100.0;
        if (pct < 5.0) {
            startAngle -= span;
            continue;
        }

        QColor textColor = theme->textColorPrimary();
        painter->setPen(textColor);

        QFontMetrics fm(labelFont);
        int tw = fm.horizontalAdvance(text);

        // 对齐方式取决于角度
        if (midAngle > 0 && midAngle < 180) {
            // 右侧
            painter->drawText(QRectF(labelPos.x(), labelPos.y() - 8, tw + 4, 16),
                              Qt::AlignLeft | Qt::AlignVCenter, text);
        } else {
            // 左侧
            painter->drawText(QRectF(labelPos.x() - tw - 4, labelPos.y() - 8, tw + 4, 16),
                              Qt::AlignRight | Qt::AlignVCenter, text);
        }

        // 引导线
        qreal innerLabelR = outerR + 4;
        QPointF p1(center.x() + innerLabelR * qCos(rad),
                   center.y() - innerLabelR * qSin(rad));
        painter->setPen(QPen(theme->dividerColor(), 1));
        painter->drawLine(p1, labelPos);

        startAngle -= span;
    }
}

void FluentPieChart::drawCenterText(QPainter* painter, const QRectF& pieRect) {
    auto* theme = FluentThemeManager::instance().currentTheme();
    QPointF center = pieRect.center();
    qreal outerR = pieRect.width() / 2.0;
    qreal innerR = outerR * m_holeRatio;

    if (!m_title.isEmpty()) {
        painter->setFont(theme->bodyStrongFont());
        painter->setPen(theme->textColorPrimary());
        QFontMetrics fm(theme->bodyStrongFont());
        int tw = fm.horizontalAdvance(m_title);
        painter->drawText(QRectF(center.x() - tw / 2, center.y() - fm.height() / 2 - 4, tw, fm.height()),
                          Qt::AlignCenter, m_title);
    }

    // 如果有悬停切片，在中心显示值
    if (m_hoverSlice >= 0 && m_hoverSlice < m_slices.size()) {
        const auto& s = m_slices[m_hoverSlice];
        qreal total = totalValue();
        qreal pct = total > 0 ? (s.value / total) * 100.0 : 0;

        QString valueText = QString::number(pct, 'f', 1) + "%";
        painter->setFont(theme->subtitleFont());
        painter->setPen(sliceColor(m_hoverSlice));
        QFontMetrics fm(theme->subtitleFont());
        int tw = fm.horizontalAdvance(valueText);
        int yOff = m_title.isEmpty() ? -4 : 14;
        painter->drawText(QRectF(center.x() - tw / 2, center.y() + yOff, tw, fm.height()),
                          Qt::AlignCenter, valueText);
    }
}

void FluentPieChart::drawLegend(QPainter* painter) {
    auto* theme = FluentThemeManager::instance().currentTheme();
    if (m_slices.isEmpty()) return;

    int legendX = width() - 140;
    int legendY = 24;

    QFont legendFont = theme->captionFont();
    painter->setFont(legendFont);
    QFontMetrics fm(legendFont);

    m_legendRects.clear();

    qreal total = totalValue();

    for (int i = 0; i < m_slices.size(); ++i) {
        const auto& s = m_slices[i];
        QColor c = sliceColor(i);
        bool isHover = (i == m_hoverSlice) || (i == m_legendHoverIdx);

        // 色块
        QRectF colorRect(legendX, legendY, 10, 10);
        painter->setPen(Qt::NoPen);
        painter->setBrush(c);
        painter->drawRoundedRect(colorRect, 2, 2);

        // 文本
        qreal pct = total > 0 ? (s.value / total) * 100.0 : 0;
        QString text = s.name;
        if (m_showPercent) text += QString(" %1%").arg(pct, 0, 'f', 1);

        QColor textColor = isHover ? c : theme->textColorPrimary();
        painter->setPen(textColor);
        painter->drawText(QRectF(legendX + 16, legendY - 2, 120, 16),
                          Qt::AlignLeft | Qt::AlignVCenter, text);

        // 悬停高亮背景
        if (isHover) {
            painter->setPen(Qt::NoPen);
            QColor bg = theme->surfaceColor();
            bg.setAlpha(100);
            painter->setBrush(bg);
            painter->drawRoundedRect(QRectF(legendX - 4, legendY - 4, 136, 18), 4, 4);
        }

        m_legendRects.append(QRectF(legendX - 4, legendY - 4, 136, 18));
        legendY += 22;
    }
}

void FluentPieChart::drawTooltip(QPainter* painter, const QRectF& pieRect) {
    auto* theme = FluentThemeManager::instance().currentTheme();
    if (m_hoverSlice < 0 || m_hoverSlice >= m_slices.size()) return;

    const auto& s = m_slices[m_hoverSlice];
    qreal total = totalValue();
    qreal pct = total > 0 ? (s.value / total) * 100.0 : 0;

    QString text = QString("%1: %2 (%3%)").arg(s.name)
                    .arg(s.value, 0, 'f', 1)
                    .arg(pct, 0, 'f', 1);

    QFont tipFont = theme->captionFont();
    QFontMetrics fm(tipFont);
    int tw = fm.horizontalAdvance(text) + 16;
    int th = fm.height() + 10;

    // tooltip 位置
    QPoint tipPos = mapFromGlobal(QCursor::pos());
    int tx = tipPos.x() + 12;
    int ty = tipPos.y() - th - 4;
    if (tx + tw > width()) tx = tipPos.x() - tw - 12;
    if (ty < 0) ty = tipPos.y() + 16;

    QRectF tipRect(tx, ty, tw, th);

    painter->setPen(QPen(theme->dividerColor(), 1));
    painter->setBrush(theme->cardColor());
    painter->drawRoundedRect(tipRect, 6, 6);

    painter->setFont(tipFont);
    painter->setPen(theme->textColorPrimary());
    painter->drawText(tipRect, Qt::AlignCenter, text);
}

// === 事件 ===

int FluentPieChart::sliceAtAngle(qreal angle) const {
    qreal total = totalValue();
    if (total <= 0) return -1;

    // 将角度归一化到 [0, 360)
    angle = fmod(angle, 360.0);
    if (angle < 0) angle += 360.0;

    // 绘制从 90° 开始顺时针，转换为从 0° 开始顺时针偏移
    qreal offset = fmod(90.0 - angle, 360.0);
    if (offset < 0) offset += 360.0;

    qreal cumAngle = 0.0;
    for (int i = 0; i < m_slices.size(); ++i) {
        qreal span = (m_slices[i].value / total) * 360.0;
        cumAngle += span;
        if (offset < cumAngle) return i;
    }
    return m_slices.size() - 1;
}

int FluentPieChart::sliceAtPos(const QPoint& pos) const {
    if (m_slices.isEmpty()) return -1;

    // 计算饼图中心（与绘制逻辑一致）
    int legendW = m_showLegend ? 140 : 0;
    QRectF pieRect;
    if (m_showLegend) {
        qreal side = qMin(width() - legendW - 40, height() - 40);
        side = qMax(side, 100.0);
        pieRect = QRectF(20, 20 + (height() - 40 - side) / 2, side, side);
    } else {
        qreal side = qMin(width() - 40, height() - 40);
        side = qMax(side, 100.0);
        pieRect = QRectF(20 + (width() - 40 - side) / 2, 20 + (height() - 40 - side) / 2, side, side);
    }

    QPointF center = pieRect.center();
    qreal outerR = pieRect.width() / 2.0;
    qreal innerR = m_pieStyle == Donut ? outerR * m_holeRatio : 0;

    QPointF delta = pos - center;
    qreal dist = qSqrt(delta.x() * delta.x() + delta.y() * delta.y());

    if (dist > outerR || dist < innerR) return -1;

    // 计算角度（0° 在右侧，逆时针为正，与绘制一致）
    qreal angle = qRadiansToDegrees(qAtan2(-delta.y(), delta.x()));
    if (angle < 0) angle += 360.0;

    return sliceAtAngle(angle);
}

int FluentPieChart::legendItemAt(const QPoint& pos) const {
    for (int i = 0; i < m_legendRects.size(); ++i) {
        if (m_legendRects[i].contains(pos)) return i;
    }
    return -1;
}

qreal FluentPieChart::totalValue() const {
    qreal total = 0;
    for (const auto& s : m_slices) total += s.value;
    return total;
}

QColor FluentPieChart::sliceColor(int index) const {
    if (index >= 0 && index < m_slices.size()) {
        if (m_slices[index].color.isValid()) return m_slices[index].color;
        return s_palette[index % PALETTE_SIZE];
    }
    return QColor();
}

void FluentPieChart::mouseMoveEvent(QMouseEvent* event) {
    int slice = sliceAtPos(event->pos());
    int legend = m_showLegend ? legendItemAt(event->pos()) : -1;

    int newHover = (slice >= 0) ? slice : ((legend >= 0) ? legend : -1);

    if (newHover != m_hoverSlice || legend != m_legendHoverIdx) {
        m_hoverSlice = slice;
        m_legendHoverIdx = legend;
        setCursor(newHover >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
    }
}

void FluentPieChart::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int slice = sliceAtPos(event->pos());
        if (slice >= 0) {
            setSelectedSlice(slice);
            emit sliceClicked(slice);
        }
    }
}

void FluentPieChart::leaveEvent(QEvent* /*event*/) {
    m_hoverSlice = -1;
    m_legendHoverIdx = -1;
    setCursor(Qt::ArrowCursor);
    update();
}

void FluentPieChart::timerEvent(QTimerEvent* event) {
    if (event->timerId() == m_animTimerId) {
        m_animProgress += 1.0 / (ANIM_DURATION / 1000.0 * ANIM_FPS);
        if (m_animProgress >= 1.0) {
            m_animProgress = 1.0;
            killTimer(m_animTimerId);
            m_animTimerId = 0;
        }
        update();
    }
}

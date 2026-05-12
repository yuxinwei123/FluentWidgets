#include "FluentTimeline.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QScrollBar>
#include <QtMath>

FluentTimeline::FluentTimeline(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [this]() {
        update();
    });
}

// === 属性 ===

FluentTimeline::Orientation FluentTimeline::orientation() const { return m_orientation; }
void FluentTimeline::setOrientation(Orientation orient) {
    if (m_orientation != orient) {
        m_orientation = orient;
        updateGeometry();
        update();
    }
}

bool FluentTimeline::selectionEnabled() const { return m_selectionEnabled; }
void FluentTimeline::setSelectionEnabled(bool enabled) {
    m_selectionEnabled = enabled;
    if (!enabled) clearSelection();
}

int FluentTimeline::selectedIndex() const {
    auto sel = selectedIndexes();
    return sel.isEmpty() ? -1 : sel.first();
}

void FluentTimeline::setSelectedIndex(int index) {
    if (index >= 0 && index < m_items.size()) {
        setSelectedIndexes({index});
    } else {
        clearSelection();
    }
}

QVector<int> FluentTimeline::selectedIndexes() const {
    QVector<int> result;
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].selected) result.append(i);
    }
    return result;
}

void FluentTimeline::setSelectedIndexes(const QVector<int>& indexes) {
    for (int i = 0; i < m_items.size(); ++i) {
        m_items[i].selected = indexes.contains(i);
    }
    emit selectionChanged(indexes);
    update();
}

void FluentTimeline::clearSelection() {
    bool changed = false;
    for (auto& item : m_items) {
        if (item.selected) { item.selected = false; changed = true; }
    }
    if (changed) {
        emit selectionChanged({});
        update();
    }
}

// === 增删 ===

void FluentTimeline::addItem(const Item& item) {
    m_items.append(item);
    updateGeometry();
    update();
}

void FluentTimeline::addItem(const QString& title, const QString& description,
                              const QString& timestamp, const QColor& color, NodeStyle style) {
    Item item;
    item.title = title;
    item.description = description;
    item.timestamp = timestamp;
    item.color = color;
    item.nodeStyle = style;
    addItem(item);
}

void FluentTimeline::insertItem(int index, const Item& item) {
    if (index >= 0 && index <= m_items.size()) {
        m_items.insert(index, item);
        updateGeometry();
        update();
    }
}

void FluentTimeline::removeItem(int index) {
    if (index >= 0 && index < m_items.size()) {
        m_items.removeAt(index);
        updateGeometry();
        update();
    }
}

void FluentTimeline::clearItems() {
    m_items.clear();
    m_itemRects.clear();
    m_hoverIndex = -1;
    updateGeometry();
    update();
}

int FluentTimeline::itemCount() const { return m_items.size(); }

FluentTimeline::Item FluentTimeline::item(int index) const {
    if (index >= 0 && index < m_items.size()) return m_items[index];
    return Item();
}

QVector<FluentTimeline::Item> FluentTimeline::items() const { return m_items; }

void FluentTimeline::updateItem(int index, const Item& item) {
    if (index >= 0 && index < m_items.size()) {
        m_items[index] = item;
        update();
    }
}

int FluentTimeline::itemSpacing() const { return m_itemSpacing; }
void FluentTimeline::setItemSpacing(int spacing) {
    m_itemSpacing = qMax(30, spacing);
    updateGeometry();
    update();
}

// === 绘制 ===

void FluentTimeline::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if (m_orientation == Vertical)
        drawVertical(p);
    else
        drawHorizontal(p);

    // 绘制框选矩形
    if (m_rubberBanding) {
        QRect rubber = QRect(m_rubberStart, m_rubberCurrent).normalized();
        auto* theme = FluentThemeManager::instance().currentTheme();
        p.setPen(QPen(theme->primaryColor(), 1, Qt::DashLine));
        p.setBrush(theme->primaryColor().lighter(180));
        p.drawRect(rubber);
    }
}

void FluentTimeline::drawVertical(QPainter& p) {
    auto* theme = FluentThemeManager::instance().currentTheme();
    if (m_items.isEmpty()) return;

    const int nodeX = 30;                // 轴线 X 位置
    const int titleX = nodeX + m_contentMargin + m_nodeRadius;
    const int timestampX = nodeX - m_contentMargin - m_nodeRadius;
    const int totalW = width();

    m_itemRects.clear();
    int y = 20;

    QFont titleFont = theme->bodyStrongFont();
    QFont descFont = theme->captionFont();
    QFont timeFont = theme->captionFont();

    for (int i = 0; i < m_items.size(); ++i) {
        const auto& item = m_items[i];
        QColor nodeColor = item.color.isValid() ? item.color : theme->primaryColor();
        bool isHover = (i == m_hoverIndex);
        bool isSelected = item.selected;

        // === 轴线 ===
        if (i < m_items.size() - 1) {
            QColor lineColor = theme->dividerColor();
            p.setPen(QPen(lineColor, m_lineWidth, Qt::SolidLine, Qt::RoundCap));
            p.drawLine(nodeX, y + m_nodeRadius, nodeX, y + m_itemSpacing - m_nodeRadius);
        }

        // === 节点 ===
        QRectF nodeRect(nodeX - m_nodeRadius, y - m_nodeRadius,
                        m_nodeRadius * 2, m_nodeRadius * 2);

        // 选中/悬浮光晕
        if (isSelected || isHover) {
            QColor glowColor = nodeColor;
            glowColor.setAlpha(40);
            p.setPen(Qt::NoPen);
            p.setBrush(glowColor);
            p.drawEllipse(nodeRect.adjusted(-4, -4, 4, 4));
        }

        switch (item.nodeStyle) {
        case Dot:
            p.setPen(Qt::NoPen);
            p.setBrush(nodeColor);
            p.drawEllipse(nodeRect);
            break;
        case Ring:
            p.setPen(QPen(nodeColor, 2.5));
            p.setBrush(theme->backgroundColor());
            p.drawEllipse(nodeRect);
            break;
        case Diamond: {
            QPainterPath dp;
            QPointF center = nodeRect.center();
            qreal s = m_nodeRadius;
            dp.moveTo(center.x(), center.y() - s);
            dp.lineTo(center.x() + s, center.y());
            dp.lineTo(center.x(), center.y() + s);
            dp.lineTo(center.x() - s, center.y());
            dp.closeSubpath();
            p.setPen(Qt::NoPen);
            p.setBrush(nodeColor);
            p.drawPath(dp);
            break;
        }
        case Icon:
            // 用实心点 + 内部小白点表示"图标"样式
            p.setPen(Qt::NoPen);
            p.setBrush(nodeColor);
            p.drawEllipse(nodeRect);
            p.setBrush(theme->backgroundColor());
            p.drawEllipse(nodeRect.adjusted(3, 3, -3, -3));
            break;
        }

        // === 时间戳（左侧） ===
        if (!item.timestamp.isEmpty()) {
            p.setFont(timeFont);
            p.setPen(theme->textColorSecondary());
            QFontMetrics fm(timeFont);
            int tw = fm.horizontalAdvance(item.timestamp);
            p.drawText(QRect(timestampX - tw - 4, y - fm.height() / 2, tw + 4, fm.height()),
                       Qt::AlignRight | Qt::AlignVCenter, item.timestamp);
        }

        // === 标题 ===
        if (!item.title.isEmpty()) {
            p.setFont(titleFont);
            QColor titleColor = (isHover || isSelected) ? nodeColor : theme->textColorPrimary();
            p.setPen(titleColor);
            p.drawText(QRect(titleX, y - 14, totalW - titleX - 10, 20),
                       Qt::AlignLeft | Qt::AlignVCenter, item.title);
        }

        // === 描述 ===
        if (!item.description.isEmpty()) {
            p.setFont(descFont);
            p.setPen(theme->textColorSecondary());
            QFontMetrics fmDesc(descFont);
            // 简单自动换行
            QRect descRect(titleX, y + 6, totalW - titleX - 10, 40);
            p.drawText(descRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, item.description);
        }

        // 计算此条目的点击区域
        int itemHeight = m_itemSpacing;
        if (!item.description.isEmpty()) {
            QFontMetrics fmDesc(descFont);
            int descH = fmDesc.boundingRect(QRect(0, 0, totalW - titleX - 10, 0),
                                            Qt::TextWordWrap, item.description).height();
            itemHeight = qMax(itemHeight, 28 + descH + 4);
        }
        m_itemRects.append(QRect(0, y - m_nodeRadius, totalW, itemHeight));

        y += itemHeight;
    }
}

void FluentTimeline::drawHorizontal(QPainter& p) {
    auto* theme = FluentThemeManager::instance().currentTheme();
    if (m_items.isEmpty()) return;

    const int nodeY = 40;                // 轴线 Y 位置
    const int totalH = height();
    const int totalW = width();

    m_itemRects.clear();
    int x = 40;

    QFont titleFont = theme->captionFont();
    QFont timeFont = theme->captionFont();

    for (int i = 0; i < m_items.size(); ++i) {
        const auto& item = m_items[i];
        QColor nodeColor = item.color.isValid() ? item.color : theme->primaryColor();
        bool isHover = (i == m_hoverIndex);
        bool isSelected = item.selected;

        // === 轴线 ===
        if (i < m_items.size() - 1) {
            p.setPen(QPen(theme->dividerColor(), m_lineWidth, Qt::SolidLine, Qt::RoundCap));
            p.drawLine(x + m_nodeRadius, nodeY, x + m_itemSpacing - m_nodeRadius, nodeY);
        }

        // === 节点 ===
        QRectF nodeRect(x - m_nodeRadius, nodeY - m_nodeRadius,
                        m_nodeRadius * 2, m_nodeRadius * 2);

        if (isSelected || isHover) {
            QColor glowColor = nodeColor;
            glowColor.setAlpha(40);
            p.setPen(Qt::NoPen);
            p.setBrush(glowColor);
            p.drawEllipse(nodeRect.adjusted(-4, -4, 4, 4));
        }

        switch (item.nodeStyle) {
        case Dot:
            p.setPen(Qt::NoPen);
            p.setBrush(nodeColor);
            p.drawEllipse(nodeRect);
            break;
        case Ring:
            p.setPen(QPen(nodeColor, 2.5));
            p.setBrush(theme->backgroundColor());
            p.drawEllipse(nodeRect);
            break;
        case Diamond: {
            QPainterPath dp;
            QPointF center = nodeRect.center();
            qreal s = m_nodeRadius;
            dp.moveTo(center.x(), center.y() - s);
            dp.lineTo(center.x() + s, center.y());
            dp.lineTo(center.x(), center.y() + s);
            dp.lineTo(center.x() - s, center.y());
            dp.closeSubpath();
            p.setPen(Qt::NoPen);
            p.setBrush(nodeColor);
            p.drawPath(dp);
            break;
        }
        case Icon:
            p.setPen(Qt::NoPen);
            p.setBrush(nodeColor);
            p.drawEllipse(nodeRect);
            p.setBrush(theme->backgroundColor());
            p.drawEllipse(nodeRect.adjusted(3, 3, -3, -3));
            break;
        }

        // === 时间戳（上方） ===
        if (!item.timestamp.isEmpty()) {
            p.setFont(timeFont);
            p.setPen(theme->textColorSecondary());
            QFontMetrics fm(timeFont);
            int tw = fm.horizontalAdvance(item.timestamp);
            p.drawText(QRect(x - tw / 2, nodeY - m_nodeRadius - 20, qMax(tw, m_itemSpacing), 18),
                       Qt::AlignHCenter | Qt::AlignVCenter, item.timestamp);
        }

        // === 标题（下方） ===
        if (!item.title.isEmpty()) {
            p.setFont(titleFont);
            QColor titleColor = (isHover || isSelected) ? nodeColor : theme->textColorPrimary();
            p.setPen(titleColor);
            QFontMetrics fm(titleFont);
            int tw = fm.horizontalAdvance(item.title);
            p.drawText(QRect(x - qMax(tw, m_itemSpacing) / 2, nodeY + m_nodeRadius + 6,
                             qMax(tw, m_itemSpacing), totalH - nodeY - m_nodeRadius - 10),
                       Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap, item.title);
        }

        m_itemRects.append(QRect(x - m_itemSpacing / 2, 0, m_itemSpacing, totalH));
        x += m_itemSpacing;
    }
}

// === 事件 ===

int FluentTimeline::itemAtPos(const QPoint& pos) const {
    for (int i = 0; i < m_itemRects.size(); ++i) {
        if (m_itemRects[i].contains(pos)) return i;
    }
    return -1;
}

void FluentTimeline::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int idx = itemAtPos(event->pos());
        if (idx >= 0 && m_selectionEnabled) {
            if (event->modifiers() & Qt::ControlModifier) {
                // Ctrl+点击：切换选中
                m_items[idx].selected = !m_items[idx].selected;
                emit selectionChanged(selectedIndexes());
                emit itemClicked(idx);
                update();
            } else if (event->modifiers() & Qt::ShiftModifier) {
                // Shift+点击：范围选中
                int from = selectedIndex();
                if (from < 0) from = 0;
                int to = idx;
                if (from > to) std::swap(from, to);
                for (int i = 0; i < m_items.size(); ++i) {
                    m_items[i].selected = (i >= from && i <= to);
                }
                emit selectionChanged(selectedIndexes());
                emit itemClicked(idx);
                update();
            } else {
                // 单击
                bool wasSelected = m_items[idx].selected;
                clearSelection();
                m_items[idx].selected = true;
                if (!wasSelected || selectedIndexes().size() != 1) {
                    emit selectionChanged({idx});
                }
                emit itemClicked(idx);
                update();
            }
        } else if (m_selectionEnabled) {
            // 点击空白区域：开始框选
            m_rubberBanding = true;
            m_rubberStart = event->pos();
            m_rubberCurrent = event->pos();
        }
    }
}

void FluentTimeline::mouseMoveEvent(QMouseEvent* event) {
    if (m_rubberBanding) {
        m_rubberCurrent = event->pos();
        update();
        return;
    }

    int idx = itemAtPos(event->pos());
    if (idx != m_hoverIndex) {
        m_hoverIndex = idx;
        setCursor(idx >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
    }
}

void FluentTimeline::mouseReleaseEvent(QMouseEvent* event) {
    if (m_rubberBanding) {
        m_rubberBanding = false;
        QRect rubberRect = QRect(m_rubberStart, event->pos()).normalized();

        // 框选逻辑
        if (rubberRect.width() > 4 || rubberRect.height() > 4) {
            QVector<int> newSelection;
            for (int i = 0; i < m_itemRects.size(); ++i) {
                if (rubberRect.intersects(m_itemRects[i])) {
                    newSelection.append(i);
                }
            }
            if (!(event->modifiers() & Qt::ControlModifier)) {
                clearSelection();
            }
            for (int idx : newSelection) {
                m_items[idx].selected = true;
            }
            emit selectionChanged(selectedIndexes());
        }
        update();
    }
}

void FluentTimeline::updateGeometry() {
    if (m_orientation == Vertical) {
        int h = 40;
        auto* theme = FluentThemeManager::instance().currentTheme();
        QFontMetrics fmDesc(theme->captionFont());
        for (int i = 0; i < m_items.size(); ++i) {
            int itemH = m_itemSpacing;
            if (!m_items[i].description.isEmpty()) {
                int descH = fmDesc.boundingRect(QRect(0, 0, width() - 80, 0),
                                                Qt::TextWordWrap, m_items[i].description).height();
                itemH = qMax(itemH, 28 + descH + 4);
            }
            h += itemH;
        }
        setMinimumHeight(h);
    } else {
        int w = 80 + m_items.size() * m_itemSpacing;
        setMinimumWidth(w);
        setMinimumHeight(100);
    }
}

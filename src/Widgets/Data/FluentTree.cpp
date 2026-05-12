#include "FluentTree.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QApplication>

// ============================================================
// 构造 / 析构
// ============================================================

FluentTree::FluentTree(QWidget* parent)
    : FluentWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_selectionModel = new QItemSelectionModel(nullptr, this);
    connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &FluentTree::selectionChanged);

    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

FluentTree::~FluentTree() = default;

// ============================================================
// 数据模型
// ============================================================

void FluentTree::setModel(QAbstractItemModel* model) {
    if (m_model) {
        disconnect(m_model, nullptr, this, nullptr);
    }
    m_model = model;
    m_selectionModel->setModel(model);
    m_expandedIds.clear();

    if (m_model) {
        connect(m_model, &QAbstractItemModel::rowsInserted, this, [this]() { rebuildVisibleNodes(); });
        connect(m_model, &QAbstractItemModel::rowsRemoved, this, [this]() { rebuildVisibleNodes(); });
        connect(m_model, &QAbstractItemModel::dataChanged, this, [this]() { update(); });
        connect(m_model, &QAbstractItemModel::layoutChanged, this, [this]() { rebuildVisibleNodes(); });
        connect(m_model, &QAbstractItemModel::modelReset, this, [this]() {
            m_expandedIds.clear();
            rebuildVisibleNodes();
        });
    }

    m_scrollY = 0;
    rebuildVisibleNodes();
}

QAbstractItemModel* FluentTree::model() const { return m_model; }
QItemSelectionModel* FluentTree::selectionModel() const { return m_selectionModel; }

// ============================================================
// 属性 getter/setter
// ============================================================

int FluentTree::rowHeight() const { return m_rowHeight; }
void FluentTree::setRowHeight(int h) {
    if (m_rowHeight == h) return;
    m_rowHeight = h;
    update();
    emit rowHeightChanged();
}

int FluentTree::indent() const { return m_indent; }
void FluentTree::setIndent(int px) {
    if (m_indent == px) return;
    m_indent = px;
    update();
    emit indentChanged();
}

bool FluentTree::showLines() const { return m_showLines; }
void FluentTree::setShowLines(bool show) {
    if (m_showLines == show) return;
    m_showLines = show;
    update();
    emit showLinesChanged();
}

FluentTree::SelectionMode FluentTree::selectionMode() const { return m_selectionMode; }
void FluentTree::setSelectionMode(SelectionMode mode) {
    if (m_selectionMode == mode) return;
    m_selectionMode = mode;
    if (mode == SelectionMode::None) clearSelection();
    emit selectionModeChanged();
}

// ============================================================
// 展开 / 折叠
// ============================================================

void FluentTree::expand(const QModelIndex& index) {
    if (!index.isValid() || !m_model) return;
    m_expandedIds.insert(index.internalId());
    rebuildVisibleNodes();
    emit expanded(index);
}

void FluentTree::collapse(const QModelIndex& index) {
    if (!index.isValid()) return;
    m_expandedIds.remove(index.internalId());
    rebuildVisibleNodes();
    emit collapsed(index);
}

void FluentTree::expandAll() {
    if (!m_model) return;
    // 递归展开所有节点
    std::function<void(const QModelIndex&)> expandRecursive = [&](const QModelIndex& parent) {
        for (int r = 0; r < m_model->rowCount(parent); ++r) {
            QModelIndex child = m_model->index(r, 0, parent);
            if (m_model->hasChildren(child)) {
                m_expandedIds.insert(child.internalId());
                expandRecursive(child);
            }
        }
    };
    expandRecursive(QModelIndex());
    rebuildVisibleNodes();
}

void FluentTree::collapseAll() {
    m_expandedIds.clear();
    rebuildVisibleNodes();
}

bool FluentTree::isExpanded(const QModelIndex& index) const {
    return m_expandedIds.contains(index.internalId());
}

void FluentTree::toggleExpand(int visualRow) {
    if (visualRow < 0 || visualRow >= m_visibleNodes.size()) return;
    const auto& node = m_visibleNodes[visualRow];
    if (!node.hasChildren) return;

    if (node.expanded) {
        collapse(node.index);
    } else {
        expand(node.index);
    }
}

// ============================================================
// 选中
// ============================================================

QModelIndex FluentTree::currentIndex() const {
    return m_selectionModel ? m_selectionModel->currentIndex() : QModelIndex();
}

QModelIndexList FluentTree::selectedIndexes() const {
    return m_selectionModel ? m_selectionModel->selectedIndexes() : QModelIndexList();
}

void FluentTree::clearSelection() {
    if (m_selectionModel) m_selectionModel->clearSelection();
    update();
}

void FluentTree::updateSelectionFromClick(int visualRow, Qt::KeyboardModifiers mods) {
    if (!m_model || m_selectionMode == SelectionMode::None) return;
    if (visualRow < 0 || visualRow >= m_visibleNodes.size()) return;

    QModelIndex idx = m_visibleNodes[visualRow].index;

    if (m_selectionMode == SelectionMode::Single) {
        m_selectionModel->clearSelection();
        m_selectionModel->select(idx, QItemSelectionModel::Select);
        m_selectionModel->setCurrentIndex(idx, QItemSelectionModel::Current);
    } else if (m_selectionMode == SelectionMode::Multi) {
        if (mods & Qt::ControlModifier) {
            bool wasSelected = m_selectionModel->isSelected(idx);
            m_selectionModel->select(idx, wasSelected ? QItemSelectionModel::Deselect : QItemSelectionModel::Select);
        } else {
            m_selectionModel->clearSelection();
            m_selectionModel->select(idx, QItemSelectionModel::Select);
            m_selectionModel->setCurrentIndex(idx, QItemSelectionModel::Current);
        }
    }
    update();
}

// ============================================================
// 滚动
// ============================================================

void FluentTree::scrollTo(const QModelIndex& index) {
    // 找到 visual row
    for (int i = 0; i < m_visibleNodes.size(); ++i) {
        if (m_visibleNodes[i].index == index) {
            int y = i * m_rowHeight - m_scrollY;
            QRect area = contentRect();
            if (y < 0) {
                m_scrollY += y;
            } else if (y + m_rowHeight > area.height()) {
                m_scrollY += (y + m_rowHeight - area.height());
            }
            m_scrollY = qMax(0, m_scrollY);
            update();
            return;
        }
    }
}

// ============================================================
// 可见行
// ============================================================

void FluentTree::rebuildVisibleNodes() {
    m_visibleNodes.clear();
    if (!m_model) { update(); return; }

    std::function<void(const QModelIndex&, int)> traverse = [&](const QModelIndex& parent, int depth) {
        for (int r = 0; r < m_model->rowCount(parent); ++r) {
            QModelIndex idx = m_model->index(r, 0, parent);
            bool hasChildren = m_model->hasChildren(idx);
            bool expanded = m_expandedIds.contains(idx.internalId());

            m_visibleNodes.append({idx, depth, expanded, hasChildren});

            if (hasChildren && expanded) {
                traverse(idx, depth + 1);
            }
        }
    };

    traverse(QModelIndex(), 0);
    update();
}

// ============================================================
// 布局计算
// ============================================================

QSize FluentTree::sizeHint() const { return QSize(300, 400); }
QSize FluentTree::minimumSizeHint() const { return QSize(150, 200); }

int FluentTree::totalContentHeight() const {
    return m_visibleNodes.size() * m_rowHeight;
}

QRect FluentTree::contentRect() const {
    int rightMargin = (totalContentHeight() > height()) ? SCROLLBAR_WIDTH + SCROLLBAR_MARGIN : 0;
    return QRect(0, 0, width() - rightMargin, height());
}

int FluentTree::rowAtY(int y) const {
    int row = (y + m_scrollY) / m_rowHeight;
    if (row >= 0 && row < m_visibleNodes.size()) return row;
    return -1;
}

int FluentTree::arrowRectAt(int visualRow, int x) const {
    if (visualRow < 0 || visualRow >= m_visibleNodes.size()) return -1;
    const auto& node = m_visibleNodes[visualRow];
    if (!node.hasChildren) return -1;

    int arrowX = node.depth * m_indent + ARROW_MARGIN;
    if (x >= arrowX && x <= arrowX + ARROW_SIZE) return visualRow;
    return -1;
}

// ============================================================
// 绘制
// ============================================================

void FluentTree::paintFluent(QPainter* painter) {
    auto* t = theme();

    // 背景
    painter->fillRect(rect(), t->cardColor());

    QRect area = contentRect();

    // 裁剪绘制区域
    painter->save();
    painter->setClipRect(area);

    int firstRow = m_scrollY / m_rowHeight;
    int lastRow = qMin(m_visibleNodes.size() - 1, (m_scrollY + area.height()) / m_rowHeight);

    for (int r = firstRow; r <= lastRow; ++r) {
        int y = r * m_rowHeight - m_scrollY;
        QRect rowRect(area.x(), y, area.width(), m_rowHeight);
        paintRow(painter, r, rowRect);
    }

    painter->restore();

    // 滚动条
    paintScrollBar(painter);

    // 外边框
    painter->setPen(QPen(t->borderColor(), t->borderWidth()));
    painter->setBrush(Qt::NoBrush);
    QPainterPath borderPath;
    borderPath.addRoundedRect(rect(), t->cornerRadiusSmall(), t->cornerRadiusSmall());
    painter->drawPath(borderPath);
}

void FluentTree::paintRow(QPainter* painter, int visualRow, const QRect& rowRect) {
    auto* t = theme();
    const auto& node = m_visibleNodes[visualRow];

    // 悬停高亮
    if (m_hoveredRow == visualRow) {
        QColor hoverColor = t->textColorPrimary();
        hoverColor.setAlphaF(0.05);
        painter->fillRect(rowRect, hoverColor);
    }

    // 选中高亮
    if (m_selectionMode != SelectionMode::None && m_selectionModel && m_selectionModel->isSelected(node.index)) {
        QColor selColor = t->primaryColor();
        selColor.setAlphaF(0.08);
        painter->fillRect(rowRect, selColor);

        // 左侧选中指示条
        painter->fillRect(QRect(rowRect.left(), rowRect.top(), 3, rowRect.height()), t->primaryColor());
    }

    // 连接线
    if (m_showLines) {
        paintConnectingLines(painter, visualRow, rowRect);
    }

    // 展开/折叠箭头
    int arrowX = node.depth * m_indent + ARROW_MARGIN;
    int arrowY = rowRect.y() + (rowRect.height() - ARROW_SIZE) / 2;
    if (node.hasChildren) {
        bool arrowHovered = (m_hoveredArrowRow == visualRow);
        paintExpandArrow(painter, arrowX, arrowY, ARROW_SIZE, node.expanded, arrowHovered);
    }

    // 文本
    int textX = arrowX + ARROW_SIZE + TEXT_MARGIN;
    if (!node.hasChildren) {
        // 叶子节点有额外偏移，与箭头对齐
        textX = arrowX + ARROW_SIZE + TEXT_MARGIN;
    }
    QRect textRect(textX, rowRect.y(), rowRect.width() - textX, rowRect.height());

    bool isSelected = m_selectionMode != SelectionMode::None && m_selectionModel && m_selectionModel->isSelected(node.index);
    painter->setPen(isSelected ? t->primaryColor() : t->textColorPrimary());
    painter->setFont(t->bodyFont());

    QString text = m_model ? m_model->data(node.index, Qt::DisplayRole).toString() : QString();
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);

    // 分隔线
    QColor lineColor = t->dividerColor();
    lineColor.setAlphaF(0.3);
    painter->setPen(QPen(lineColor, 1));
    painter->drawLine(rowRect.bottomLeft(), rowRect.bottomRight());
}

void FluentTree::paintExpandArrow(QPainter* painter, int x, int y, int size, bool expanded, bool hovered) {
    auto* t = theme();

    // 悬停背景
    if (hovered) {
        QColor hoverBg = t->textColorPrimary();
        hoverBg.setAlphaF(0.06);
        painter->setPen(Qt::NoPen);
        painter->setBrush(hoverBg);
        painter->drawRoundedRect(QRect(x - 2, y - 2, size + 4, size + 4), 3, 3);
    }

    // 绘制箭头 (▶ / ▼)
    painter->setPen(Qt::NoPen);
    painter->setBrush(t->textColorSecondary());

    qreal cx = x + size / 2.0;
    qreal cy = y + size / 2.0;
    qreal halfSize = size / 4.5;

    QPainterPath arrowPath;
    if (expanded) {
        // ▼ 向下
        arrowPath.moveTo(cx - halfSize, cy - halfSize * 0.7);
        arrowPath.lineTo(cx + halfSize, cy - halfSize * 0.7);
        arrowPath.lineTo(cx, cy + halfSize * 0.9);
    } else {
        // ▶ 向右
        arrowPath.moveTo(cx - halfSize * 0.7, cy - halfSize);
        arrowPath.lineTo(cx + halfSize * 0.9, cy);
        arrowPath.lineTo(cx - halfSize * 0.7, cy + halfSize);
    }
    arrowPath.closeSubpath();
    painter->drawPath(arrowPath);
}

void FluentTree::paintConnectingLines(QPainter* painter, int visualRow, const QRect& rowRect) {
    auto* t = theme();
    QColor lineColor = t->textColorSecondary();
    lineColor.setAlphaF(0.2);
    painter->setPen(QPen(lineColor, 1));

    const auto& node = m_visibleNodes[visualRow];
    int nodeCenterX = node.depth * m_indent + ARROW_MARGIN + ARROW_SIZE / 2;
    int nodeCenterY = rowRect.center().y();

    // 垂直线：从上方兄弟到当前节点
    // 找父节点的最后一个子节点的 visualRow
    if (node.depth > 0) {
        // 垂直线从当前节点中心向上到缩进处
        // 判断是否是父节点的最后一个子节点
        bool isLastChild = true;
        if (visualRow + 1 < m_visibleNodes.size()) {
            const auto& nextNode = m_visibleNodes[visualRow + 1];
            if (nextNode.depth >= node.depth) {
                isLastChild = false;
            }
        }

        // 画水平连接线
        int parentLineX = (node.depth - 1) * m_indent + ARROW_MARGIN + ARROW_SIZE / 2;
        painter->drawLine(nodeCenterX, nodeCenterY, parentLineX + ARROW_SIZE / 2 + TEXT_MARGIN / 2, nodeCenterY);

        // 画垂直线到父节点
        int verticalTop = rowRect.top();
        if (!isLastChild) {
            verticalTop = rowRect.top();
            painter->drawLine(nodeCenterX, rowRect.top(), nodeCenterX, rowRect.bottom());
        } else {
            painter->drawLine(nodeCenterX, rowRect.top(), nodeCenterX, nodeCenterY);
        }
    }
}

void FluentTree::paintScrollBar(QPainter* painter) {
    int totalH = totalContentHeight();
    int viewH = height();
    if (totalH <= viewH) return;

    auto* t = theme();
    int barX = width() - SCROLLBAR_WIDTH - SCROLLBAR_MARGIN;
    int barH = qMax(30, int(viewH * viewH / qreal(totalH)));
    int maxScroll = totalH - viewH;
    int barY = int((viewH - barH) * (m_scrollY / qreal(maxScroll)));

    QRect thumbRect(barX, barY, SCROLLBAR_WIDTH, barH);
    QColor thumbColor = t->textColorPrimary();
    thumbColor.setAlphaF(0.2);
    painter->setPen(Qt::NoPen);
    painter->setBrush(thumbColor);
    painter->drawRoundedRect(thumbRect, SCROLLBAR_WIDTH / 2, SCROLLBAR_WIDTH / 2);
}

// ============================================================
// 事件处理
// ============================================================

void FluentTree::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;
    m_pressedRow = rowAtY(event->pos().y());
}

void FluentTree::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    int row = rowAtY(event->pos().y());
    int x = event->pos().x();

    if (row >= 0 && row == m_pressedRow) {
        // 检查是否点击了箭头
        int arrowRow = arrowRectAt(row, x);
        if (arrowRow >= 0) {
            toggleExpand(row);
        } else {
            updateSelectionFromClick(row, event->modifiers());
            if (m_model) emit clicked(m_visibleNodes[row].index);
        }
    }

    m_pressedRow = -1;
}

void FluentTree::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    int row = rowAtY(event->pos().y());
    if (row >= 0 && m_model) {
        const auto& node = m_visibleNodes[row];
        if (node.hasChildren) {
            toggleExpand(row);
        }
        emit doubleClicked(node.index);
    }
}

void FluentTree::mouseMoveEvent(QMouseEvent* event) {
    int oldRow = m_hoveredRow;
    int oldArrow = m_hoveredArrowRow;

    m_hoveredRow = rowAtY(event->pos().y());
    m_hoveredArrowRow = (m_hoveredRow >= 0) ? arrowRectAt(m_hoveredRow, event->pos().x()) : -1;

    // 更新光标
    if (m_hoveredArrowRow >= 0) {
        setCursor(Qt::PointingHandCursor);
    } else if (m_hoveredRow >= 0) {
        setCursor(m_selectionMode != SelectionMode::None ? Qt::PointingHandCursor : Qt::ArrowCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }

    if (oldRow != m_hoveredRow || oldArrow != m_hoveredArrowRow) {
        update();
    }
}

void FluentTree::wheelEvent(QWheelEvent* event) {
    int delta = event->angleDelta().y();
    int totalH = totalContentHeight();
    int viewH = height();

    if (totalH > viewH) {
        m_scrollY -= delta;
        m_scrollY = qBound(0, m_scrollY, totalH - viewH);
        update();
    }
    event->accept();
}

void FluentTree::keyPressEvent(QKeyEvent* event) {
    if (!m_model || m_visibleNodes.isEmpty()) return FluentWidget::keyPressEvent(event);

    int currentVisual = -1;
    QModelIndex cur = currentIndex();
    for (int i = 0; i < m_visibleNodes.size(); ++i) {
        if (m_visibleNodes[i].index == cur) { currentVisual = i; break; }
    }

    switch (event->key()) {
    case Qt::Key_Up:
        if (currentVisual > 0) {
            updateSelectionFromClick(currentVisual - 1, Qt::NoModifier);
            scrollTo(m_visibleNodes[currentVisual - 1].index);
        } else if (currentVisual < 0 && !m_visibleNodes.isEmpty()) {
            updateSelectionFromClick(0, Qt::NoModifier);
        }
        break;
    case Qt::Key_Down:
        if (currentVisual < m_visibleNodes.size() - 1) {
            updateSelectionFromClick(currentVisual + 1, Qt::NoModifier);
            scrollTo(m_visibleNodes[currentVisual + 1].index);
        }
        break;
    case Qt::Key_Right:
        if (currentVisual >= 0) {
            const auto& node = m_visibleNodes[currentVisual];
            if (node.hasChildren && !node.expanded) expand(node.index);
        }
        break;
    case Qt::Key_Left:
        if (currentVisual >= 0) {
            const auto& node = m_visibleNodes[currentVisual];
            if (node.hasChildren && node.expanded) {
                collapse(node.index);
            } else if (node.depth > 0) {
                // 跳到父节点
                QModelIndex parent = node.index.parent();
                for (int i = 0; i < m_visibleNodes.size(); ++i) {
                    if (m_visibleNodes[i].index == parent) {
                        updateSelectionFromClick(i, Qt::NoModifier);
                        scrollTo(parent);
                        break;
                    }
                }
            }
        }
        break;
    case Qt::Key_Home:
        if (!m_visibleNodes.isEmpty()) {
            updateSelectionFromClick(0, Qt::NoModifier);
            m_scrollY = 0;
            update();
        }
        break;
    case Qt::Key_End:
        if (!m_visibleNodes.isEmpty()) {
            updateSelectionFromClick(m_visibleNodes.size() - 1, Qt::NoModifier);
            scrollTo(m_visibleNodes.last().index);
        }
        break;
    case Qt::Key_Return:
    case Qt::Key_Space:
        if (currentVisual >= 0) {
            const auto& node = m_visibleNodes[currentVisual];
            if (node.hasChildren) toggleExpand(currentVisual);
            else emit doubleClicked(node.index);
        }
        break;
    default:
        FluentWidget::keyPressEvent(event);
    }
}

void FluentTree::resizeEvent(QResizeEvent* event) {
    FluentWidget::resizeEvent(event);
    int totalH = totalContentHeight();
    if (totalH > height()) {
        m_scrollY = qBound(0, m_scrollY, totalH - height());
    } else {
        m_scrollY = 0;
    }
}

void FluentTree::leaveEvent(QEvent* event) {
    m_hoveredRow = -1;
    m_hoveredArrowRow = -1;
    setCursor(Qt::ArrowCursor);
    update();
    FluentWidget::leaveEvent(event);
}

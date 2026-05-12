#include "FluentTable.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QApplication>

// ============================================================
// 构造 / 析构
// ============================================================

FluentTable::FluentTable(QWidget* parent)
    : FluentWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_selectionModel = new QItemSelectionModel(nullptr, this);
    connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &FluentTable::selectionChanged);

    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

FluentTable::~FluentTable() = default;

// ============================================================
// 数据模型
// ============================================================

void FluentTable::setModel(QAbstractItemModel* model) {
    if (m_model) {
        disconnect(m_model, nullptr, this, nullptr);
    }
    m_model = model;
    m_selectionModel->setModel(model);

    if (m_model) {
        connect(m_model, &QAbstractItemModel::rowsInserted, this, [this]() { update(); });
        connect(m_model, &QAbstractItemModel::rowsRemoved, this, [this]() { update(); });
        connect(m_model, &QAbstractItemModel::dataChanged, this, [this]() { update(); });
        connect(m_model, &QAbstractItemModel::layoutChanged, this, [this]() {
            // 同步列信息
            if (m_columns.size() != m_model->columnCount()) {
                m_columns.clear();
                for (int c = 0; c < m_model->columnCount(); ++c) {
                    ColumnInfo info;
                    info.header = m_model->headerData(c, Qt::Horizontal).toString();
                    info.width = 120;
                    info.visible = true;
                    m_columns.append(info);
                }
            }
            update();
        });

        // 初始化列信息
        m_columns.clear();
        for (int c = 0; c < m_model->columnCount(); ++c) {
            ColumnInfo info;
            info.header = m_model->headerData(c, Qt::Horizontal).toString();
            info.width = 120;
            info.visible = true;
            m_columns.append(info);
        }
    } else {
        m_columns.clear();
    }

    m_scrollY = 0;
    m_scrollX = 0;
    update();
}

QAbstractItemModel* FluentTable::model() const { return m_model; }

QItemSelectionModel* FluentTable::selectionModel() const { return m_selectionModel; }

// ============================================================
// 属性 getter/setter
// ============================================================

int FluentTable::rowHeight() const { return m_rowHeight; }
void FluentTable::setRowHeight(int h) {
    if (m_rowHeight == h) return;
    m_rowHeight = h;
    update();
    emit rowHeightChanged();
}

int FluentTable::headerHeight() const { return m_headerHeight; }
void FluentTable::setHeaderHeight(int h) {
    if (m_headerHeight == h) return;
    m_headerHeight = h;
    update();
    emit headerHeightChanged();
}

bool FluentTable::alternatingRowColors() const { return m_alternatingRowColors; }
void FluentTable::setAlternatingRowColors(bool enable) {
    if (m_alternatingRowColors == enable) return;
    m_alternatingRowColors = enable;
    update();
    emit alternatingRowColorsChanged();
}

bool FluentTable::showGrid() const { return m_showGrid; }
void FluentTable::setShowGrid(bool show) {
    if (m_showGrid == show) return;
    m_showGrid = show;
    update();
    emit showGridChanged();
}

bool FluentTable::sortable() const { return m_sortable; }
void FluentTable::setSortable(bool enable) {
    if (m_sortable == enable) return;
    m_sortable = enable;
    emit sortableChanged();
}

FluentTable::SelectionMode FluentTable::selectionMode() const { return m_selectionMode; }
void FluentTable::setSelectionMode(SelectionMode mode) {
    if (m_selectionMode == mode) return;
    m_selectionMode = mode;
    if (mode == SelectionMode::None) {
        clearSelection();
    }
    emit selectionModeChanged();
}

// ============================================================
// 列操作
// ============================================================

void FluentTable::setColumnWidth(int column, int width) {
    if (column < 0 || column >= m_columns.size()) return;
    m_columns[column].width = qMax(40, width);
    update();
}

void FluentTable::setColumnVisible(int column, bool visible) {
    if (column < 0 || column >= m_columns.size()) return;
    m_columns[column].visible = visible;
    update();
}

void FluentTable::setColumnHeader(int column, const QString& text) {
    if (column < 0 || column >= m_columns.size()) return;
    m_columns[column].header = text;
    update();
}

// ============================================================
// 选中
// ============================================================

QModelIndexList FluentTable::selectedIndexes() const {
    return m_selectionModel ? m_selectionModel->selectedIndexes() : QModelIndexList();
}

int FluentTable::currentRow() const {
    auto idx = m_selectionModel ? m_selectionModel->currentIndex() : QModelIndex();
    return idx.isValid() ? idx.row() : -1;
}

void FluentTable::selectRow(int row, bool additive) {
    if (!m_model || m_selectionMode == SelectionMode::None) return;
    if (row < 0 || row >= m_model->rowCount()) return;

    if (!additive && m_selectionMode != SelectionMode::Multi) {
        m_selectionModel->clearSelection();
    }

    for (int c = 0; c < m_model->columnCount(); ++c) {
        QModelIndex idx = m_model->index(row, c);
        m_selectionModel->select(idx, QItemSelectionModel::Select);
    }
    m_selectionModel->setCurrentIndex(m_model->index(row, 0), QItemSelectionModel::Current);
    update();
}

void FluentTable::clearSelection() {
    if (m_selectionModel) m_selectionModel->clearSelection();
    update();
}

void FluentTable::updateSelectionFromClick(int row, int col, Qt::KeyboardModifiers mods) {
    if (!m_model || m_selectionMode == SelectionMode::None) return;

    bool additive = (mods & Qt::ControlModifier) || (mods & Qt::ShiftModifier);

    if (m_selectionMode == SelectionMode::Single) {
        m_selectionModel->clearSelection();
        for (int c = 0; c < m_model->columnCount(); ++c) {
            QModelIndex idx = m_model->index(row, c);
            m_selectionModel->select(idx, QItemSelectionModel::Select);
        }
        m_selectionModel->setCurrentIndex(m_model->index(row, col), QItemSelectionModel::Current);
    } else if (m_selectionMode == SelectionMode::Multi || m_selectionMode == SelectionMode::Extended) {
        if (mods & Qt::ControlModifier) {
            // 切换
            QModelIndex idx = m_model->index(row, 0);
            bool wasSelected = m_selectionModel->isSelected(idx);
            for (int c = 0; c < m_model->columnCount(); ++c) {
                QModelIndex ci = m_model->index(row, c);
                m_selectionModel->select(ci, wasSelected ? QItemSelectionModel::Deselect : QItemSelectionModel::Select);
            }
        } else if (mods & Qt::ShiftModifier && m_selectionMode == SelectionMode::Extended) {
            // 范围选中
            int anchor = m_selectionModel->currentIndex().row();
            int from = qMin(anchor, row);
            int to = qMax(anchor, row);
            m_selectionModel->clearSelection();
            for (int r = from; r <= to; ++r) {
                for (int c = 0; c < m_model->columnCount(); ++c) {
                    m_selectionModel->select(m_model->index(r, c), QItemSelectionModel::Select);
                }
            }
        } else {
            m_selectionModel->clearSelection();
            for (int c = 0; c < m_model->columnCount(); ++c) {
                m_selectionModel->select(m_model->index(row, c), QItemSelectionModel::Select);
            }
            m_selectionModel->setCurrentIndex(m_model->index(row, col), QItemSelectionModel::Current);
        }
    }
    update();
}

// ============================================================
// 排序
// ============================================================

void FluentTable::toggleSort(int column) {
    if (!m_sortable || !m_model) return;

    if (m_sortColumn == column) {
        m_sortOrder = (m_sortOrder == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
    } else {
        m_sortColumn = column;
        m_sortOrder = Qt::AscendingOrder;
    }
    m_model->sort(m_sortColumn, m_sortOrder);
    update();
}

// ============================================================
// 列宽拖拽
// ============================================================

void FluentTable::startColumnResize(int col, int startX) {
    m_isResizing = true;
    m_resizingCol = col;
    m_resizeStartX = startX;
    m_resizeOrigWidth = m_columns[col].width;
    setCursor(Qt::SplitHCursor);
}

void FluentTable::doColumnResize(int currentX) {
    if (!m_isResizing || m_resizingCol < 0) return;
    int delta = currentX - m_resizeStartX;
    m_columns[m_resizingCol].width = qMax(40, m_resizeOrigWidth + delta);
    update();
}

void FluentTable::finishColumnResize() {
    m_isResizing = false;
    m_resizingCol = -1;
    setCursor(Qt::ArrowCursor);
}

// ============================================================
// 滚动
// ============================================================

void FluentTable::scrollToTop() { m_scrollY = 0; update(); }
void FluentTable::scrollToBottom() {
    int maxScroll = qMax(0, totalContentHeight() - contentAreaRect().height());
    m_scrollY = maxScroll;
    update();
}
void FluentTable::scrollToRow(int row) {
    if (!m_model) return;
    int y = row * m_rowHeight - m_scrollY;
    QRect area = contentAreaRect();
    if (y < 0) {
        m_scrollY += y;
    } else if (y + m_rowHeight > area.height()) {
        m_scrollY += (y + m_rowHeight - area.height());
    }
    m_scrollY = qMax(0, m_scrollY);
    update();
}

// ============================================================
// 布局计算
// ============================================================

QSize FluentTable::sizeHint() const { return QSize(600, 400); }
QSize FluentTable::minimumSizeHint() const { return QSize(200, 150); }

int FluentTable::totalContentWidth() const {
    int w = 0;
    for (const auto& col : m_columns) {
        if (col.visible) w += col.width;
    }
    return w;
}

int FluentTable::totalContentHeight() const {
    return m_model ? m_model->rowCount() * m_rowHeight : 0;
}

QRect FluentTable::contentAreaRect() const {
    int bottomMargin = m_vScrollBarVisible ? SCROLLBAR_WIDTH + SCROLLBAR_MARGIN : 0;
    int rightMargin = m_hScrollBarVisible ? SCROLLBAR_WIDTH + SCROLLBAR_MARGIN : 0;
    return QRect(0, m_headerHeight, width() - rightMargin, height() - m_headerHeight - bottomMargin);
}

QRect FluentTable::cellRect(int row, int col) const {
    int x = 0;
    for (int c = 0; c < col; ++c) {
        if (m_columns[c].visible) x += m_columns[c].width;
    }
    int y = m_headerHeight + row * m_rowHeight - m_scrollY;
    return QRect(x - m_scrollX, y, m_columns.value(col).width, m_rowHeight);
}

QRect FluentTable::headerCellRect(int col) const {
    int x = 0;
    for (int c = 0; c < col; ++c) {
        if (m_columns[c].visible) x += m_columns[c].width;
    }
    return QRect(x - m_scrollX, 0, m_columns.value(col).width, m_headerHeight);
}

int FluentTable::columnAtX(int x) const {
    int cx = x + m_scrollX;
    int accum = 0;
    for (int c = 0; c < m_columns.size(); ++c) {
        if (!m_columns[c].visible) continue;
        accum += m_columns[c].width;
        if (cx < accum) return c;
    }
    return -1;
}

int FluentTable::rowAtY(int y) const {
    if (y < m_headerHeight) return -1;
    int row = (y - m_headerHeight + m_scrollY) / m_rowHeight;
    if (m_model && row >= 0 && row < m_model->rowCount()) return row;
    return -1;
}

bool FluentTable::isInHeaderArea(int y) const { return y < m_headerHeight; }

bool FluentTable::isInResizeHandle(int x, int col) const {
    if (col < 0 || col >= m_columns.size()) return false;
    QRect rect = headerCellRect(col);
    return qAbs(x - rect.right()) <= RESIZE_HANDLE_WIDTH / 2;
}

// ============================================================
// 绘制
// ============================================================

void FluentTable::paintFluent(QPainter* painter) {
    auto* t = theme();

    // 背景
    painter->fillRect(rect(), t->cardColor());

    // 计算滚动条可见性
    QRect contentArea = contentAreaRect();
    m_vScrollBarVisible = totalContentHeight() > contentArea.height();
    m_hScrollBarVisible = totalContentWidth() > contentArea.width();
    // 重新计算（因为滚动条可见性会影响 contentArea）
    contentArea = contentAreaRect();

    // 绘制表头
    paintHeader(painter, QRect(0, 0, width(), m_headerHeight));

    // 绘制内容区
    paintBody(painter, contentArea);

    // 绘制滚动条
    paintScrollBars(painter);

    // 外边框
    painter->setPen(QPen(t->borderColor(), t->borderWidth()));
    painter->setBrush(Qt::NoBrush);
    QPainterPath borderPath;
    borderPath.addRoundedRect(rect(), t->cornerRadiusSmall(), t->cornerRadiusSmall());
    painter->drawPath(borderPath);
}

void FluentTable::paintHeader(QPainter* painter, const QRect& rect) {
    auto* t = theme();

    // 表头背景
    painter->fillRect(rect, t->surfaceColor());

    // 表头底部线
    painter->setPen(QPen(t->dividerColor(), 1));
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());

    // 各列表头单元格
    for (int c = 0; c < m_columns.size(); ++c) {
        if (!m_columns[c].visible) continue;
        QRect cellRect = headerCellRect(c);
        if (cellRect.right() < 0 || cellRect.left() > width()) continue;
        paintHeaderCell(painter, c, cellRect);
    }
}

void FluentTable::paintHeaderCell(QPainter* painter, int col, const QRect& cellRect) {
    auto* t = theme();

    // 排序指示
    bool isSortCol = (m_sortColumn == col);

    // 悬停高亮
    QPoint cursorPos = mapFromGlobal(QCursor::pos());
    if (m_hoveredCol == col && isInHeaderArea(cursorPos.y())) {
        QColor hoverColor = t->textColorPrimary();
        hoverColor.setAlphaF(0.04);
        painter->fillRect(cellRect, hoverColor);
    }

    // 文本
    painter->setPen(t->textColorSecondary());
    painter->setFont(t->bodyStrongFont());
    int textMargin = 12;
    QRect textRect = cellRect.adjusted(textMargin, 0, -textMargin - (isSortCol ? 16 : 0), 0);
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, m_columns[col].header);

    // 排序箭头
    if (isSortCol) {
        painter->setPen(t->textColorSecondary());
        QFont arrowFont = t->captionFont();
        painter->setFont(arrowFont);
        QRect arrowRect(cellRect.right() - textMargin - 14, cellRect.y(), 14, cellRect.height());
        painter->drawText(arrowRect, Qt::AlignVCenter | Qt::AlignCenter,
                          m_sortOrder == Qt::AscendingOrder ? u8"▲" : u8"▼");
    }

    // 列分隔线（不在最后一列）
    if (col < m_columns.size() - 1) {
        QColor sepColor = t->dividerColor();
        sepColor.setAlphaF(0.5);
        painter->setPen(QPen(sepColor, 1));
        painter->drawLine(cellRect.topRight(), cellRect.bottomRight());
    }
}

void FluentTable::paintBody(QPainter* painter, const QRect& area) {
    if (!m_model) return;

    auto* t = theme();

    // 裁剪绘制区域
    painter->save();
    painter->setClipRect(area);

    int firstRow = m_scrollY / m_rowHeight;
    int lastRow = qMin(m_model->rowCount() - 1, (m_scrollY + area.height()) / m_rowHeight);

    for (int r = firstRow; r <= lastRow; ++r) {
        int y = m_headerHeight + r * m_rowHeight - m_scrollY;
        QRect rowRect(area.x(), y, area.width(), m_rowHeight);
        paintRow(painter, r, rowRect);
    }

    painter->restore();
}

void FluentTable::paintRow(QPainter* painter, int row, const QRect& rowRect) {
    auto* t = theme();

    // 交替行色
    if (m_alternatingRowColors && row % 2 == 1) {
        QColor altColor = t->textColorPrimary();
        altColor.setAlphaF(0.02);
        painter->fillRect(rowRect, altColor);
    }

    // 悬停行高亮
    if (m_hoveredRow == row) {
        QColor hoverColor = t->textColorPrimary();
        hoverColor.setAlphaF(0.05);
        painter->fillRect(rowRect, hoverColor);
    }

    // 选中行高亮
    QModelIndex firstIdx = m_model->index(row, 0);
    if (m_selectionMode != SelectionMode::None && m_selectionModel && m_selectionModel->isSelected(firstIdx)) {
        QColor selColor = t->primaryColor();
        selColor.setAlphaF(0.08);
        painter->fillRect(rowRect, selColor);
    }

    // 各单元格
    for (int c = 0; c < m_columns.size(); ++c) {
        if (!m_columns[c].visible) continue;
        QRect cellR = cellRect(row, c);
        if (cellR.right() < 0 || cellR.left() > width()) continue;
        paintCell(painter, row, c, cellR);
    }

    // 网格线 - 水平
    if (m_showGrid) {
        painter->setPen(QPen(t->dividerColor(), 1));
        painter->drawLine(rowRect.bottomLeft(), rowRect.bottomRight());
    }
}

void FluentTable::paintCell(QPainter* painter, int row, int col, const QRect& cellRect) {
    auto* t = theme();

    // 网格线 - 垂直
    if (m_showGrid && col < m_columns.size() - 1) {
        QColor vLineColor = t->dividerColor();
        vLineColor.setAlphaF(0.4);
        painter->setPen(QPen(vLineColor, 1));
        painter->drawLine(cellRect.topRight(), cellRect.bottomRight());
    }

    // 文本
    if (!m_model) return;
    QModelIndex idx = m_model->index(row, col);
    QString text = m_model->data(idx, Qt::DisplayRole).toString();
    Qt::Alignment align = Qt::Alignment(m_model->data(idx, Qt::TextAlignmentRole).toInt());
    if (align == 0) align = Qt::AlignVCenter | Qt::AlignLeft;

    bool isSelected = m_selectionMode != SelectionMode::None && m_selectionModel && m_selectionModel->isSelected(idx);
    painter->setPen(isSelected ? t->primaryColor() : t->textColorPrimary());
    painter->setFont(t->bodyFont());
    int textMargin = 12;
    QRect textRect = cellRect.adjusted(textMargin, 0, -textMargin, 0);
    painter->drawText(textRect, align, text);
}

void FluentTable::paintScrollBars(QPainter* painter) {
    auto* t = theme();
    QRect content = contentAreaRect();

    // 垂直滚动条
    if (m_vScrollBarVisible) {
        int totalH = totalContentHeight();
        int viewH = content.height();
        if (totalH > viewH) {
            int barX = width() - SCROLLBAR_WIDTH - SCROLLBAR_MARGIN;
            int barH = qMax(30, int(viewH * viewH / qreal(totalH)));
            int barY = m_headerHeight + int((viewH - barH) * (m_scrollY / qreal(totalH - viewH)));

            QRect trackRect(barX, m_headerHeight, SCROLLBAR_WIDTH, viewH);
            painter->fillRect(trackRect, Qt::transparent);

            QRect thumbRect(barX, barY, SCROLLBAR_WIDTH, barH);
            QColor thumbColor = t->textColorPrimary();
            thumbColor.setAlphaF(0.2);
            painter->setPen(Qt::NoPen);
            painter->setBrush(thumbColor);
            painter->drawRoundedRect(thumbRect, SCROLLBAR_WIDTH / 2, SCROLLBAR_WIDTH / 2);
        }
    }

    // 水平滚动条
    if (m_hScrollBarVisible) {
        int totalW = totalContentWidth();
        int viewW = content.width();
        if (totalW > viewW) {
            int barY = height() - SCROLLBAR_WIDTH - SCROLLBAR_MARGIN;
            int barW = qMax(30, int(viewW * viewW / qreal(totalW)));
            int barX = int((viewW - barW) * (m_scrollX / qreal(totalW - viewW)));

            QRect trackRect(0, barY, viewW, SCROLLBAR_WIDTH);
            painter->fillRect(trackRect, Qt::transparent);

            QRect thumbRect(barX, barY, barW, SCROLLBAR_WIDTH);
            QColor thumbColor = t->textColorPrimary();
            thumbColor.setAlphaF(0.2);
            painter->setPen(Qt::NoPen);
            painter->setBrush(thumbColor);
            painter->drawRoundedRect(thumbRect, SCROLLBAR_WIDTH / 2, SCROLLBAR_WIDTH / 2);
        }
    }
}

// ============================================================
// 事件处理
// ============================================================

void FluentTable::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    int x = event->pos().x();
    int y = event->pos().y();

    // 列宽拖拽
    if (isInHeaderArea(y)) {
        int col = columnAtX(x);
        if (col >= 0 && isInResizeHandle(x, col)) {
            startColumnResize(col, x);
            return;
        }
    }

    m_pressedRow = rowAtY(y);
    m_pressedCol = columnAtX(x);
}

void FluentTable::mouseReleaseEvent(QMouseEvent* event) {
    if (m_isResizing) {
        finishColumnResize();
        return;
    }

    if (event->button() != Qt::LeftButton) return;

    int x = event->pos().x();
    int y = event->pos().y();

    if (isInHeaderArea(y) && m_sortable) {
        int col = columnAtX(x);
        if (col >= 0 && !isInResizeHandle(x, col)) {
            toggleSort(col);
        }
    } else {
        int row = rowAtY(y);
        int col = columnAtX(x);
        if (row >= 0 && col >= 0) {
            updateSelectionFromClick(row, col, event->modifiers());
            emit clicked(m_model ? m_model->index(row, col) : QModelIndex());
        }
    }

    m_pressedRow = -1;
    m_pressedCol = -1;
}

void FluentTable::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    int row = rowAtY(event->pos().y());
    int col = columnAtX(event->pos().x());
    if (row >= 0 && col >= 0 && m_model) {
        emit doubleClicked(m_model->index(row, col));
    }
}

void FluentTable::mouseMoveEvent(QMouseEvent* event) {
    int x = event->pos().x();
    int y = event->pos().y();

    // 列宽拖拽中
    if (m_isResizing) {
        doColumnResize(x);
        return;
    }

    // 更新悬停行/列
    int oldRow = m_hoveredRow;
    int oldCol = m_hoveredCol;
    m_hoveredRow = rowAtY(y);
    m_hoveredCol = columnAtX(x);

    // 表头区域拖拽光标
    if (isInHeaderArea(y)) {
        int col = columnAtX(x);
        if (col >= 0 && isInResizeHandle(x, col)) {
            setCursor(Qt::SplitHCursor);
        } else if (m_sortable && col >= 0) {
            setCursor(Qt::PointingHandCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    } else {
        setCursor(m_hoveredRow >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
    }

    if (oldRow != m_hoveredRow || oldCol != m_hoveredCol) {
        update();
    }
}

void FluentTable::wheelEvent(QWheelEvent* event) {
    int delta = event->angleDelta().y();
    int contentH = totalContentHeight();
    int viewH = contentAreaRect().height();

    if (contentH > viewH) {
        m_scrollY -= delta;
        m_scrollY = qBound(0, m_scrollY, contentH - viewH);
        update();
    }
    event->accept();
}

void FluentTable::keyPressEvent(QKeyEvent* event) {
    if (!m_model) return FluentWidget::keyPressEvent(event);

    int current = currentRow();

    switch (event->key()) {
    case Qt::Key_Up:
        if (current > 0) {
            selectRow(current - 1, false);
            scrollToRow(current - 1);
        }
        break;
    case Qt::Key_Down:
        if (current < m_model->rowCount() - 1) {
            selectRow(current + 1, false);
            scrollToRow(current + 1);
        }
        break;
    case Qt::Key_Home:
        if (m_model->rowCount() > 0) {
            selectRow(0, false);
            scrollToTop();
        }
        break;
    case Qt::Key_End:
        if (m_model->rowCount() > 0) {
            selectRow(m_model->rowCount() - 1, false);
            scrollToBottom();
        }
        break;
    case Qt::Key_Space:
    case Qt::Key_Return:
        if (current >= 0) {
            emit doubleClicked(m_model->index(current, 0));
        }
        break;
    default:
        FluentWidget::keyPressEvent(event);
    }
}

void FluentTable::resizeEvent(QResizeEvent* event) {
    FluentWidget::resizeEvent(event);

    // 限制滚动偏移
    int contentH = totalContentHeight();
    int viewH = contentAreaRect().height();
    if (contentH > viewH) {
        m_scrollY = qBound(0, m_scrollY, contentH - viewH);
    } else {
        m_scrollY = 0;
    }

    int contentW = totalContentWidth();
    int viewW = contentAreaRect().width();
    if (contentW > viewW) {
        m_scrollX = qBound(0, m_scrollX, contentW - viewW);
    } else {
        m_scrollX = 0;
    }
}

void FluentTable::leaveEvent(QEvent* event) {
    m_hoveredRow = -1;
    m_hoveredCol = -1;
    if (m_isResizing) finishColumnResize();
    setCursor(Qt::ArrowCursor);
    update();
    FluentWidget::leaveEvent(event);
}

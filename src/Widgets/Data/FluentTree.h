#ifndef FLUENTTREE_H
#define FLUENTTREE_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QAbstractItemModel>
#include <QItemSelectionModel>

// Fluent UI 风格树型控件
// 特性：展开/折叠箭头、缩进、节点图标、行悬停/选中高亮、自定义滚动条、主题适配
class FLUENT_EXPORT FluentTree : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(int rowHeight READ rowHeight WRITE setRowHeight NOTIFY rowHeightChanged)
    Q_PROPERTY(int indent READ indent WRITE setIndent NOTIFY indentChanged)
    Q_PROPERTY(bool showLines READ showLines WRITE setShowLines NOTIFY showLinesChanged)
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)

public:
    enum class SelectionMode {
        None,       // 无选中
        Single,     // 单选
        Multi       // 多选 (Ctrl)
    };
    Q_ENUM(SelectionMode)

    explicit FluentTree(QWidget* parent = nullptr);
    ~FluentTree() override;

    // 数据模型
    void setModel(QAbstractItemModel* model);
    QAbstractItemModel* model() const;
    QItemSelectionModel* selectionModel() const;

    // 行高
    int rowHeight() const;
    void setRowHeight(int h);

    // 缩进
    int indent() const;
    void setIndent(int px);

    // 连接线
    bool showLines() const;
    void setShowLines(bool show);

    // 选中模式
    SelectionMode selectionMode() const;
    void setSelectionMode(SelectionMode mode);

    // 展开/折叠
    void expand(const QModelIndex& index);
    void collapse(const QModelIndex& index);
    void expandAll();
    void collapseAll();
    bool isExpanded(const QModelIndex& index) const;

    // 选中
    QModelIndex currentIndex() const;
    QModelIndexList selectedIndexes() const;
    void clearSelection();

    // 滚动
    void scrollTo(const QModelIndex& index);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void rowHeightChanged();
    void indentChanged();
    void showLinesChanged();
    void selectionModeChanged();

    void clicked(const QModelIndex& index);
    void doubleClicked(const QModelIndex& index);
    void expanded(const QModelIndex& index);
    void collapsed(const QModelIndex& index);
    void selectionChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    // 可见行（扁平化后的展开节点列表）
    struct VisibleNode {
        QModelIndex index;
        int depth = 0;
        bool expanded = false;
        bool hasChildren = false;
    };

    // 重建可见行列表
    void rebuildVisibleNodes();

    // 布局
    int totalContentHeight() const;
    QRect contentRect() const;
    int rowAtY(int y) const;
    int arrowRectAt(int row, int x) const;  // 返回箭头点击区域，-1 表示不在箭头上

    // 绘制子过程
    void paintRow(QPainter* painter, int visualRow, const QRect& rowRect);
    void paintExpandArrow(QPainter* painter, int x, int y, int size, bool expanded, bool hovered);
    void paintConnectingLines(QPainter* painter, int visualRow, const QRect& rowRect);
    void paintScrollBar(QPainter* painter);

    // 选中
    void updateSelectionFromClick(int visualRow, Qt::KeyboardModifiers mods);

    // 展开/折叠
    void toggleExpand(int visualRow);

    QAbstractItemModel* m_model = nullptr;
    QItemSelectionModel* m_selectionModel = nullptr;
    QList<VisibleNode> m_visibleNodes;

    int m_rowHeight = 32;
    int m_indent = 24;
    bool m_showLines = false;
    SelectionMode m_selectionMode = SelectionMode::Single;

    // 滚动
    int m_scrollY = 0;

    // 交互状态
    int m_hoveredRow = -1;
    int m_pressedRow = -1;
    int m_hoveredArrowRow = -1;  // 鼠标悬停在箭头上的行

    // 展开/折叠集合：存储持久化索引的 internalId
    QSet<qint64> m_expandedIds;

    enum { ARROW_SIZE = 12, ARROW_MARGIN = 8, TEXT_MARGIN = 4,
           SCROLLBAR_WIDTH = 8, SCROLLBAR_MARGIN = 2 };
};

#endif // FLUENTTREE_H

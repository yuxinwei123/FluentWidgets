#ifndef FLUENTTABLE_H
#define FLUENTTABLE_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QHeaderView>

class QScrollBar;
class FluentTablePrivate;

// Fluent UI 风格表格组件
// 特性：自定义绘制表头/行、交替行色、行选中高亮、列排序、列宽拖拽、主题适配
class FLUENT_EXPORT FluentTable : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(int rowHeight READ rowHeight WRITE setRowHeight NOTIFY rowHeightChanged)
    Q_PROPERTY(int headerHeight READ headerHeight WRITE setHeaderHeight NOTIFY headerHeightChanged)
    Q_PROPERTY(bool alternatingRowColors READ alternatingRowColors WRITE setAlternatingRowColors NOTIFY alternatingRowColorsChanged)
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid NOTIFY showGridChanged)
    Q_PROPERTY(bool sortable READ sortable WRITE setSortable NOTIFY sortableChanged)
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)

public:
    // 选中模式
    enum class SelectionMode {
        None,           // 无选中
        Single,         // 单行选中
        Multi,          // 多行选中 (Ctrl+点击)
        Extended        // 扩展选中 (Shift/Ctrl)
    };
    Q_ENUM(SelectionMode)

    explicit FluentTable(QWidget* parent = nullptr);
    ~FluentTable() override;

    // 数据模型
    void setModel(QAbstractItemModel* model);
    QAbstractItemModel* model() const;

    // 选中模型
    QItemSelectionModel* selectionModel() const;

    // 行高 / 表头高
    int rowHeight() const;
    void setRowHeight(int height);

    int headerHeight() const;
    void setHeaderHeight(int height);

    // 交替行颜色
    bool alternatingRowColors() const;
    void setAlternatingRowColors(bool enable);

    // 网格线
    bool showGrid() const;
    void setShowGrid(bool show);

    // 排序
    bool sortable() const;
    void setSortable(bool enable);

    // 选中模式
    SelectionMode selectionMode() const;
    void setSelectionMode(SelectionMode mode);

    // 列操作
    void setColumnWidth(int column, int width);
    void setColumnVisible(int column, bool visible);
    void setColumnHeader(int column, const QString& text);

    // 选中行
    QModelIndexList selectedIndexes() const;
    int currentRow() const;

    // 滚动
    void scrollToTop();
    void scrollToBottom();
    void scrollToRow(int row);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void rowHeightChanged();
    void headerHeightChanged();
    void alternatingRowColorsChanged();
    void showGridChanged();
    void sortableChanged();
    void selectionModeChanged();

    void clicked(const QModelIndex& index);
    void doubleClicked(const QModelIndex& index);
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
    friend class FluentTablePrivate;
    FluentTablePrivate* d;

    // 布局计算
    int totalContentWidth() const;
    int totalContentHeight() const;
    QRect cellRect(int row, int col) const;
    QRect headerCellRect(int col) const;
    QRect contentAreaRect() const;

    // 命中测试
    int columnAtX(int x) const;
    int rowAtY(int y) const;
    bool isInHeaderArea(int y) const;
    bool isInResizeHandle(int x, int col) const;

    // 绘制子过程
    void paintHeader(QPainter* painter, const QRect& rect);
    void paintBody(QPainter* painter, const QRect& rect);
    void paintRow(QPainter* painter, int row, const QRect& rowRect);
    void paintCell(QPainter* painter, int row, int col, const QRect& cellRect);
    void paintHeaderCell(QPainter* painter, int col, const QRect& cellRect);
    void paintScrollBars(QPainter* painter);

    // 选中
    void selectRow(int row, bool additive);
    void clearSelection();
    void updateSelectionFromClick(int row, int col, Qt::KeyboardModifiers mods);

    // 排序
    void toggleSort(int column);

    // 列宽拖拽
    void startColumnResize(int col, int startX);
    void doColumnResize(int currentX);
    void finishColumnResize();

    struct ColumnInfo {
        QString header;
        int width = 120;
        bool visible = true;
    };

    QList<ColumnInfo> m_columns;
    QAbstractItemModel* m_model = nullptr;
    QItemSelectionModel* m_selectionModel = nullptr;

    int m_rowHeight = 36;
    int m_headerHeight = 32;
    bool m_alternatingRowColors = true;
    bool m_showGrid = true;
    bool m_sortable = false;
    SelectionMode m_selectionMode = SelectionMode::Single;

    // 滚动偏移
    int m_scrollX = 0;
    int m_scrollY = 0;

    // 交互状态
    int m_hoveredRow = -1;
    int m_hoveredCol = -1;
    int m_pressedRow = -1;
    int m_pressedCol = -1;

    // 排序状态
    int m_sortColumn = -1;
    Qt::SortOrder m_sortOrder = Qt::AscendingOrder;

    // 列宽拖拽
    int m_resizingCol = -1;
    int m_resizeStartX = 0;
    int m_resizeOrigWidth = 0;
    bool m_isResizing = false;

    // 滚动条
    bool m_vScrollBarVisible = false;
    bool m_hScrollBarVisible = false;
    int m_vScrollValue = 0;
    int m_hScrollValue = 0;

    enum { SCROLLBAR_WIDTH = 8, SCROLLBAR_MARGIN = 2, RESIZE_HANDLE_WIDTH = 6 };
};

#endif // FLUENTTABLE_H

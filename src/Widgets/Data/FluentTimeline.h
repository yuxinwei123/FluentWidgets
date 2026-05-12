#ifndef FLUENTTIMELINE_H
#define FLUENTTIMELINE_H

#include "Core/FluentGlobal.h"
#include <QWidget>

class FLUENT_EXPORT FluentTimeline : public QWidget {
    Q_OBJECT

public:
    // 方向
    enum Orientation {
        Vertical,   // 垂直（默认）
        Horizontal  // 水平
    };
    Q_ENUM(Orientation)

    // 节点样式
    enum NodeStyle {
        Dot,        // 圆点
        Ring,       // 空心圆环
        Icon,       // 图标（自定义绘制）
        Diamond     // 菱形
    };
    Q_ENUM(NodeStyle)

    // 历程项
    struct Item {
        QString title;
        QString description;
        QString timestamp;
        QColor color;           // 节点颜色（可不同位置不同颜色）
        NodeStyle nodeStyle = Dot;
        bool selected = false;
    };

    explicit FluentTimeline(QWidget* parent = nullptr);

    // 方向
    Orientation orientation() const;
    void setOrientation(Orientation orient);

    // 框选
    bool selectionEnabled() const;
    void setSelectionEnabled(bool enabled);

    // 选中项
    int selectedIndex() const;
    void setSelectedIndex(int index);
    QVector<int> selectedIndexes() const;
    void setSelectedIndexes(const QVector<int>& indexes);
    void clearSelection();

    // 添加/删除项
    void addItem(const Item& item);
    void addItem(const QString& title, const QString& description = QString(),
                 const QString& timestamp = QString(), const QColor& color = QColor(),
                 NodeStyle style = Dot);
    void insertItem(int index, const Item& item);
    void removeItem(int index);
    void clearItems();

    // 访问项
    int itemCount() const;
    Item item(int index) const;
    QVector<Item> items() const;
    void updateItem(int index, const Item& item);

    // 间距
    int itemSpacing() const;
    void setItemSpacing(int spacing);

signals:
    void itemClicked(int index);
    void selectionChanged(const QVector<int>& indexes);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void drawVertical(QPainter& p);
    void drawHorizontal(QPainter& p);
    int itemAtPos(const QPoint& pos) const;
    void updateGeometry();

    QVector<Item> m_items;
    Orientation m_orientation = Vertical;
    bool m_selectionEnabled = true;

    int m_itemSpacing = 60;
    int m_nodeRadius = 8;
    int m_lineWidth = 3;
    int m_contentMargin = 16;

    // 框选状态
    bool m_rubberBanding = false;
    QPoint m_rubberStart;
    QPoint m_rubberCurrent;

    // 缓存
    QVector<QRect> m_itemRects;   // 每个项的点击区域
    int m_hoverIndex = -1;
};

#endif // FLUENTTIMELINE_H

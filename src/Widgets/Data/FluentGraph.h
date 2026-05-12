#ifndef FLUENTGRAPH_H
#define FLUENTGRAPH_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QPointF>
#include <QRectF>
#include <QMap>
#include <QSet>

// Fluent UI 风格节点连接图控件
// 特性：节点拖拽、连接线(贝塞尔曲线)、端口连接、缩放/平移、选中高亮、主题适配

// ========== 数据结构 ==========

// 连接端口（节点上的输入/输出锚点）
struct FLUENT_EXPORT FluentGraphPort {
    enum class Direction { In, Out };
    QString name;
    Direction direction = Direction::In;
    QPointF localPos;   // 相对于节点的偏移
};

// 图节点
class FLUENT_EXPORT FluentGraphNode {
public:
    explicit FluentGraphNode(const QString& id, const QString& title = QString());
    ~FluentGraphNode() = default;

    QString id() const;
    void setTitle(const QString& title);
    QString title() const;

    void setPosition(const QPointF& pos);
    QPointF position() const;
    void setSize(const QSizeF& size);
    QSizeF size() const;
    QRectF rect() const;

    // 端口
    void addPort(const QString& name, FluentGraphPort::Direction dir);
    QList<FluentGraphPort> ports() const;
    QList<FluentGraphPort> inPorts() const;
    QList<FluentGraphPort> outPorts() const;
    QPointF portScenePos(const QString& portName) const;

    // 颜色标签（自定义节点顶部颜色条）
    void setColorTag(const QColor& color);
    QColor colorTag() const;

    // 选中
    void setSelected(bool selected);
    bool isSelected() const;

private:
    QString m_id;
    QString m_title;
    QPointF m_pos;
    QSizeF m_size = QSizeF(160, 80);
    QMap<QString, FluentGraphPort> m_ports;
    QColor m_colorTag;
    bool m_selected = false;
};

// 图边（连接两个端口）
class FLUENT_EXPORT FluentGraphEdge {
public:
    explicit FluentGraphEdge(const QString& fromNode, const QString& fromPort,
                             const QString& toNode, const QString& toPort);
    ~FluentGraphEdge() = default;

    QString fromNode() const;
    QString fromPort() const;
    QString toNode() const;
    QString toPort() const;

    void setSelected(bool selected);
    bool isSelected() const;

    // 计算贝塞尔路径的起终点
    QPointF startPoint() const;
    QPointF endPoint() const;

private:
    QString m_fromNode, m_fromPort;
    QString m_toNode, m_toPort;
    bool m_selected = false;
};

// ========== 控件 ==========

class FLUENT_EXPORT FluentGraph : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(bool editable READ editable WRITE setEditable NOTIFY editableChanged)
    Q_PROPERTY(ConnectionStyle connectionStyle READ connectionStyle WRITE setConnectionStyle NOTIFY connectionStyleChanged)

public:
    // 连接线样式
    enum class ConnectionStyle {
        Bezier,     // 贝塞尔曲线（默认）
        Straight,   // 直线
        Step        // 阶梯折线
    };
    Q_ENUM(ConnectionStyle)

    explicit FluentGraph(QWidget* parent = nullptr);
    ~FluentGraph() override;

    // 节点操作
    FluentGraphNode* addNode(const QString& id, const QString& title = QString());
    void removeNode(const QString& id);
    FluentGraphNode* node(const QString& id) const;
    QList<FluentGraphNode*> nodes() const;

    // 边操作
    FluentGraphEdge* addEdge(const QString& fromNode, const QString& fromPort,
                             const QString& toNode, const QString& toPort);
    void removeEdge(const QString& fromNode, const QString& fromPort,
                    const QString& toNode, const QString& toPort);
    QList<FluentGraphEdge*> edges() const;

    // 选中
    QList<FluentGraphNode*> selectedNodes() const;
    QList<FluentGraphEdge*> selectedEdges() const;
    void clearSelection();

    // 缩放
    qreal zoom() const;
    void setZoom(qreal factor);
    void resetZoom();

    // 平移
    QPointF panOffset() const;
    void setPanOffset(const QPointF& offset);
    void resetPan();

    // 编辑模式
    bool editable() const;
    void setEditable(bool editable);

    // 连接线样式
    ConnectionStyle connectionStyle() const;
    void setConnectionStyle(ConnectionStyle style);

    // 布局辅助：自动布局
    void layoutHorizontal(int spacingX = 250, int spacingY = 120);
    void layoutVertical(int spacingX = 200, int spacingY = 150);

    // 坐标转换
    QPointF mapFromScene(const QPointF& scenePos) const;
    QPointF mapToScene(const QPointF& widgetPos) const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void zoomChanged();
    void editableChanged();
    void connectionStyleChanged();
    void nodeMoved(const QString& id, const QPointF& pos);
    void nodeClicked(const QString& id);
    void nodeDoubleClicked(const QString& id);
    void edgeClicked(const QString& fromNode, const QString& fromPort,
                     const QString& toNode, const QString& toPort);
    void edgeCreated(const QString& fromNode, const QString& fromPort,
                     const QString& toNode, const QString& toPort);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    // 绘制子过程
    void paintGrid(QPainter* painter);
    void paintEdges(QPainter* painter);
    void paintEdge(QPainter* painter, FluentGraphEdge* edge);
    void paintNodes(QPainter* painter);
    void paintNode(QPainter* painter, FluentGraphNode* node);
    void paintPorts(QPainter* painter, FluentGraphNode* node);
    void paintTempConnection(QPainter* painter);  // 拖拽创建连接时的临时线

    // 命中测试
    FluentGraphNode* nodeAt(const QPointF& scenePos) const;
    FluentGraphEdge* edgeAt(const QPointF& scenePos) const;
    QString portAt(FluentGraphNode* node, const QPointF& scenePos) const;

    // 交互状态
    enum class Interaction { None, DragNode, Pan, ConnectPort };
    Interaction m_interaction = Interaction::None;

    QString m_dragNodeId;
    QPointF m_dragStartScene;
    QPointF m_dragNodeOrigPos;

    QPointF m_panStart;
    QPointF m_panOrigOffset;

    QString m_connectFromNode;
    QString m_connectFromPort;
    QPointF m_connectTempEnd;     // 临时连接线终点(scene坐标)

    // 数据
    QMap<QString, FluentGraphNode*> m_nodes;
    QList<FluentGraphEdge*> m_edges;

    // 视图变换
    qreal m_zoom = 1.0;
    QPointF m_panOffset;
    bool m_editable = true;
    ConnectionStyle m_connectionStyle = ConnectionStyle::Bezier;

    // 悬停
    QString m_hoveredNodeId;
    QString m_hoveredPortName;
    QString m_hoveredPortNode;

    enum { PORT_RADIUS = 5, NODE_RADIUS = 6, GRID_SIZE = 20 };
};

#endif // FLUENTGRAPH_H

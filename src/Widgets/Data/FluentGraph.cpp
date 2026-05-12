#include "FluentGraph.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QApplication>
#include <cmath>

// ============================================================
// FluentGraphNode
// ============================================================

FluentGraphNode::FluentGraphNode(const QString& id, const QString& title)
    : m_id(id), m_title(title.isEmpty() ? id : title)
{
    // 根据端口数量自动调整高度
}

QString FluentGraphNode::id() const { return m_id; }
void FluentGraphNode::setTitle(const QString& title) { m_title = title; }
QString FluentGraphNode::title() const { return m_title; }

void FluentGraphNode::setPosition(const QPointF& pos) { m_pos = pos; }
QPointF FluentGraphNode::position() const { return m_pos; }
void FluentGraphNode::setSize(const QSizeF& size) { m_size = size; }
QSizeF FluentGraphNode::size() const { return m_size; }
QRectF FluentGraphNode::rect() const { return QRectF(m_pos, m_size); }

void FluentGraphNode::addPort(const QString& name, FluentGraphPort::Direction dir) {
    FluentGraphPort port;
    port.name = name;
    port.direction = dir;

    // 计算端口局部位置
    int inCount = inPorts().size();
    int outCount = outPorts().size();

    if (dir == FluentGraphPort::Direction::In) {
        // 左侧，垂直等分
        qreal step = m_size.height() / (inCount + 2);
        port.localPos = QPointF(0, step * (inCount + 1));
        // 重新分配已有的 in 端口位置
        int idx = 0;
        for (auto& p : m_ports) {
            if (p.direction == FluentGraphPort::Direction::In) {
                idx++;
                p.localPos = QPointF(0, step * idx);
            }
        }
        port.localPos = QPointF(0, step * (idx + 1));
    } else {
        // 右侧
        qreal step = m_size.height() / (outCount + 2);
        int idx = 0;
        for (auto& p : m_ports) {
            if (p.direction == FluentGraphPort::Direction::Out) {
                idx++;
                p.localPos = QPointF(m_size.width(), step * idx);
            }
        }
        port.localPos = QPointF(m_size.width(), step * (idx + 1));
    }

    m_ports[name] = port;
}

QList<FluentGraphPort> FluentGraphNode::ports() const { return m_ports.values(); }

QList<FluentGraphPort> FluentGraphNode::inPorts() const {
    QList<FluentGraphPort> result;
    for (const auto& p : m_ports) {
        if (p.direction == FluentGraphPort::Direction::In) result.append(p);
    }
    return result;
}

QList<FluentGraphPort> FluentGraphNode::outPorts() const {
    QList<FluentGraphPort> result;
    for (const auto& p : m_ports) {
        if (p.direction == FluentGraphPort::Direction::Out) result.append(p);
    }
    return result;
}

QPointF FluentGraphNode::portScenePos(const QString& portName) const {
    auto it = m_ports.constFind(portName);
    if (it != m_ports.constEnd()) {
        return m_pos + it->localPos;
    }
    return m_pos;
}

void FluentGraphNode::setColorTag(const QColor& color) { m_colorTag = color; }
QColor FluentGraphNode::colorTag() const { return m_colorTag; }

void FluentGraphNode::setSelected(bool selected) { m_selected = selected; }
bool FluentGraphNode::isSelected() const { return m_selected; }

// ============================================================
// FluentGraphEdge
// ============================================================

FluentGraphEdge::FluentGraphEdge(const QString& fromNode, const QString& fromPort,
                                 const QString& toNode, const QString& toPort)
    : m_fromNode(fromNode), m_fromPort(fromPort)
    , m_toNode(toNode), m_toPort(toPort)
{
}

QString FluentGraphEdge::fromNode() const { return m_fromNode; }
QString FluentGraphEdge::fromPort() const { return m_fromPort; }
QString FluentGraphEdge::toNode() const { return m_toNode; }
QString FluentGraphEdge::toPort() const { return m_toPort; }

void FluentGraphEdge::setSelected(bool selected) { m_selected = selected; }
bool FluentGraphEdge::isSelected() const { return m_selected; }

QPointF FluentGraphEdge::startPoint() const { return QPointF(); }  // 由 FluentGraph 计算
QPointF FluentGraphEdge::endPoint() const { return QPointF(); }

// ============================================================
// FluentGraph 控件
// ============================================================

FluentGraph::FluentGraph(QWidget* parent)
    : FluentWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

FluentGraph::~FluentGraph() {
    qDeleteAll(m_edges);
    qDeleteAll(m_nodes);
}

// ============================================================
// 节点操作
// ============================================================

FluentGraphNode* FluentGraph::addNode(const QString& id, const QString& title) {
    if (m_nodes.contains(id)) return m_nodes[id];
    auto* node = new FluentGraphNode(id, title);
    m_nodes[id] = node;
    update();
    return node;
}

void FluentGraph::removeNode(const QString& id) {
    // 移除关联的边
    QList<FluentGraphEdge*> toRemove;
    for (auto* edge : m_edges) {
        if (edge->fromNode() == id || edge->toNode() == id) {
            toRemove.append(edge);
        }
    }
    for (auto* e : toRemove) {
        m_edges.removeOne(e);
        delete e;
    }

    auto it = m_nodes.find(id);
    if (it != m_nodes.end()) {
        delete it.value();
        m_nodes.erase(it);
    }
    update();
}

FluentGraphNode* FluentGraph::node(const QString& id) const {
    return m_nodes.value(id, nullptr);
}

QList<FluentGraphNode*> FluentGraph::nodes() const { return m_nodes.values(); }

// ============================================================
// 边操作
// ============================================================

FluentGraphEdge* FluentGraph::addEdge(const QString& fromNode, const QString& fromPort,
                                      const QString& toNode, const QString& toPort) {
    // 检查是否已存在
    for (auto* e : m_edges) {
        if (e->fromNode() == fromNode && e->fromPort() == fromPort &&
            e->toNode() == toNode && e->toPort() == toPort) {
            return e;
        }
    }
    auto* edge = new FluentGraphEdge(fromNode, fromPort, toNode, toPort);
    m_edges.append(edge);
    update();
    return edge;
}

void FluentGraph::removeEdge(const QString& fromNode, const QString& fromPort,
                             const QString& toNode, const QString& toPort) {
    for (auto* e : m_edges) {
        if (e->fromNode() == fromNode && e->fromPort() == fromPort &&
            e->toNode() == toNode && e->toPort() == toPort) {
            m_edges.removeOne(e);
            delete e;
            update();
            return;
        }
    }
}

QList<FluentGraphEdge*> FluentGraph::edges() const { return m_edges; }

// ============================================================
// 选中
// ============================================================

QList<FluentGraphNode*> FluentGraph::selectedNodes() const {
    QList<FluentGraphNode*> result;
    for (auto* n : m_nodes) {
        if (n->isSelected()) result.append(n);
    }
    return result;
}

QList<FluentGraphEdge*> FluentGraph::selectedEdges() const {
    QList<FluentGraphEdge*> result;
    for (auto* e : m_edges) {
        if (e->isSelected()) result.append(e);
    }
    return result;
}

void FluentGraph::clearSelection() {
    for (auto* n : m_nodes) n->setSelected(false);
    for (auto* e : m_edges) e->setSelected(false);
    update();
}

// ============================================================
// 缩放 / 平移
// ============================================================

qreal FluentGraph::zoom() const { return m_zoom; }
void FluentGraph::setZoom(qreal factor) {
    factor = qBound(0.1, factor, 5.0);
    if (qFuzzyCompare(m_zoom, factor)) return;
    m_zoom = factor;
    update();
    emit zoomChanged();
}
void FluentGraph::resetZoom() { setZoom(1.0); }

QPointF FluentGraph::panOffset() const { return m_panOffset; }
void FluentGraph::setPanOffset(const QPointF& offset) { m_panOffset = offset; update(); }
void FluentGraph::resetPan() { m_panOffset = QPointF(); update(); }

bool FluentGraph::editable() const { return m_editable; }
void FluentGraph::setEditable(bool editable) {
    if (m_editable == editable) return;
    m_editable = editable;
    emit editableChanged();
}

FluentGraph::ConnectionStyle FluentGraph::connectionStyle() const { return m_connectionStyle; }
void FluentGraph::setConnectionStyle(ConnectionStyle style) {
    if (m_connectionStyle == style) return;
    m_connectionStyle = style;
    update();
    emit connectionStyleChanged();
}

// ============================================================
// 自动布局
// ============================================================

void FluentGraph::layoutHorizontal(int spacingX, int spacingY) {
    // 简单拓扑排序布局：按入度分层
    QMap<QString, int> inDegree;
    QMap<QString, QList<QString>> adj;  // from -> [to]
    for (auto* n : m_nodes) inDegree[n->id()] = 0;
    for (auto* e : m_edges) {
        inDegree[e->toNode()]++;
        adj[e->fromNode()].append(e->toNode());
    }

    // BFS 分层
    QList<QList<QString>> layers;
    QSet<QString> placed;
    QList<QString> queue;
    for (auto it = inDegree.constBegin(); it != inDegree.constEnd(); ++it) {
        if (it.value() == 0) queue.append(it.key());
    }

    while (!queue.isEmpty()) {
        layers.append(queue);
        QList<QString> next;
        for (const auto& id : queue) {
            placed.insert(id);
            for (const auto& to : adj[id]) {
                inDegree[to]--;
                if (inDegree[to] == 0 && !placed.contains(to)) {
                    next.append(to);
                }
            }
        }
        queue = next;
    }

    // 未被拓扑排序覆盖的节点
    for (auto it = m_nodes.constBegin(); it != m_nodes.constEnd(); ++it) {
        if (!placed.contains(it.key())) {
            layers.append({it.key()});
        }
    }

    // 分配位置
    for (int col = 0; col < layers.size(); ++col) {
        const auto& layer = layers[col];
        int startY = -((layer.size() - 1) * spacingY) / 2;
        for (int row = 0; row < layer.size(); ++row) {
            auto* n = m_nodes.value(layer[row]);
            if (n) {
                n->setPosition(QPointF(col * spacingX, startY + row * spacingY));
            }
        }
    }
    update();
}

void FluentGraph::layoutVertical(int spacingX, int spacingY) {
    // 同 layoutHorizontal，交换 x/y
    QMap<QString, int> inDegree;
    QMap<QString, QList<QString>> adj;
    for (auto* n : m_nodes) inDegree[n->id()] = 0;
    for (auto* e : m_edges) {
        inDegree[e->toNode()]++;
        adj[e->fromNode()].append(e->toNode());
    }

    QList<QList<QString>> layers;
    QSet<QString> placed;
    QList<QString> queue;
    for (auto it = inDegree.constBegin(); it != inDegree.constEnd(); ++it) {
        if (it.value() == 0) queue.append(it.key());
    }

    while (!queue.isEmpty()) {
        layers.append(queue);
        QList<QString> next;
        for (const auto& id : queue) {
            placed.insert(id);
            for (const auto& to : adj[id]) {
                inDegree[to]--;
                if (inDegree[to] == 0 && !placed.contains(to)) {
                    next.append(to);
                }
            }
        }
        queue = next;
    }

    for (auto it = m_nodes.constBegin(); it != m_nodes.constEnd(); ++it) {
        if (!placed.contains(it.key())) {
            layers.append({it.key()});
        }
    }

    for (int row = 0; row < layers.size(); ++row) {
        const auto& layer = layers[row];
        int startX = -((layer.size() - 1) * spacingX) / 2;
        for (int col = 0; col < layer.size(); ++col) {
            auto* n = m_nodes.value(layer[col]);
            if (n) {
                n->setPosition(QPointF(startX + col * spacingX, row * spacingY));
            }
        }
    }
    update();
}

// ============================================================
// 坐标转换
// ============================================================

QPointF FluentGraph::mapFromScene(const QPointF& scenePos) const {
    return scenePos * m_zoom + m_panOffset + QPointF(width() / 2.0, height() / 2.0);
}

QPointF FluentGraph::mapToScene(const QPointF& widgetPos) const {
    return (widgetPos - m_panOffset - QPointF(width() / 2.0, height() / 2.0)) / m_zoom;
}

QSize FluentGraph::sizeHint() const { return QSize(700, 500); }
QSize FluentGraph::minimumSizeHint() const { return QSize(300, 200); }

// ============================================================
// 绘制
// ============================================================

void FluentGraph::paintFluent(QPainter* painter) {
    auto* t = theme();

    // 背景
    painter->fillRect(rect(), t->backgroundColor());

    // 应用变换
    painter->save();
    painter->translate(m_panOffset + QPointF(width() / 2.0, height() / 2.0));
    painter->scale(m_zoom, m_zoom);

    paintGrid(painter);
    paintEdges(painter);
    paintNodes(painter);

    painter->restore();

    // 临时连接线（在 widget 坐标绘制以避免缩放影响线条宽度）
    if (m_interaction == Interaction::ConnectPort) {
        paintTempConnection(painter);
    }
}

void FluentGraph::paintGrid(QPainter* painter) {
    auto* t = theme();

    // 计算可见场景范围
    QPointF topLeft = mapToScene(QPointF(0, 0));
    QPointF bottomRight = mapToScene(QPointF(width(), height()));

    QColor gridColor = t->textColorPrimary();
    gridColor.setAlphaF(0.04);
    QColor gridColorMajor = t->textColorPrimary();
    gridColorMajor.setAlphaF(0.08);

    QPen thinPen(gridColor, 1.0 / m_zoom);
    QPen thickPen(gridColorMajor, 1.0 / m_zoom);

    int gs = GRID_SIZE;
    int startX = int(topLeft.x() / gs) * gs;
    int startY = int(topLeft.y() / gs) * gs;
    int endX = int(bottomRight.x() / gs + 1) * gs;
    int endY = int(bottomRight.y() / gs + 1) * gs;

    for (int x = startX; x <= endX; x += gs) {
        painter->setPen((x % (gs * 5) == 0) ? thickPen : thinPen);
        painter->drawLine(x, startY, x, endY);
    }
    for (int y = startY; y <= endY; y += gs) {
        painter->setPen((y % (gs * 5) == 0) ? thickPen : thinPen);
        painter->drawLine(startX, y, endX, y);
    }
}

void FluentGraph::paintEdges(QPainter* painter) {
    for (auto* edge : m_edges) {
        paintEdge(painter, edge);
    }
}

void FluentGraph::paintEdge(QPainter* painter, FluentGraphEdge* edge) {
    auto* t = theme();
    auto* fromNode = m_nodes.value(edge->fromNode());
    auto* toNode = m_nodes.value(edge->toNode());
    if (!fromNode || !toNode) return;

    QPointF start = fromNode->portScenePos(edge->fromPort());
    QPointF end = toNode->portScenePos(edge->toPort());

    QColor lineColor = edge->isSelected() ? t->primaryColor() : t->textColorSecondary();
    lineColor.setAlphaF(edge->isSelected() ? 0.9 : 0.5);
    qreal lineWidth = (edge->isSelected() ? 2.5 : 1.8) / m_zoom;

    painter->setPen(QPen(lineColor, lineWidth, Qt::SolidLine, Qt::RoundCap));
    painter->setBrush(Qt::NoBrush);

    QPainterPath path;
    if (m_connectionStyle == ConnectionStyle::Bezier) {
        qreal dx = qAbs(end.x() - start.x()) * 0.5;
        QPointF cp1(start.x() + dx, start.y());
        QPointF cp2(end.x() - dx, end.y());
        path.moveTo(start);
        path.cubicTo(cp1, cp2, end);
    } else if (m_connectionStyle == ConnectionStyle::Straight) {
        path.moveTo(start);
        path.lineTo(end);
    } else { // Step
        qreal midX = (start.x() + end.x()) / 2.0;
        path.moveTo(start);
        path.lineTo(midX, start.y());
        path.lineTo(midX, end.y());
        path.lineTo(end);
    }

    painter->drawPath(path);

    // 箭头（在终点处）
    if (m_connectionStyle == ConnectionStyle::Bezier) {
        qreal dx = qAbs(end.x() - start.x()) * 0.5;
        QPointF cp2(end.x() - dx, end.y());
        // 切线方向
        QPointF tangent = end - cp2;
        qreal len = std::sqrt(tangent.x() * tangent.x() + tangent.y() * tangent.y());
        if (len > 0) {
            tangent /= len;
            QPointF normal(-tangent.y(), tangent.x());
            qreal arrowSize = 8.0 / m_zoom;
            QPointF p1 = end - tangent * arrowSize + normal * arrowSize * 0.4;
            QPointF p2 = end - tangent * arrowSize - normal * arrowSize * 0.4;
            painter->setBrush(lineColor);
            painter->setPen(Qt::NoPen);
            QPainterPath arrow;
            arrow.moveTo(end);
            arrow.lineTo(p1);
            arrow.lineTo(p2);
            arrow.closeSubpath();
            painter->drawPath(arrow);
        }
    }
}

void FluentGraph::paintNodes(QPainter* painter) {
    for (auto* node : m_nodes) {
        paintNode(painter, node);
    }
}

void FluentGraph::paintNode(QPainter* painter, FluentGraphNode* node) {
    auto* t = theme();
    QRectF r = node->rect();
    qreal radius = NODE_RADIUS;

    // 阴影
    QColor shadowColor(0, 0, 0, 25);
    painter->setPen(Qt::NoPen);
    painter->setBrush(shadowColor);
    QPainterPath shadowPath;
    shadowPath.addRoundedRect(r.translated(0, 3.0 / m_zoom), radius, radius);
    painter->drawPath(shadowPath);

    // 节点背景
    QPainterPath bgPath;
    bgPath.addRoundedRect(r, radius, radius);

    QColor bgColor = t->cardColor();
    if (node->isSelected()) {
        bgColor = t->cardColor();
    }
    painter->setPen(Qt::NoPen);
    painter->setBrush(bgColor);
    painter->drawPath(bgPath);

    // 顶部颜色标签条
    if (node->colorTag().isValid()) {
        QPainterPath tagPath;
        QRectF tagRect(r.left(), r.top(), r.width(), 4.0 / m_zoom);
        tagPath.addRoundedRect(tagRect, radius, radius);
        // 裁剪底部圆角
        painter->setBrush(node->colorTag());
        painter->drawRect(tagRect);
    }

    // 边框
    QColor borderColor = node->isSelected() ? t->primaryColor() : t->borderColor();
    qreal borderWidth = (node->isSelected() ? 2.0 : 1.0) / m_zoom;
    painter->setPen(QPen(borderColor, borderWidth));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(bgPath);

    // 标题
    painter->setPen(t->textColorPrimary());
    QFont titleFont = t->bodyStrongFont();
    painter->setFont(titleFont);
    QRectF titleRect = r.adjusted(12.0 / m_zoom, 6.0 / m_zoom, -12.0 / m_zoom, -r.height() / 2.0);
    painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignTop, node->title());

    // 端口
    paintPorts(painter, node);
}

void FluentGraph::paintPorts(QPainter* painter, FluentGraphNode* node) {
    auto* t = theme();

    for (const auto& port : node->ports()) {
        QPointF scenePos = node->portScenePos(port.name);
        bool isHovered = (m_hoveredPortNode == node->id() && m_hoveredPortName == port.name);

        qreal r = (isHovered ? PORT_RADIUS + 2 : PORT_RADIUS) * 1.0;

        // 端口外圈
        painter->setPen(QPen(t->textColorSecondary(), 1.5 / m_zoom));
        painter->setBrush(t->cardColor());
        painter->drawEllipse(scenePos, r / m_zoom, r / m_zoom);

        // 端口内圈
        QColor innerColor = (port.direction == FluentGraphPort::Direction::Out) ? t->primaryColor() : t->successColor();
        if (isHovered) innerColor = innerColor.lighter(120);
        painter->setPen(Qt::NoPen);
        painter->setBrush(innerColor);
        painter->drawEllipse(scenePos, r * 0.5 / m_zoom, r * 0.5 / m_zoom);

        // 端口标签
        painter->setPen(t->textColorSecondary());
        painter->setFont(t->captionFont());
        qreal labelOffset = 8.0 / m_zoom;
        if (port.direction == FluentGraphPort::Direction::In) {
            QRectF labelRect(scenePos.x() + labelOffset + PORT_RADIUS / m_zoom, scenePos.y() - 8,
                             80, 16);
            painter->drawText(labelRect, Qt::AlignVCenter | Qt::AlignLeft, port.name);
        } else {
            QFontMetrics fm(t->captionFont());
            int textW = fm.horizontalAdvance(port.name);
            QRectF labelRect(scenePos.x() - labelOffset - PORT_RADIUS / m_zoom - textW, scenePos.y() - 8,
                             textW, 16);
            painter->drawText(labelRect, Qt::AlignVCenter | Qt::AlignRight, port.name);
        }
    }
}

void FluentGraph::paintTempConnection(QPainter* painter) {
    auto* t = theme();
    auto* fromNode = m_nodes.value(m_connectFromNode);
    if (!fromNode) return;

    QPointF startWidget = mapFromScene(fromNode->portScenePos(m_connectFromPort));
    QPointF endWidget = m_connectTempEnd;

    QColor lineColor = t->primaryColor();
    lineColor.setAlphaF(0.7);
    painter->setPen(QPen(lineColor, 2, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);

    QPainterPath path;
    qreal dx = qAbs(endWidget.x() - startWidget.x()) * 0.5;
    QPointF cp1(startWidget.x() + dx, startWidget.y());
    QPointF cp2(endWidget.x() - dx, endWidget.y());
    path.moveTo(startWidget);
    path.cubicTo(cp1, cp2, endWidget);
    painter->drawPath(path);
}

// ============================================================
// 命中测试
// ============================================================

FluentGraphNode* FluentGraph::nodeAt(const QPointF& scenePos) const {
    // 倒序遍历，上层节点优先
    QList<FluentGraphNode*> nodeList = m_nodes.values();
    for (int i = nodeList.size() - 1; i >= 0; --i) {
        if (nodeList[i]->rect().contains(scenePos)) return nodeList[i];
    }
    return nullptr;
}

FluentGraphEdge* FluentGraph::edgeAt(const QPointF& scenePos) const {
    // 检查点到贝塞尔曲线的距离
    for (auto* edge : m_edges) {
        auto* fromNode = m_nodes.value(edge->fromNode());
        auto* toNode = m_nodes.value(edge->toNode());
        if (!fromNode || !toNode) continue;

        QPointF start = fromNode->portScenePos(edge->fromPort());
        QPointF end = toNode->portScenePos(edge->toPort());
        qreal dx = qAbs(end.x() - start.x()) * 0.5;
        QPointF cp1(start.x() + dx, start.y());
        QPointF cp2(end.x() - dx, end.y());

        // 采样检测
        const int samples = 20;
        for (int i = 0; i <= samples; ++i) {
            qreal t = i / qreal(samples);
            qreal t2 = t * t, t3 = t2 * t;
            qreal mt = 1 - t, mt2 = mt * mt, mt3 = mt2 * mt;
            QPointF pt = mt3 * start + 3 * mt2 * t * cp1 + 3 * mt * t2 * cp2 + t3 * end;
            if ((pt - scenePos).manhattanLength() < 10) return edge;
        }
    }
    return nullptr;
}

QString FluentGraph::portAt(FluentGraphNode* node, const QPointF& scenePos) const {
    for (const auto& port : node->ports()) {
        QPointF portPos = node->portScenePos(port.name);
        qreal dist = (scenePos - portPos).manhattanLength();
        if (dist < 12) return port.name;
    }
    return QString();
}

// ============================================================
// 事件处理
// ============================================================

void FluentGraph::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    QPointF scenePos = mapToScene(event->pos());

    // 检查端口
    FluentGraphNode* hitNode = nodeAt(scenePos);
    if (hitNode && m_editable) {
        QString portName = portAt(hitNode, scenePos);
        if (!portName.isEmpty()) {
            // 查找端口方向
            FluentGraphPort::Direction dir = FluentGraphPort::Direction::In;
            for (const auto& p : hitNode->ports()) {
                if (p.name == portName) { dir = p.direction; break; }
            }
            if (dir == FluentGraphPort::Direction::Out) {
                // 开始连接
                m_interaction = Interaction::ConnectPort;
                m_connectFromNode = hitNode->id();
                m_connectFromPort = portName;
                m_connectTempEnd = event->pos();
                return;
            }
        }
    }

    if (hitNode) {
        // 选中节点
        if (!(event->modifiers() & Qt::ControlModifier)) {
            clearSelection();
        }
        hitNode->setSelected(true);
        emit nodeClicked(hitNode->id());

        // 开始拖拽节点
        if (m_editable) {
            m_interaction = Interaction::DragNode;
            m_dragNodeId = hitNode->id();
            m_dragStartScene = scenePos;
            m_dragNodeOrigPos = hitNode->position();
        }
        update();
    } else {
        // 检查边
        FluentGraphEdge* hitEdge = edgeAt(scenePos);
        if (hitEdge) {
            clearSelection();
            hitEdge->setSelected(true);
            emit edgeClicked(hitEdge->fromNode(), hitEdge->fromPort(),
                             hitEdge->toNode(), hitEdge->toPort());
            update();
        } else {
            // 空白区域：开始平移
            clearSelection();
            m_interaction = Interaction::Pan;
            m_panStart = event->pos();
            m_panOrigOffset = m_panOffset;
            update();
        }
    }
}

void FluentGraph::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    if (m_interaction == Interaction::ConnectPort) {
        // 尝试完成连接
        QPointF scenePos = mapToScene(event->pos());
        FluentGraphNode* targetNode = nodeAt(scenePos);
        if (targetNode && targetNode->id() != m_connectFromNode) {
            QString targetPort = portAt(targetNode, scenePos);
            if (!targetPort.isEmpty()) {
                FluentGraphPort::Direction dir = FluentGraphPort::Direction::In;
                for (const auto& p : targetNode->ports()) {
                    if (p.name == targetPort) { dir = p.direction; break; }
                }
                if (dir == FluentGraphPort::Direction::In) {
                    addEdge(m_connectFromNode, m_connectFromPort,
                            targetNode->id(), targetPort);
                    emit edgeCreated(m_connectFromNode, m_connectFromPort,
                                     targetNode->id(), targetPort);
                }
            }
        }
    } else if (m_interaction == Interaction::DragNode) {
        auto* n = m_nodes.value(m_dragNodeId);
        if (n) emit nodeMoved(n->id(), n->position());
    }

    m_interaction = Interaction::None;
    m_dragNodeId.clear();
    m_connectFromNode.clear();
    m_connectFromPort.clear();
    update();
}

void FluentGraph::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    QPointF scenePos = mapToScene(event->pos());
    FluentGraphNode* hitNode = nodeAt(scenePos);
    if (hitNode) {
        emit nodeDoubleClicked(hitNode->id());
    }
}

void FluentGraph::mouseMoveEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->pos());

    if (m_interaction == Interaction::DragNode) {
        auto* n = m_nodes.value(m_dragNodeId);
        if (n) {
            QPointF delta = scenePos - m_dragStartScene;
            n->setPosition(m_dragNodeOrigPos + delta);
            update();
        }
    } else if (m_interaction == Interaction::Pan) {
        QPointF delta = event->pos() - m_panStart;
        m_panOffset = m_panOrigOffset + delta;
        update();
    } else if (m_interaction == Interaction::ConnectPort) {
        m_connectTempEnd = event->pos();
        update();
    } else {
        // 更新悬停状态
        FluentGraphNode* hitNode = nodeAt(scenePos);
        QString oldHovered = m_hoveredNodeId;
        m_hoveredNodeId = hitNode ? hitNode->id() : QString();

        // 检查端口悬停
        QString oldPort = m_hoveredPortName;
        m_hoveredPortName.clear();
        m_hoveredPortNode.clear();
        if (hitNode) {
            QString pName = portAt(hitNode, scenePos);
            if (!pName.isEmpty()) {
                m_hoveredPortName = pName;
                m_hoveredPortNode = hitNode->id();
                setCursor(Qt::CrossCursor);
            } else {
                setCursor(m_editable ? Qt::OpenHandCursor : Qt::ArrowCursor);
            }
        } else {
            setCursor(Qt::ArrowCursor);
        }

        if (oldHovered != m_hoveredNodeId || oldPort != m_hoveredPortName) {
            update();
        }
    }
}

void FluentGraph::wheelEvent(QWheelEvent* event) {
    qreal delta = event->angleDelta().y();
    qreal factor = m_zoom;
    if (delta > 0) {
        factor *= 1.1;
    } else {
        factor /= 1.1;
    }

    // 以鼠标位置为中心缩放
    QPointF mousePos = event->pos();
    QPointF sceneBefore = mapToScene(mousePos);

    setZoom(factor);

    QPointF sceneAfter = mapToScene(mousePos);
    QPointF diff = (sceneAfter - sceneBefore) * m_zoom;
    m_panOffset -= diff;

    event->accept();
}

void FluentGraph::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        // 删除选中节点
        QList<FluentGraphNode*> toRemove = selectedNodes();
        for (auto* n : toRemove) {
            removeNode(n->id());
        }
    } else if (event->key() == Qt::Key_Home) {
        resetZoom();
        resetPan();
    } else {
        FluentWidget::keyPressEvent(event);
    }
}

void FluentGraph::leaveEvent(QEvent* event) {
    m_hoveredNodeId.clear();
    m_hoveredPortName.clear();
    m_hoveredPortNode.clear();
    setCursor(Qt::ArrowCursor);
    update();
    FluentWidget::leaveEvent(event);
}

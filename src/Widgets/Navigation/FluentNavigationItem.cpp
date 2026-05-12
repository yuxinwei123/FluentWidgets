#include "FluentNavigationItem.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QtMath>

FluentNavigationItem::FluentNavigationItem(const QString& text, const QIcon& icon,
                                           const QString& key, QWidget* parent)
    : QWidget(parent)
    , m_text(text)
    , m_icon(icon)
    , m_key(key.isEmpty() ? text : key)
{
    setupUI();
    setMouseTracking(true);
    setAttribute(Qt::WA_StyledBackground, false);
    setAutoFillBackground(false);
    setFixedHeight(contentHeight());

    connect(FluentThemeManager::instance().currentTheme(), &FluentTheme::themeChanged,
            this, [this]() { update(); });
}

QString FluentNavigationItem::text() const { return m_text; }
void FluentNavigationItem::setText(const QString& text) {
    if (m_text != text) { m_text = text; update(); }
}

QIcon FluentNavigationItem::icon() const { return m_icon; }
void FluentNavigationItem::setIcon(const QIcon& icon) {
    m_icon = icon; update();
}

QString FluentNavigationItem::key() const { return m_key; }
void FluentNavigationItem::setKey(const QString& key) { m_key = key; }

bool FluentNavigationItem::expanded() const { return m_expanded; }
void FluentNavigationItem::setExpanded(bool expanded) {
    if (m_expanded == expanded) return;
    m_expanded = expanded;

    // 动画展开/折叠
    auto* anim = new QPropertyAnimation(this, "expandProgress");
    anim->setStartValue(m_expandProgress);
    anim->setEndValue(expanded ? 1.0 : 0.0);
    anim->setDuration(200);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &QAbstractAnimation::finished, anim, &QObject::deleteLater);
    anim->start();

    updateChildVisibility();
    emit expandedChanged(expanded);
}

bool FluentNavigationItem::selected() const { return m_selected; }
void FluentNavigationItem::setSelected(bool selected) {
    if (m_selected != selected) {
        m_selected = selected;
        update();
        emit selectedChanged(selected);
    }
}

bool FluentNavigationItem::hasChildren() const { return !m_children.isEmpty(); }

qreal FluentNavigationItem::expandProgress() const { return m_expandProgress; }
void FluentNavigationItem::setExpandProgress(qreal progress) {
    if (qFuzzyCompare(m_expandProgress, progress)) return;
    m_expandProgress = progress;

    // 动态调整子容器高度
    if (m_childrenContainer && hasChildren()) {
        int fullHeight = 0;
        for (auto* child : m_children) {
            fullHeight += child->sizeHint().height();
        }
        m_childrenContainer->setFixedHeight(static_cast<int>(fullHeight * progress));
    }

    // 更新整个控件的固定高度
    setFixedHeight(contentHeight());
    emit expandProgressChanged(progress);

    // 通知父级导航视图重新布局
    QWidget* p = parentWidget();
    while (p) {
        p->updateGeometry();
        p = p->parentWidget();
    }
}

QList<FluentNavigationItem*> FluentNavigationItem::children() const { return m_children; }

FluentNavigationItem* FluentNavigationItem::parentItem() const { return m_parentItem; }
void FluentNavigationItem::setParentItem(FluentNavigationItem* parent) { m_parentItem = parent; }

int FluentNavigationItem::indentLevel() const {
    int level = 0;
    FluentNavigationItem* p = m_parentItem;
    while (p) {
        level++;
        p = p->parentItem();
    }
    return level;
}

QSize FluentNavigationItem::sizeHint() const {
    int h = ITEM_HEIGHT;
    if (hasChildren() && m_expanded) {
        for (auto* child : m_children) {
            h += static_cast<int>(child->sizeHint().height() * m_expandProgress);
        }
    }
    return QSize(240, h);
}

void FluentNavigationItem::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    auto* theme = FluentThemeManager::instance().currentTheme();
    int level = indentLevel();
    int leftOffset = LEFT_PADDING + level * INDENT_PER_LEVEL;

    // === 绘制自身行 ===
    QRect itemRect(0, 0, width(), ITEM_HEIGHT);

    // 选中指示器（左侧竖条）
    if (m_selected) {
        p.fillRect(QRect(0, 4, 3, ITEM_HEIGHT - 8), theme->primaryColor());
    }

    // 悬停/选中背景
    if (m_selected) {
        QColor selColor = theme->primaryColor();
        selColor.setAlphaF(0.08);
        p.fillRect(itemRect, selColor);
    } else if (m_hovered) {
        QColor hoverColor = theme->textColorPrimary();
        hoverColor.setAlphaF(0.04);
        p.fillRect(itemRect, hoverColor);
    }

    // 图标
    if (!m_icon.isNull()) {
        QRect iconRect(leftOffset, (ITEM_HEIGHT - ICON_SIZE) / 2, ICON_SIZE, ICON_SIZE);
        m_icon.paint(&p, iconRect);
        leftOffset += ICON_SIZE + 8;
    }

    // 文字
    p.setPen(m_selected ? theme->primaryColor() : theme->textColorPrimary());
    p.setFont(theme->bodyFont());
    QRect textRect(leftOffset, 0, width() - leftOffset - (hasChildren() ? 28 : 8), ITEM_HEIGHT);
    p.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, m_text);

    // 展开/折叠箭头 (钝角 V 形，约 120° 张角)
    if (hasChildren()) {
        int arrowX = width() - 20;
        int arrowY = ITEM_HEIGHT / 2;

        p.save();
        p.translate(arrowX, arrowY);
        // 箭头旋转角度：折叠时 0°(朝下)，展开时 -90°(朝上)
        p.rotate(-90.0 * m_expandProgress);

        QPen pen(theme->textColorSecondary(), 1.5);
        pen.setCapStyle(Qt::RoundCap);
        p.setPen(pen);

        // 钝角箭头：两条线各偏离竖直方向 30°，张角 120°
        const qreal len = 5.0;   // 线段长度
        const qreal angle = 30.0; // 偏离竖直方向的角度
        qreal radL = qDegreesToRadians(180.0 + angle);
        qreal radR = qDegreesToRadians(-angle);
        p.drawLine(QPointF(0, 0), QPointF(len * qCos(radL), -len * qSin(radL)));
        p.drawLine(QPointF(0, 0), QPointF(len * qCos(radR), -len * qSin(radR)));

        p.restore();
    }
}

void FluentNavigationItem::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        update();
    }
}

void FluentNavigationItem::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;

        // 判断点击位置：在自身行内
        if (event->pos().y() < ITEM_HEIGHT) {
            if (hasChildren()) {
                toggleExpanded();
            } else {
                emit clicked(m_key);
            }
        }
        update();
    }
}

void FluentNavigationItem::enterEvent(QEvent* /*event*/) {
    m_hovered = true;
    update();
}

void FluentNavigationItem::leaveEvent(QEvent* /*event*/) {
    m_hovered = false;
    update();
}

void FluentNavigationItem::setupUI() {
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    // 顶部占位（自身行由 paintEvent 绘制）
    auto* topSpacer = new QWidget(this);
    topSpacer->setFixedHeight(ITEM_HEIGHT);
    topSpacer->setStyleSheet("background: transparent;");
    m_layout->addWidget(topSpacer);

    // 子项容器
    m_childrenContainer = new QWidget(this);
    m_childrenContainer->setStyleSheet("background: transparent;");
    m_childrenLayout = new QVBoxLayout(m_childrenContainer);
    m_childrenLayout->setContentsMargins(0, 0, 0, 0);
    m_childrenLayout->setSpacing(0);
    m_childrenContainer->hide();

    m_layout->addWidget(m_childrenContainer);
}

FluentNavigationItem* FluentNavigationItem::addChild(const QString& text, const QIcon& icon,
                                                      const QString& key) {
    auto* child = new FluentNavigationItem(text, icon, key, m_childrenContainer);
    child->setParentItem(this);
    m_children.append(child);
    m_childrenLayout->addWidget(child);
    m_childrenContainer->adjustSize();

    // 点击子项时转发信号
    connect(child, &FluentNavigationItem::clicked, this, &FluentNavigationItem::clicked);

    setFixedHeight(contentHeight());
    return child;
}

void FluentNavigationItem::updateChildVisibility() {
    if (hasChildren()) {
        if (m_expanded) {
            m_childrenContainer->show();
            // 子项可见性
            for (auto* child : m_children) {
                child->setVisible(true);
            }
        } else {
            // 折叠时不立即隐藏，等动画结束
            // m_childrenContainer->hide();  // 动画结束后隐藏
        }
    }
}

void FluentNavigationItem::toggleExpanded() {
    if (hasChildren()) {
        setExpanded(!m_expanded);
    }
}

int FluentNavigationItem::contentHeight() const {
    int h = ITEM_HEIGHT;
    if (hasChildren() && m_childrenContainer && m_childrenContainer->isVisible()) {
        h += m_childrenContainer->height();
    }
    return h;
}

#include "FluentPopover.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QLabel>
#include <QVBoxLayout>
#include <QtMath>

// ============================================================
// FluentPopover
// ============================================================

FluentPopover::FluentPopover(QWidget* parent)
    : QWidget(parent, Qt::Window | Qt::FramelessWindowHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_NoMouseReplay);
    setMouseTracking(true);
    installEventFilter(this);

    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [this]() { update(); });
}

FluentPopover::~FluentPopover() {
    if (m_autoCloseTimerId) killTimer(m_autoCloseTimerId);
    if (m_hoverShowTimerId) killTimer(m_hoverShowTimerId);
    if (m_hoverHideTimerId) killTimer(m_hoverHideTimerId);
}

// ============================================================
// 属性
// ============================================================

void FluentPopover::setContent(QWidget* content) {
    if (m_content) m_content->setParent(nullptr);
    m_content = content;
    if (m_content) {
        m_content->setParent(this);
        m_content->show();
    }
    updateContentSize();
    update();
}

QWidget* FluentPopover::content() const { return m_content; }

void FluentPopover::setTarget(QWidget* target) {
    if (m_target) m_target->removeEventFilter(this);
    m_target = target;
    if (m_target) m_target->installEventFilter(this);
}

QWidget* FluentPopover::target() const { return m_target; }

FluentPopover::PopoverPlacement FluentPopover::placement() const { return m_placement; }
void FluentPopover::setPlacement(PopoverPlacement p) { m_placement = p; updatePosition(); emit placementChanged(); }

int FluentPopover::arrowSize() const { return m_arrowSize; }
void FluentPopover::setArrowSize(int size) { m_arrowSize = size; update(); emit arrowSizeChanged(); }

int FluentPopover::margin() const { return m_margin; }
void FluentPopover::setMargin(int m) { m_margin = m; updatePosition(); emit marginChanged(); }

bool FluentPopover::autoClose() const { return m_autoClose; }
void FluentPopover::setAutoClose(bool on) { m_autoClose = on; emit autoCloseChanged(); }

int FluentPopover::autoCloseDelay() const { return m_autoCloseDelay; }
void FluentPopover::setAutoCloseDelay(int ms) { m_autoCloseDelay = ms; emit autoCloseDelayChanged(); }

int FluentPopover::borderRadius() const { return m_borderRadius; }
void FluentPopover::setBorderRadius(int r) { m_borderRadius = r; update(); emit borderRadiusChanged(); }

FluentPopover::TriggerMode FluentPopover::triggerMode() const { return m_triggerMode; }
void FluentPopover::setTriggerMode(TriggerMode mode) { m_triggerMode = mode; emit triggerModeChanged(); }

int FluentPopover::hoverShowDelay() const { return m_hoverShowDelay; }
void FluentPopover::setHoverShowDelay(int ms) { m_hoverShowDelay = ms; }

int FluentPopover::hoverHideDelay() const { return m_hoverHideDelay; }
void FluentPopover::setHoverHideDelay(int ms) { m_hoverHideDelay = ms; }

bool FluentPopover::isPopoverVisible() const { return isVisible(); }

void FluentPopover::setTitleAndContent(const QString& title, const QString& content) {
    m_title = title;
    m_contentText = content;

    // 创建内置内容控件
    auto* container = new QWidget(this);
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(PADDING, PADDING, PADDING, PADDING);
    layout->setSpacing(TITLE_CONTENT_GAP);

    if (!title.isEmpty()) {
        auto* titleLabel = new QLabel(title, container);
        auto* t = FluentThemeManager::instance().currentTheme();
        titleLabel->setFont(t->bodyStrongFont());
        titleLabel->setWordWrap(true);
        layout->addWidget(titleLabel);
    }

    if (!content.isEmpty()) {
        auto* contentLabel = new QLabel(content, container);
        auto* t = FluentThemeManager::instance().currentTheme();
        contentLabel->setFont(t->bodyFont());
        contentLabel->setWordWrap(true);
        contentLabel->setMaximumWidth(MAX_WIDTH - PADDING * 2);
        layout->addWidget(contentLabel);
    }

    setContent(container);
}

QSize FluentPopover::sizeHint() const {
    if (m_content) return m_content->sizeHint() + QSize(m_arrowSize * 2, m_arrowSize * 2);
    return QSize(MIN_WIDTH, 60);
}

QSize FluentPopover::minimumSizeHint() const { return QSize(MIN_WIDTH, 40); }

// ============================================================
// 显示/隐藏
// ============================================================

void FluentPopover::showPopover() {
    if (!m_target) return;

    updateContentSize();
    updatePosition();
    show();
    raise();

    if (m_autoClose && m_autoCloseDelay > 0) {
        if (m_autoCloseTimerId) killTimer(m_autoCloseTimerId);
        m_autoCloseTimerId = startTimer(m_autoCloseDelay);
    }

    emit popoverShown();
}

void FluentPopover::hidePopover() {
    if (m_autoCloseTimerId) { killTimer(m_autoCloseTimerId); m_autoCloseTimerId = 0; }
    hide();
    emit popoverHidden();
}

// ============================================================
// 布局计算
// ============================================================

void FluentPopover::updateContentSize() {
    if (!m_content) return;
    QSize hint = m_content->sizeHint();
    hint = hint.boundedTo(QSize(MAX_WIDTH, 600));
    hint = hint.expandedTo(minimumSizeHint());
    m_content->setFixedSize(hint);
}

void FluentPopover::updatePosition() {
    if (!m_target) return;

    PopoverPlacement p = (m_placement == Auto) ? bestPlacement() : m_placement;

    QRect targetRect = QRect(m_target->mapToGlobal(QPoint(0, 0)), m_target->size());
    QScreen* screen = QApplication::screenAt(targetRect.center());
    if (!screen) screen = QApplication::primaryScreen();
    QRect screenRect = screen->availableGeometry();

    QSize popSize = sizeHint();
    if (m_content) popSize = m_content->size() + QSize(m_arrowSize * 2, m_arrowSize * 2);

    QPoint pos;

    switch (p) {
    case Top:
    case TopLeft:
    case TopRight:
        pos.setY(targetRect.top() - m_margin - popSize.height());
        break;
    case Bottom:
    case BottomLeft:
    case BottomRight:
        pos.setY(targetRect.bottom() + m_margin);
        break;
    case Left:
        pos.setX(targetRect.left() - m_margin - popSize.width());
        break;
    case Right:
        pos.setX(targetRect.right() + m_margin);
        break;
    default:
        break;
    }

    switch (p) {
    case Top:
    case Bottom:
        pos.setX(targetRect.center().x() - popSize.width() / 2);
        break;
    case TopLeft:
    case BottomLeft:
        pos.setX(targetRect.left());
        break;
    case TopRight:
    case BottomRight:
        pos.setX(targetRect.right() - popSize.width());
        break;
    case Left:
    case Right:
        pos.setY(targetRect.center().y() - popSize.height() / 2);
        break;
    default:
        break;
    }

    // 屏幕边界修正
    if (pos.x() < screenRect.left()) pos.setX(screenRect.left());
    if (pos.x() + popSize.width() > screenRect.right()) pos.setX(screenRect.right() - popSize.width());
    if (pos.y() < screenRect.top()) pos.setY(screenRect.top());
    if (pos.y() + popSize.height() > screenRect.bottom()) pos.setY(screenRect.bottom() - popSize.height());

    setGeometry(QRect(pos, popSize));

    // 更新内容位置
    if (m_content) {
        QRect cr = contentRect(p);
        m_content->setGeometry(cr);
    }
}

FluentPopover::PopoverPlacement FluentPopover::bestPlacement() const {
    if (!m_target) return Bottom;

    QRect targetRect = QRect(m_target->mapToGlobal(QPoint(0, 0)), m_target->size());
    QScreen* screen = QApplication::screenAt(targetRect.center());
    if (!screen) screen = QApplication::primaryScreen();
    QRect screenRect = screen->availableGeometry();

    QSize popSize = sizeHint();
    int spaceBelow = screenRect.bottom() - targetRect.bottom();
    int spaceAbove = targetRect.top() - screenRect.top();
    int spaceRight = screenRect.right() - targetRect.right();
    int spaceLeft = targetRect.left() - screenRect.left();

    if (spaceBelow >= popSize.height() + m_margin) return Bottom;
    if (spaceAbove >= popSize.height() + m_margin) return Top;
    if (spaceRight >= popSize.width() + m_margin) return Right;
    if (spaceLeft >= popSize.width() + m_margin) return Left;

    return Bottom;
}

QRect FluentPopover::contentRect(PopoverPlacement p) const {
    int a = m_arrowSize;
    switch (p) {
    case Top:
    case TopLeft:
    case TopRight:
        return QRect(0, 0, width(), height() - a);
    case Bottom:
    case BottomLeft:
    case BottomRight:
        return QRect(0, a, width(), height() - a);
    case Left:
        return QRect(0, 0, width() - a, height());
    case Right:
        return QRect(a, 0, width() - a, height());
    default:
        return rect().adjusted(a, a, -a, -a);
    }
}

QRectF FluentPopover::bubbleRect(PopoverPlacement p) const {
    QRectF cr = contentRect(p);
    int r = m_borderRadius;
    return cr.adjusted(r, r, -r, -r);
}

QPolygon FluentPopover::arrowPolygon(PopoverPlacement p) const {
    if (!m_target) return QPolygon();

    QRect targetRect = QRect(m_target->mapToGlobal(QPoint(0, 0)), m_target->size());
    int a = m_arrowSize;

    // 箭头指向目标的中心
    QPolygon arrow;

    switch (p) {
    case Top:
    case TopLeft:
    case TopRight: {
        // 箭头在底部，指向下方
        int targetCenterX = targetRect.center().x() - x();
        int tipY = height();
        int baseY = height() - a;
        targetCenterX = qBound(a + m_borderRadius, targetCenterX, width() - a - m_borderRadius);
        arrow << QPoint(targetCenterX - a / 2, baseY)
              << QPoint(targetCenterX, tipY)
              << QPoint(targetCenterX + a / 2, baseY);
        break;
    }
    case Bottom:
    case BottomLeft:
    case BottomRight: {
        // 箭头在顶部，指向上方
        int targetCenterX = targetRect.center().x() - x();
        int tipY = 0;
        int baseY = a;
        targetCenterX = qBound(a + m_borderRadius, targetCenterX, width() - a - m_borderRadius);
        arrow << QPoint(targetCenterX - a / 2, baseY)
              << QPoint(targetCenterX, tipY)
              << QPoint(targetCenterX + a / 2, baseY);
        break;
    }
    case Left: {
        // 箭头在右侧，指向右方
        int targetCenterY = targetRect.center().y() - y();
        int tipX = width();
        int baseX = width() - a;
        targetCenterY = qBound(a + m_borderRadius, targetCenterY, height() - a - m_borderRadius);
        arrow << QPoint(baseX, targetCenterY - a / 2)
              << QPoint(tipX, targetCenterY)
              << QPoint(baseX, targetCenterY + a / 2);
        break;
    }
    case Right: {
        // 箭头在左侧，指向左方
        int targetCenterY = targetRect.center().y() - y();
        int tipX = 0;
        int baseX = a;
        targetCenterY = qBound(a + m_borderRadius, targetCenterY, height() - a - m_borderRadius);
        arrow << QPoint(baseX, targetCenterY - a / 2)
              << QPoint(tipX, targetCenterY)
              << QPoint(baseX, targetCenterY + a / 2);
        break;
    }
    default:
        break;
    }

    return arrow;
}

// ============================================================
// 绘制
// ============================================================

void FluentPopover::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    auto* t = FluentThemeManager::instance().currentTheme();

    PopoverPlacement p = (m_placement == Auto) ? bestPlacement() : m_placement;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 阴影
    QRectF shadowRect = rect().adjusted(SHADOW_SIZE, SHADOW_SIZE, SHADOW_SIZE, SHADOW_SIZE);
    QColor shadowColor(0, 0, 0, 30);
    painter.setPen(Qt::NoPen);
    painter.setBrush(shadowColor);
    painter.drawRoundedRect(shadowRect, m_borderRadius + 2, m_borderRadius + 2);

    // 气泡主体 + 箭头
    QRectF bubble = contentRect(p);
    QPolygon arrow = arrowPolygon(p);

    QPainterPath path;
    path.addRoundedRect(bubble, m_borderRadius, m_borderRadius);

    // 添加箭头区域
    if (!arrow.isEmpty()) {
        QPainterPath arrowPath;
        arrowPath.addPolygon(arrow);
        path = path.united(arrowPath);
    }

    // 填充
    painter.setPen(Qt::NoPen);
    painter.setBrush(t->cardColor());
    painter.drawPath(path);

    // 边框
    painter.setPen(QPen(t->dividerColor(), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);
}

// ============================================================
// 事件
// ============================================================

void FluentPopover::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    // 安装全局事件过滤器以检测外部点击
    qApp->installEventFilter(this);
}

void FluentPopover::hideEvent(QHideEvent* event) {
    qApp->removeEventFilter(this);
    QWidget::hideEvent(event);
}

void FluentPopover::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    PopoverPlacement p = (m_placement == Auto) ? bestPlacement() : m_placement;
    if (m_content) {
        m_content->setGeometry(contentRect(p));
    }
}

bool FluentPopover::eventFilter(QObject* obj, QEvent* event) {
    // 目标控件移动/调整大小时更新位置
    if (obj == m_target) {
        if (event->type() == QEvent::Move || event->type() == QEvent::Resize) {
            if (isVisible()) updatePosition();
        }

        // Hover 模式：进入目标显示
        if (m_triggerMode == Hover) {
            if (event->type() == QEvent::Enter) {
                // 取消隐藏计时，启动显示计时
                if (m_hoverHideTimerId) { killTimer(m_hoverHideTimerId); m_hoverHideTimerId = 0; }
                if (!isVisible() && !m_hoverShowTimerId) {
                    m_hoverShowTimerId = startTimer(m_hoverShowDelay);
                }
            } else if (event->type() == QEvent::Leave) {
                // 取消显示计时，启动隐藏计时
                if (m_hoverShowTimerId) { killTimer(m_hoverShowTimerId); m_hoverShowTimerId = 0; }
                if (isVisible() && !m_hoverHideTimerId) {
                    m_hoverHideTimerId = startTimer(m_hoverHideDelay);
                }
            }
        }
    }

    // Hover 模式：Popover 自身 Enter/Leave
    if (obj == this && m_triggerMode == Hover) {
        if (event->type() == QEvent::Enter) {
            // 鼠标进入 Popover，取消隐藏
            if (m_hoverHideTimerId) { killTimer(m_hoverHideTimerId); m_hoverHideTimerId = 0; }
        } else if (event->type() == QEvent::Leave) {
            // 鼠标离开 Popover，启动隐藏
            if (isVisible() && !m_hoverHideTimerId) {
                m_hoverHideTimerId = startTimer(m_hoverHideDelay);
            }
        }
    }

    // Hover 模式：全局鼠标位置检测（弥补顶层窗口 Enter/Leave 可能丢失的问题）
    if (m_triggerMode == Hover && isVisible() && event->type() == QEvent::MouseMove) {
        QPoint globalPos = QCursor::pos();
        bool onTarget = m_target && m_target->rect().contains(m_target->mapFromGlobal(globalPos));
        bool onPopover = rect().contains(mapFromGlobal(globalPos));
        if (onTarget || onPopover) {
            if (m_hoverHideTimerId) { killTimer(m_hoverHideTimerId); m_hoverHideTimerId = 0; }
        } else {
            if (!m_hoverHideTimerId) {
                m_hoverHideTimerId = startTimer(m_hoverHideDelay);
            }
        }
    }

    // 自动关闭: 点击外部（仅 Click 模式）
    if (m_triggerMode == Click && m_autoClose && isVisible() && event->type() == QEvent::MouseButtonPress) {
        auto* me = static_cast<QMouseEvent*>(event);
        QPoint globalPos = me->globalPos();
        bool onTarget = m_target && m_target->rect().contains(m_target->mapFromGlobal(globalPos));
        bool onPopover = rect().contains(mapFromGlobal(globalPos));
        if (!onTarget && !onPopover) {
            hidePopover();
        }
    }

    return QWidget::eventFilter(obj, event);
}

void FluentPopover::timerEvent(QTimerEvent* event) {
    if (event->timerId() == m_autoCloseTimerId) {
        killTimer(m_autoCloseTimerId);
        m_autoCloseTimerId = 0;
        hidePopover();
    } else if (event->timerId() == m_hoverShowTimerId) {
        killTimer(m_hoverShowTimerId);
        m_hoverShowTimerId = 0;
        showPopover();
    } else if (event->timerId() == m_hoverHideTimerId) {
        killTimer(m_hoverHideTimerId);
        m_hoverHideTimerId = 0;
        hidePopover();
    }
}

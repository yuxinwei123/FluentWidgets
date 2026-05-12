#include "FluentWidget.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

FluentWidget::FluentWidget(QWidget* parent)
    : QWidget(parent)
    , m_theme(FluentThemeManager::instance().currentTheme())
{
    setMouseTracking(true);
    connect(m_theme, &FluentTheme::themeChanged, this, [this]() {
        update();
        emit themeChanged();
    });
}

FluentWidget::~FluentWidget() = default;

FluentTheme* FluentWidget::theme() const { return m_theme; }

void FluentWidget::setTheme(FluentTheme* theme) {
    if (m_theme != theme) {
        if (m_theme) {
            disconnect(m_theme, nullptr, this, nullptr);
        }
        m_theme = theme;
        if (m_theme) {
            connect(m_theme, &FluentTheme::themeChanged, this, [this]() {
                update();
                emit themeChanged();
            });
        }
        update();
        emit themeChanged();
    }
}

void FluentWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    qreal progress = m_theme->revealProgress();

    if (progress >= 1.0 || !m_theme->isRevealing()) {
        paintFluent(&p);
        return;
    }

    // === 填充式圆形扩散动画 ===
    // 绘制策略：
    //   第一遍：用旧调色板绘制整个组件
    //   第二遍：设置圆形裁剪，用新调色板绘制圆形内区域
    // 效果：圆内是新主题色，圆外是旧主题色

    QWidget* win = this->window();
    if (!win) {
        paintFluent(&p);
        return;
    }

    // 圆心：窗口左下角，映射到本组件局部坐标
    QPoint centerLocal = mapFrom(win, QPoint(0, win->height()));

    // 最大半径：窗口对角线长度
    qreal maxRadius = qSqrt(qreal(win->width() * win->width()) +
                            qreal(win->height() * win->height()));
    qreal radius = maxRadius * progress;

    // 第一遍：用旧调色板绘制全画面
    m_theme->setPaintOldPalette(true);
    paintFluent(&p);
    m_theme->setPaintOldPalette(false);

    // 第二遍：圆形裁剪内用新调色板绘制
    QPainterPath circlePath;
    circlePath.addEllipse(QPointF(centerLocal), radius, radius);
    p.setClipPath(circlePath);
    paintFluent(&p);
    p.setClipping(false);
}

void FluentWidget::enterEvent(QEvent* /*event*/) {
    m_hovered = true;
    update();
}

void FluentWidget::leaveEvent(QEvent* /*event*/) {
    m_hovered = false;
    update();
}

void FluentWidget::mousePressEvent(QMouseEvent* /*event*/) {
    m_pressed = true;
    update();
}

void FluentWidget::mouseReleaseEvent(QMouseEvent* /*event*/) {
    m_pressed = false;
    update();
}

bool FluentWidget::isHovered() const { return m_hovered; }
bool FluentWidget::isPressed() const { return m_pressed; }

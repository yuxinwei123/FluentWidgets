#include "FluentTitleBar.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>
#include <QMouseEvent>

FluentTitleBar::FluentTitleBar(QWidget* parent)
    : QWidget(parent)
{
    setupUI();

    auto* theme = FluentThemeManager::instance().currentTheme();
    connect(theme, &FluentTheme::themeChanged, this, [this]() { update(); });
}

void FluentTitleBar::setTitle(const QString& title) {
    if (m_titleLabel) {
        m_titleLabel->setText(title);
    }
}

void FluentTitleBar::setIcon(const QIcon& icon) {
    Q_UNUSED(icon)
    // TODO: 设置窗口图标
}

int FluentTitleBar::titleBarHeight() const { return TITLE_BAR_HEIGHT; }

void FluentTitleBar::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    auto* theme = FluentThemeManager::instance().currentTheme();
    qreal progress = theme->revealProgress();

    if (progress >= 1.0 || !theme->isRevealing()) {
        p.fillRect(rect(), theme->backgroundColor());
        return;
    }

    // 填充式圆形扩散动画：与 FluentWindow/FluentWidget 保持一致
    QWidget* win = this->window();
    if (!win) {
        p.fillRect(rect(), theme->backgroundColor());
        return;
    }

    // 第一遍：旧调色板
    theme->setPaintOldPalette(true);
    p.fillRect(rect(), theme->backgroundColor());
    theme->setPaintOldPalette(false);

    // 第二遍：圆形裁剪内新调色板
    QPoint centerLocal = mapFrom(win, QPoint(0, win->height()));
    qreal maxRadius = qSqrt(qreal(win->width() * win->width()) +
                            qreal(win->height() * win->height()));
    qreal radius = maxRadius * progress;

    QPainterPath clipPath;
    clipPath.addEllipse(QPointF(centerLocal), radius, radius);
    p.setClipPath(clipPath);
    p.fillRect(rect(), theme->backgroundColor());
    p.setClipping(false);
}

void FluentTitleBar::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit maximizeClicked();
    }
}

void FluentTitleBar::setupUI() {
    setFixedHeight(TITLE_BAR_HEIGHT);
    setAttribute(Qt::WA_StyledBackground, true);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 0, 0, 0);
    layout->setSpacing(0);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("fluentTitleLabel");

    layout->addWidget(m_titleLabel);
    layout->addStretch();

    // 窗口按钮
    m_minBtn = new QPushButton(u8"─", this);
    m_maxBtn = new QPushButton(u8"□", this);
    m_closeBtn = new QPushButton(u8"✕", this);

    m_minBtn->setFixedSize(46, TITLE_BAR_HEIGHT);
    m_maxBtn->setFixedSize(46, TITLE_BAR_HEIGHT);
    m_closeBtn->setFixedSize(46, TITLE_BAR_HEIGHT);

    m_minBtn->setObjectName("fluentMinBtn");
    m_maxBtn->setObjectName("fluentMaxBtn");
    m_closeBtn->setObjectName("fluentCloseBtn");

    layout->addWidget(m_minBtn);
    layout->addWidget(m_maxBtn);
    layout->addWidget(m_closeBtn);

    connect(m_minBtn, &QPushButton::clicked, this, &FluentTitleBar::minimizeClicked);
    connect(m_maxBtn, &QPushButton::clicked, this, &FluentTitleBar::maximizeClicked);
    connect(m_closeBtn, &QPushButton::clicked, this, &FluentTitleBar::closeClicked);
}

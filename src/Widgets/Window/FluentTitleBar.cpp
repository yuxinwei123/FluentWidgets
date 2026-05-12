#include "FluentTitleBar.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include "Widgets/Input/FluentToggleSwitch.h"
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
    connect(theme, &FluentTheme::themeChanged, this, [this]() {
        update();
        updateThemeToggleStyle();
    });

    // 初始化主题切换状态
    updateThemeToggleStyle();
}

void FluentTitleBar::setTitle(const QString& title) {
    if (m_titleLabel) {
        m_titleLabel->setText(title);
    }
}

void FluentTitleBar::setIcon(const QIcon& icon) {
    if (m_iconLabel) {
        m_iconLabel->setPixmap(icon.pixmap(ICON_SIZE, ICON_SIZE));
        m_iconLabel->show();
    }
}

int FluentTitleBar::titleBarHeight() const { return TITLE_BAR_HEIGHT; }

void FluentTitleBar::setThemeToggleVisible(bool visible) {
    m_themeToggleVisible = visible;
    if (m_themeBtn) m_themeBtn->setVisible(visible);
    if (m_themeToggle) m_themeToggle->setVisible(visible);
}

bool FluentTitleBar::isThemeToggleVisible() const { return m_themeToggleVisible; }

void FluentTitleBar::updateThemeToggleStyle() {
    auto* t = FluentThemeManager::instance().currentTheme();
    bool isDark = t->isDark();

    // 更新主题按钮图标文字
    if (m_themeBtn) {
        m_themeBtn->setText(isDark ? u8"☀" : u8"🌙");
        m_themeBtn->setStyleSheet(QString(
            "QPushButton {"
            "  color: %1;"
            "  background: transparent;"
            "  border: none;"
            "  font-size: 14px;"
            "}"
            "QPushButton:hover {"
            "  background-color: %2;"
            "}"
        ).arg(t->textColorSecondary().name(),
              t->dividerColor().name()));
    }

    // 同步 toggle 状态（不触发信号）
    if (m_themeToggle) {
        m_themeToggle->blockSignals(true);
        m_themeToggle->setChecked(isDark);
        m_themeToggle->blockSignals(false);
    }
}

void FluentTitleBar::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    auto* theme = FluentThemeManager::instance().currentTheme();
    qreal progress = theme->revealProgress();

    if (progress >= 1.0 || !theme->isRevealing()) {
        p.fillRect(rect(), theme->backgroundColor());
        return;
    }

    QWidget* win = this->window();
    if (!win) {
        p.fillRect(rect(), theme->backgroundColor());
        return;
    }

    theme->setPaintOldPalette(true);
    p.fillRect(rect(), theme->backgroundColor());
    theme->setPaintOldPalette(false);

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

    m_iconLabel = new QLabel(this);
    m_iconLabel->setObjectName("fluentIconLabel");
    m_iconLabel->setFixedSize(ICON_SIZE, ICON_SIZE);
    m_iconLabel->hide();

    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("fluentTitleLabel");

    layout->addWidget(m_iconLabel);
    layout->addSpacing(6);
    layout->addWidget(m_titleLabel);
    layout->addStretch();

    // === 主题切换区域 ===
    // 主题图标按钮
    m_themeBtn = new QPushButton(u8"🌙", this);
    m_themeBtn->setFixedSize(32, TITLE_BAR_HEIGHT);
    m_themeBtn->setObjectName("fluentThemeBtn");
    m_themeBtn->setCursor(Qt::PointingHandCursor);
    connect(m_themeBtn, &QPushButton::clicked, this, [this]() {
        bool isDark = FluentThemeManager::instance().currentTheme()->isDark();
        emit themeToggleRequested(!isDark);
    });

    // 主题切换开关
    m_themeToggle = new FluentToggleSwitch(QString(), this);
    m_themeToggle->setFixedSize(40, 20);
    m_themeToggle->setCursor(Qt::PointingHandCursor);
    connect(m_themeToggle, &FluentToggleSwitch::checkedChanged, this, [this](bool checked) {
        emit themeToggleRequested(checked);
    });

    layout->addWidget(m_themeBtn);
    layout->addSpacing(2);
    layout->addWidget(m_themeToggle);
    layout->addSpacing(8);

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

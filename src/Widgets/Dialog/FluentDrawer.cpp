#include "FluentDrawer.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include "Widgets/Buttons/FluentButton.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QKeyEvent>
#include <QApplication>

// ============================================================
// FluentDrawer
// ============================================================

FluentDrawer::FluentDrawer(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
    applyTheme();

    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged,
            this, [this]() { applyTheme(); update(); });
}

FluentDrawer::~FluentDrawer() {
    if (m_slideAnim) {
        m_slideAnim->stop();
        delete m_slideAnim;
    }
}

// ============================================================
// 属性
// ============================================================

FluentDrawer::DrawerEdge FluentDrawer::edge() const { return m_edge; }

void FluentDrawer::setEdge(DrawerEdge edge) {
    if (m_edge == edge) return;
    m_edge = edge;
    updatePanelPosition();
    update();
    emit edgeChanged();
}

int FluentDrawer::drawerWidth() const { return m_drawerWidth; }

void FluentDrawer::setDrawerWidth(int width) {
    if (m_drawerWidth == width) return;
    m_drawerWidth = qMax(100, width);
    if (m_isOpen) updatePanelGeometry();
    update();
    emit drawerWidthChanged();
}

int FluentDrawer::drawerHeight() const { return m_drawerHeight; }

void FluentDrawer::setDrawerHeight(int height) {
    if (m_drawerHeight == height) return;
    m_drawerHeight = qMax(100, height);
    if (m_isOpen) updatePanelGeometry();
    update();
    emit drawerHeightChanged();
}

bool FluentDrawer::overlay() const { return m_overlay; }

void FluentDrawer::setOverlay(bool on) {
    if (m_overlay == on) return;
    m_overlay = on;
    if (m_maskWidget) m_maskWidget->setVisible(on);
    update();
    emit overlayChanged();
}

bool FluentDrawer::closable() const { return m_closable; }

void FluentDrawer::setClosable(bool on) {
    if (m_closable == on) return;
    m_closable = on;
    emit closableChanged();
}

QString FluentDrawer::title() const { return m_title; }

void FluentDrawer::setTitle(const QString& text) {
    if (m_title == text) return;
    m_title = text;
    if (m_titleLabel) m_titleLabel->setText(text);
    emit titleChanged();
}

void FluentDrawer::setContent(QWidget* content) {
    if (m_contentWidget) {
        m_panelLayout->removeWidget(m_contentWidget);
        m_contentWidget->setParent(nullptr);
    }
    m_contentWidget = content;
    if (m_contentWidget && m_panelLayout) {
        m_panelLayout->addWidget(m_contentWidget, 1);
    }
}

QWidget* FluentDrawer::content() const { return m_contentWidget; }

int FluentDrawer::animationDuration() const { return m_animDuration; }

void FluentDrawer::setAnimationDuration(int ms) {
    m_animDuration = qMax(0, ms);
}

qreal FluentDrawer::slideProgress() const { return m_slideProgress; }

void FluentDrawer::setSlideProgress(qreal progress) {
    progress = qBound(0.0, progress, 1.0);
    if (qFuzzyCompare(m_slideProgress, progress)) return;
    m_slideProgress = progress;
    updatePanelPosition();
    // 更新遮罩透明度
    if (m_maskWidget) {
        qreal maskAlpha = m_overlay ? progress * 0.5 : 0.0;
        m_maskWidget->setStyleSheet(
            QString("background-color: rgba(0,0,0,%1);").arg(maskAlpha, 0, 'f', 2));
    }
    emit slideProgressChanged();
}

bool FluentDrawer::isOpen() const { return m_isOpen; }

QSize FluentDrawer::sizeHint() const { return QSize(400, 400); }

// ============================================================
// 显示 / 隐藏
// ============================================================

void FluentDrawer::open() {
    if (m_isOpen) return;
    m_isOpen = true;

    // 先定位到起始位置
    m_slideProgress = 0.0;
    m_drawerPanel->setGeometry(panelStartRect());
    m_drawerPanel->show();
    if (m_maskWidget) m_maskWidget->show();
    show();
    raise();

    // 动画滑入
    if (m_slideAnim) { m_slideAnim->stop(); delete m_slideAnim; }
    m_slideAnim = new QPropertyAnimation(this, "slideProgress");
    m_slideAnim->setStartValue(0.0);
    m_slideAnim->setEndValue(1.0);
    m_slideAnim->setDuration(m_animDuration);
    m_slideAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_slideAnim, &QAbstractAnimation::finished, this, [this]() {
        emit opened();
    });
    m_slideAnim->start(QAbstractAnimation::DeleteWhenStopped);
    m_slideAnim = nullptr; // 会被自动删除
}

void FluentDrawer::close() {
    if (!m_isOpen) return;

    if (m_slideAnim) { m_slideAnim->stop(); delete m_slideAnim; }
    m_slideAnim = new QPropertyAnimation(this, "slideProgress");
    m_slideAnim->setStartValue(m_slideProgress);
    m_slideAnim->setEndValue(0.0);
    m_slideAnim->setDuration(m_animDuration);
    m_slideAnim->setEasingCurve(QEasingCurve::InCubic);
    connect(m_slideAnim, &QAbstractAnimation::finished, this, [this]() {
        m_isOpen = false;
        m_drawerPanel->hide();
        if (m_maskWidget) m_maskWidget->hide();
        hide();
        emit closed();
    });
    m_slideAnim->start(QAbstractAnimation::DeleteWhenStopped);
    m_slideAnim = nullptr;
}

// ============================================================
// 绘制
// ============================================================

void FluentDrawer::paintEvent(QPaintEvent* /*event*/) {
    // 主窗口透明，实际内容由 m_maskWidget 和 m_drawerPanel 承载
}

void FluentDrawer::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateMaskGeometry();
    if (m_isOpen) {
        updatePanelGeometry();
    }
}

bool FluentDrawer::eventFilter(QObject* obj, QEvent* event) {
    if (obj == m_maskWidget && event->type() == QEvent::MouseButtonPress) {
        if (m_closable) {
            close();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void FluentDrawer::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape && m_closable) {
        close();
        return;
    }
    QWidget::keyPressEvent(event);
}

// ============================================================
// 布局计算
// ============================================================

QRect FluentDrawer::panelTargetRect() const {
    int w = width();
    int h = height();

    switch (m_edge) {
    case Left:
        return QRect(0, 0, m_drawerWidth, h);
    case Right:
        return QRect(w - m_drawerWidth, 0, m_drawerWidth, h);
    case Top:
        return QRect(0, 0, w, m_drawerHeight);
    case Bottom:
        return QRect(0, h - m_drawerHeight, w, m_drawerHeight);
    }
    return QRect(0, 0, m_drawerWidth, h);
}

QRect FluentDrawer::panelStartRect() const {
    int w = width();
    int h = height();

    switch (m_edge) {
    case Left:
        return QRect(-m_drawerWidth, 0, m_drawerWidth, h);
    case Right:
        return QRect(w, 0, m_drawerWidth, h);
    case Top:
        return QRect(0, -m_drawerHeight, w, m_drawerHeight);
    case Bottom:
        return QRect(0, h, w, m_drawerHeight);
    }
    return QRect(-m_drawerWidth, 0, m_drawerWidth, h);
}

void FluentDrawer::updateMaskGeometry() {
    if (m_maskWidget) {
        m_maskWidget->setGeometry(0, 0, width(), height());
    }
}

void FluentDrawer::updatePanelGeometry() {
    if (m_drawerPanel) {
        m_drawerPanel->setGeometry(panelTargetRect());
    }
}

void FluentDrawer::updatePanelPosition() {
    if (!m_drawerPanel) return;

    QRect start = panelStartRect();
    QRect target = panelTargetRect();

    // 根据 slideProgress 插值
    qreal p = m_slideProgress;
    int x = qRound(start.x() + (target.x() - start.x()) * p);
    int y = qRound(start.y() + (target.y() - start.y()) * p);
    int w = target.width();
    int h = target.height();

    m_drawerPanel->setGeometry(x, y, w, h);
}

// ============================================================
// 内部初始化
// ============================================================

void FluentDrawer::setupUI() {
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowModal, false); // 非模态，可自行控制
    hide();

    // 遮罩层
    m_maskWidget = new QWidget(this);
    m_maskWidget->setStyleSheet("background-color: rgba(0,0,0,0.50);");
    m_maskWidget->installEventFilter(this);
    m_maskWidget->setVisible(m_overlay);

    // 抽屉面板
    m_drawerPanel = new QWidget(this);
    m_drawerPanel->setObjectName("fluentDrawerPanel");

    m_panelLayout = new QVBoxLayout(m_drawerPanel);
    m_panelLayout->setContentsMargins(PADDING, PADDING, PADDING, PADDING);
    m_panelLayout->setSpacing(0);

    // 标题栏
    auto* headerWidget = new QWidget(m_drawerPanel);
    headerWidget->setObjectName("fluentDrawerHeader");
    headerWidget->setFixedHeight(HEADER_HEIGHT);

    auto* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 0, 12, 0);
    headerLayout->setSpacing(8);

    m_titleLabel = new QLabel(m_title, headerWidget);
    m_titleLabel->setObjectName("fluentDrawerTitle");
    headerLayout->addWidget(m_titleLabel, 1);

    auto* closeBtn = new FluentButton(u8"✕", FluentButtonVariant::Subtle, headerWidget);
    closeBtn->setFixedSize(32, 32);
    connect(closeBtn, &FluentButton::clicked, this, [this]() { close(); });
    headerLayout->addWidget(closeBtn);

    m_panelLayout->addWidget(headerWidget);

    // 分割线
    auto* separator = new QFrame(m_drawerPanel);
    separator->setObjectName("fluentDrawerSeparator");
    separator->setFixedHeight(1);
    m_panelLayout->addWidget(separator);

    m_drawerPanel->hide();
}

void FluentDrawer::applyTheme() {
    auto* theme = FluentThemeManager::instance().currentTheme();

    // 根据方向设置圆角
    QString radiusStr;
    switch (m_edge) {
    case Left:  radiusStr = "0px 8px 8px 0px"; break;
    case Right: radiusStr = "8px 0px 0px 8px"; break;
    case Top:   radiusStr = "0px 0px 8px 8px"; break;
    case Bottom: radiusStr = "8px 8px 0px 0px"; break;
    }

    m_drawerPanel->setStyleSheet(
        QString("#fluentDrawerPanel {"
                "  background-color: %1;"
                "  border: 1px solid %2;"
                "  border-radius: %3;"
                "}"
                "#fluentDrawerHeader {"
                "  background: transparent;"
                "}"
                "#fluentDrawerTitle {"
                "  color: %4;"
                "  font-size: 16px;"
                "  font-weight: bold;"
                "}"
                "#fluentDrawerSeparator {"
                "  background-color: %5;"
                "  border: none;"
                "}")
        .arg(theme->cardColor().name())
        .arg(theme->borderColor().name())
        .arg(radiusStr)
        .arg(theme->textColorPrimary().name())
        .arg(theme->dividerColor().name())
    );
}

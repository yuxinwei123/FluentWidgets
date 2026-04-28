#include "FluentWindow.h"
#include "FluentTitleBar.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

#ifdef Q_OS_WIN
#include <windowsx.h>
#include <Windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif

FluentWindow::FluentWindow(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

FluentTitleBar* FluentWindow::titleBar() const { return m_titleBar; }

bool FluentWindow::micaEnabled() const { return m_micaEnabled; }
bool FluentWindow::acrylicEnabled() const { return m_acrylicEnabled; }

void FluentWindow::setCentralWidget(QWidget* widget) {
    if (m_centralWidget) {
        m_centralWidget->deleteLater();
    }
    m_centralWidget = widget;
    if (widget) {
        layout()->addWidget(widget);
    }
}

void FluentWindow::setMicaEnabled(bool enabled) {
    if (m_micaEnabled != enabled) {
        m_micaEnabled = enabled;
        if (enabled) applyMicaEffect();
        else removeMicaEffect();
        emit micaChanged();
    }
}

void FluentWindow::setAcrylicEnabled(bool enabled) {
    if (m_acrylicEnabled != enabled) {
        m_acrylicEnabled = enabled;
        emit acrylicChanged();
    }
}

void FluentWindow::setWindowTitle(const QString& title) {
    QWidget::setWindowTitle(title);
    if (m_titleBar) {
        m_titleBar->setTitle(title);
    }
}

void FluentWindow::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    FluentTheme* theme = FluentThemeManager::instance().currentTheme();
    qreal progress = theme->revealProgress();

    if (progress >= 1.0 || !theme->isRevealing()) {
        p.fillRect(rect(), theme->backgroundColor());
        return;
    }

    // === 填充式圆形扩散动画 ===

    // 1) 先绘制旧背景色覆盖整个窗口
    theme->setPaintOldPalette(true);
    p.fillRect(rect(), theme->backgroundColor());
    theme->setPaintOldPalette(false);

    // 2) 在圆形区域内绘制新背景色
    QPointF center(0, height());
    qreal maxRadius = qSqrt(qreal(width() * width()) + qreal(height() * height()));
    qreal radius = maxRadius * progress;

    QPainterPath clipPath;
    clipPath.addEllipse(center, radius, radius);
    p.setClipPath(clipPath);
    p.fillRect(rect(), theme->backgroundColor());
    p.setClipping(false);
}

void FluentWindow::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
}

bool FluentWindow::nativeEvent(const QByteArray& eventType, void* message, long* result) {
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG") {
        return hitTestWindows(reinterpret_cast<MSG*>(message), result);
    }
#else
    Q_UNUSED(eventType); Q_UNUSED(message); Q_UNUSED(result);
#endif
    return QWidget::nativeEvent(eventType, message, result);
}

#ifdef Q_OS_WIN
bool FluentWindow::hitTestWindows(MSG* msg, long* result) {
    if (msg->message == WM_NCHITTEST) {
        QPoint pos = mapFromGlobal(QPoint(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)));

        if (m_titleBar && m_titleBar->rect().contains(pos)) {
            QPoint titleBarPos = m_titleBar->mapFromParent(pos);
            QWidget* child = m_titleBar->childAt(titleBarPos);
            if (child) {
                *result = HTCLIENT;
                return true;
            }
            *result = HTCAPTION;
            return true;
        }
        const int border = 6;
        QRect r = rect();
        if (pos.x() < border) {
            if (pos.y() < border) { *result = HTTOPLEFT; return true; }
            if (pos.y() > r.height() - border) { *result = HTBOTTOMLEFT; return true; }
            *result = HTLEFT; return true;
        }
        if (pos.x() > r.width() - border) {
            if (pos.y() < border) { *result = HTTOPRIGHT; return true; }
            if (pos.y() > r.height() - border) { *result = HTBOTTOMRIGHT; return true; }
            *result = HTRIGHT; return true;
        }
        if (pos.y() < border) { *result = HTTOP; return true; }
        if (pos.y() > r.height() - border) { *result = HTBOTTOM; return true; }
    }
    return false;
}
#endif

void FluentWindow::setupUI() {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_titleBar = new FluentTitleBar(this);
    mainLayout->addWidget(m_titleBar);

    // 连接标题栏信号到窗口操作
    connect(m_titleBar, &FluentTitleBar::minimizeClicked, this, &FluentWindow::onMinimize);
    connect(m_titleBar, &FluentTitleBar::maximizeClicked, this, &FluentWindow::onMaximizeRestore);
    connect(m_titleBar, &FluentTitleBar::closeClicked, this, &FluentWindow::onClose);

    connect(FluentThemeManager::instance().currentTheme(), &FluentTheme::themeChanged,
            this, [this]() { update(); });
}

void FluentWindow::applyMicaEffect() {
#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(winId());
    int value = 2;
    DwmSetWindowAttribute(hwnd, 38, &value, sizeof(value));
#endif
}

void FluentWindow::removeMicaEffect() {
#ifdef Q_OS_WIN
    HWND hwnd = reinterpret_cast<HWND>(winId());
    int value = 0;
    DwmSetWindowAttribute(hwnd, 38, &value, sizeof(value));
#endif
}

void FluentWindow::onMinimize() {
    showMinimized();
}

void FluentWindow::onMaximizeRestore() {
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

void FluentWindow::onClose() {
    close();
}

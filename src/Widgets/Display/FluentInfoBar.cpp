#include "FluentInfoBar.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QTimer>
#include <QMouseEvent>
#include <QApplication>

// === 静态工厂方法 ===

void FluentInfoBar::info(QWidget* parent, const QString& title, const QString& content,
                          int duration, Position pos) {
    auto* bar = new FluentInfoBar(parent);
    bar->setBarType(BarType::Info);
    bar->setTitle(title);
    bar->setContent(content);
    bar->setDuration(duration);
    bar->setPosition(pos);
    bar->showBar();
}

void FluentInfoBar::success(QWidget* parent, const QString& title, const QString& content,
                             int duration, Position pos) {
    auto* bar = new FluentInfoBar(parent);
    bar->setBarType(BarType::Success);
    bar->setTitle(title);
    bar->setContent(content);
    bar->setDuration(duration);
    bar->setPosition(pos);
    bar->showBar();
}

void FluentInfoBar::warning(QWidget* parent, const QString& title, const QString& content,
                             int duration, Position pos) {
    auto* bar = new FluentInfoBar(parent);
    bar->setBarType(BarType::Warning);
    bar->setTitle(title);
    bar->setContent(content);
    bar->setDuration(duration);
    bar->setPosition(pos);
    bar->showBar();
}

void FluentInfoBar::error(QWidget* parent, const QString& title, const QString& content,
                           int duration, Position pos) {
    auto* bar = new FluentInfoBar(parent);
    bar->setBarType(BarType::Error);
    bar->setTitle(title);
    bar->setContent(content);
    bar->setDuration(duration);
    bar->setPosition(pos);
    bar->showBar();
}

// === 构造 ===

FluentInfoBar::FluentInfoBar(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
    setMouseTracking(true);
    setAttribute(Qt::WA_StyledBackground, false);
    setAutoFillBackground(false);

    connect(FluentThemeManager::instance().currentTheme(), &FluentTheme::themeChanged,
            this, [this]() { update(); });
}

FluentInfoBar::BarType FluentInfoBar::barType() const { return m_type; }
void FluentInfoBar::setBarType(BarType type) { m_type = type; update(); }

QString FluentInfoBar::title() const { return m_title; }
void FluentInfoBar::setTitle(const QString& title) { m_title = title; update(); }

QString FluentInfoBar::content() const { return m_content; }
void FluentInfoBar::setContent(const QString& content) { m_content = content; update(); }

FluentInfoBar::Position FluentInfoBar::position() const { return m_position; }
void FluentInfoBar::setPosition(Position pos) { m_position = pos; }

int FluentInfoBar::duration() const { return m_duration; }
void FluentInfoBar::setDuration(int ms) { m_duration = ms; }

bool FluentInfoBar::closeButtonVisible() const { return m_closeBtnVisible; }
void FluentInfoBar::setCloseButtonVisible(bool visible) { m_closeBtnVisible = visible; update(); }

qreal FluentInfoBar::slideProgress() const { return m_slideProgress; }
void FluentInfoBar::setSlideProgress(qreal progress) {
    if (qFuzzyCompare(m_slideProgress, progress)) return;
    m_slideProgress = progress;

    // 根据滑动进度定位
    if (parentWidget()) {
        int barW = qMin(parentWidget()->width() - 32, 500);
        int barH = BAR_HEIGHT;
        int x = (parentWidget()->width() - barW) / 2;

        int y;
        if (m_position == Position::Top) {
            int targetY = 8;
            int startY = -barH;
            y = static_cast<int>(startY + (targetY - startY) * progress);
        } else {
            int targetY = parentWidget()->height() - barH - 8;
            int startY = parentWidget()->height();
            y = static_cast<int>(startY + (targetY - startY) * progress);
        }

        setGeometry(x, y, barW, barH);
    }
    emit slideProgressChanged();
}

QSize FluentInfoBar::sizeHint() const {
    return QSize(qMin(parentWidget() ? parentWidget()->width() - 32 : 400, 500), BAR_HEIGHT);
}

void FluentInfoBar::showBar() {
    if (!parentWidget()) return;

    // 初始位置在屏幕外
    int barW = qMin(parentWidget()->width() - 32, 500);
    int barH = BAR_HEIGHT;
    int x = (parentWidget()->width() - barW) / 2;

    if (m_position == Position::Top) {
        setGeometry(x, -barH, barW, barH);
    } else {
        setGeometry(x, parentWidget()->height(), barW, barH);
    }

    show();
    raise();

    // 滑入动画
    m_slideProgress = 0.0;
    auto* anim = new QPropertyAnimation(this, "slideProgress");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(250);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &QAbstractAnimation::finished, anim, &QObject::deleteLater);
    anim->start();

    // 自动关闭定时器
    if (m_duration > 0) {
        m_autoCloseTimer = new QTimer(this);
        m_autoCloseTimer->setSingleShot(true);
        connect(m_autoCloseTimer, &QTimer::timeout, this, &FluentInfoBar::dismissBar);
        m_autoCloseTimer->start(m_duration);
    }
}

void FluentInfoBar::dismissBar() {
    if (m_autoCloseTimer) {
        m_autoCloseTimer->stop();
        m_autoCloseTimer->deleteLater();
        m_autoCloseTimer = nullptr;
    }

    // 滑出动画
    auto* anim = new QPropertyAnimation(this, "slideProgress");
    anim->setStartValue(m_slideProgress);
    anim->setEndValue(0.0);
    anim->setDuration(200);
    anim->setEasingCurve(QEasingCurve::InCubic);
    connect(anim, &QAbstractAnimation::finished, this, [this]() {
        hide();
        emit dismissed();
        deleteLater();
    });
    connect(anim, &QAbstractAnimation::finished, anim, &QObject::deleteLater);
    anim->start();
}

QColor FluentInfoBar::accentColor() const {
    auto* theme = FluentThemeManager::instance().currentTheme();
    switch (m_type) {
    case BarType::Info:    return theme->primaryColor();
    case BarType::Success: return theme->successColor();
    case BarType::Warning: return theme->warningColor();
    case BarType::Error:   return theme->errorColor();
    }
    return theme->primaryColor();
}

QString FluentInfoBar::iconChar() const {
    switch (m_type) {
    case BarType::Info:    return u8"ℹ";
    case BarType::Success: return u8"✓";
    case BarType::Warning: return u8"⚠";
    case BarType::Error:   return u8"✕";
    }
    return u8"ℹ";
}

QRect FluentInfoBar::closeBtnRect() const {
    return QRect(width() - RIGHT_PADDING - CLOSE_BTN_SIZE,
                 (BAR_HEIGHT - CLOSE_BTN_SIZE) / 2,
                 CLOSE_BTN_SIZE, CLOSE_BTN_SIZE);
}

bool FluentInfoBar::isOverCloseButton(const QPoint& pos) const {
    return m_closeBtnVisible && closeBtnRect().contains(pos);
}

void FluentInfoBar::setupUI() {
    setFixedHeight(BAR_HEIGHT);
    setMinimumWidth(200);
}

void FluentInfoBar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && isOverCloseButton(event->pos())) {
        dismissBar();
    }
}

void FluentInfoBar::mouseMoveEvent(QMouseEvent* event) {
    bool overClose = isOverCloseButton(event->pos());
    setCursor(overClose ? Qt::ArrowCursor : Qt::PointingHandCursor);
    if (overClose != m_hovered) {
        m_hovered = overClose;
        update();
    }
}

void FluentInfoBar::enterEvent(QEvent* /*event*/) {
    // 悬停时暂停自动关闭
    if (m_autoCloseTimer && m_autoCloseTimer->isActive()) {
        m_autoCloseTimer->stop();
    }
}

void FluentInfoBar::leaveEvent(QEvent* /*event*/) {
    m_hovered = false;
    update();
    // 离开时恢复自动关闭
    if (m_autoCloseTimer && m_duration > 0 && !m_autoCloseTimer->isActive()) {
        m_autoCloseTimer->start(2000); // 离开后 2 秒关闭
    }
}

void FluentInfoBar::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    auto* theme = FluentThemeManager::instance().currentTheme();
    int radius = theme->cornerRadius();

    // 1. 背景
    QPainterPath bgPath;
    bgPath.addRoundedRect(rect(), radius, radius);
    p.setPen(Qt::NoPen);
    p.setBrush(theme->cardColor());
    p.drawPath(bgPath);

    // 2. 左侧彩色边条
    QPainterPath accentPath;
    accentPath.addRoundedRect(QRect(0, 0, 4, height()), 2, 2);
    p.setBrush(accentColor());
    p.drawPath(accentPath);

    // 3. 图标
    int x = LEFT_PADDING;
    p.setPen(accentColor());
    p.setFont(QFont("Segoe UI", 13));
    QRect iconRect(x, 0, ICON_SIZE + 4, height());
    p.drawText(iconRect, Qt::AlignVCenter | Qt::AlignCenter, iconChar());
    x += ICON_SIZE + SPACING;

    // 4. 标题
    if (!m_title.isEmpty()) {
        p.setPen(theme->textColorPrimary());
        QFont titleFont = theme->bodyStrongFont();
        p.setFont(titleFont);
        QFontMetrics fmTitle(titleFont);

        int maxTitleW = width() - x - (m_closeBtnVisible ? CLOSE_BTN_SIZE + RIGHT_PADDING + SPACING : RIGHT_PADDING);
        if (!m_content.isEmpty()) {
            maxTitleW = qMin(maxTitleW, fmTitle.horizontalAdvance(m_title) + 8);
        }

        QRect titleRect(x, 0, maxTitleW, height());
        p.drawText(titleRect, Qt::AlignVCenter | Qt::AlignLeft, m_title);
        x += fmTitle.horizontalAdvance(m_title) + SPACING;
    }

    // 5. 内容
    if (!m_content.isEmpty()) {
        p.setPen(theme->textColorSecondary());
        p.setFont(theme->bodyFont());
        int maxContentW = width() - x - (m_closeBtnVisible ? CLOSE_BTN_SIZE + RIGHT_PADDING + SPACING : RIGHT_PADDING);
        QRect contentRect(x, 0, maxContentW, height());
        p.drawText(contentRect, Qt::AlignVCenter | Qt::AlignLeft, m_content);
    }

    // 6. 关闭按钮
    if (m_closeBtnVisible) {
        QRect closeRect = closeBtnRect();

        if (m_hovered && isOverCloseButton(mapFromGlobal(QCursor::pos()))) {
            QColor hoverBg = theme->textColorPrimary();
            hoverBg.setAlphaF(0.06);
            p.setPen(Qt::NoPen);
            p.setBrush(hoverBg);
            p.drawEllipse(closeRect);
        }

        p.setPen(theme->textColorSecondary());
        p.setFont(QFont("Segoe UI", 9));
        p.drawText(closeRect, Qt::AlignCenter, u8"✕");
    }

    // 7. 边框
    p.setPen(QPen(theme->borderColor(), theme->borderWidth()));
    p.setBrush(Qt::NoBrush);
    p.drawPath(bgPath);
}

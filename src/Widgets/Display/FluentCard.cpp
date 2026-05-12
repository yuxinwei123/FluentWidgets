#include "FluentCard.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

// ============================================================
// FluentCard
// ============================================================

FluentCard::FluentCard(QWidget* parent)
    : FluentWidget(parent)
{
    setupUI();
    applyTheme();

    connect(theme(), &FluentTheme::themeChanged, this, [this]() { applyTheme(); update(); });
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [this]() { applyTheme(); update(); });
}

FluentCard::~FluentCard() = default;

// ============================================================
// 属性
// ============================================================

QString FluentCard::title() const { return m_title; }

void FluentCard::setTitle(const QString& text) {
    if (m_title == text) return;
    m_title = text;
    if (m_titleLabel) {
        m_titleLabel->setText(text);
        m_titleLabel->setVisible(!text.isEmpty());
    }
    emit titleChanged();
}

QString FluentCard::subtitle() const { return m_subtitle; }

void FluentCard::setSubtitle(const QString& text) {
    if (m_subtitle == text) return;
    m_subtitle = text;
    if (m_subtitleLabel) {
        m_subtitleLabel->setText(text);
        m_subtitleLabel->setVisible(!text.isEmpty());
    }
    emit subtitleChanged();
}

QPixmap FluentCard::coverPixmap() const { return m_coverPixmap; }

void FluentCard::setCoverPixmap(const QPixmap& pixmap) {
    m_coverPixmap = pixmap;
    if (m_coverWidget) {
        m_coverWidget->setVisible(!pixmap.isNull());
        m_coverWidget->update();
    }
    emit coverPixmapChanged();
}

void FluentCard::setCoverPixmap(const QString& resourcePath) {
    setCoverPixmap(QPixmap(resourcePath));
}

int FluentCard::coverHeight() const { return m_coverHeight; }

void FluentCard::setCoverHeight(int h) {
    if (m_coverHeight == h) return;
    m_coverHeight = qMax(0, h);
    if (m_coverWidget) m_coverWidget->setFixedHeight(m_coverHeight);
    update();
}

bool FluentCard::clickable() const { return m_clickable; }

void FluentCard::setClickable(bool on) {
    if (m_clickable == on) return;
    m_clickable = on;
    if (on) setCursor(Qt::PointingHandCursor);
    else setCursor(Qt::ArrowCursor);
    emit clickableChanged();
}

bool FluentCard::hoverElevation() const { return m_hoverElevation; }

void FluentCard::setHoverElevation(bool on) {
    if (m_hoverElevation == on) return;
    m_hoverElevation = on;
    update();
    emit hoverElevationChanged();
}

int FluentCard::borderRadius() const { return m_borderRadius; }

void FluentCard::setBorderRadius(int r) {
    if (m_borderRadius == r) return;
    m_borderRadius = qMax(0, r);
    update();
    emit borderRadiusChanged();
}

int FluentCard::padding() const { return m_padding; }

void FluentCard::setPadding(int p) {
    if (m_padding == p) return;
    m_padding = qMax(0, p);
    if (m_mainLayout) m_mainLayout->setContentsMargins(m_padding, m_padding, m_padding, m_padding);
    update();
    emit paddingChanged();
}

bool FluentCard::showBorder() const { return m_showBorder; }

void FluentCard::setShowBorder(bool on) {
    if (m_showBorder == on) return;
    m_showBorder = on;
    update();
    emit showBorderChanged();
}

void FluentCard::setContentWidget(QWidget* widget) {
    if (m_contentWidget) {
        m_mainLayout->removeWidget(m_contentWidget);
        m_contentWidget->setParent(nullptr);
    }
    m_contentWidget = widget;
    if (m_contentWidget && m_mainLayout) {
        // 插入到标题之后、footer 之前
        int idx = m_mainLayout->indexOf(m_footerWidget);
        if (idx >= 0) m_mainLayout->insertWidget(idx, m_contentWidget, 1);
        else m_mainLayout->addWidget(m_contentWidget, 1);
    }
}

QWidget* FluentCard::contentWidget() const { return m_contentWidget; }

void FluentCard::setFooterWidget(QWidget* widget) {
    if (m_footerWidget) {
        m_mainLayout->removeWidget(m_footerWidget);
        m_footerWidget->setParent(nullptr);
    }
    m_footerWidget = widget;
    if (m_footerWidget && m_mainLayout) {
        m_mainLayout->addWidget(m_footerWidget);
    }
}

QWidget* FluentCard::footerWidget() const { return m_footerWidget; }

QSize FluentCard::sizeHint() const { return QSize(280, 200); }
QSize FluentCard::minimumSizeHint() const { return QSize(180, 120); }

// ============================================================
// 绘制
// ============================================================

void FluentCard::paintFluent(QPainter* painter) {
    painter->setRenderHint(QPainter::Antialiasing);

    auto* t = theme();
    QRectF rect(0, 0, width(), height());
    qreal r = m_borderRadius;

    // 阴影效果 — 悬停时加深
    if (m_hoverElevation) {
        bool hovered = isHovered();
        int shadowSize = hovered ? 8 : 4;
        int shadowAlpha = hovered ? 40 : 20;

        // 手绘阴影（多圈半透明矩形模拟）
        for (int i = shadowSize; i > 0; --i) {
            int alpha = qRound(shadowAlpha * (1.0 - qreal(i) / shadowSize));
            QColor shadowColor(0, 0, 0, alpha);
            painter->setPen(Qt::NoPen);
            painter->setBrush(shadowColor);
            painter->drawRoundedRect(rect.adjusted(-i, -i, i, i), r + i, r + i);
        }
    }

    // 背景
    QColor bgColor = t->cardColor();
    if (m_clickable && isPressed()) {
        bgColor = bgColor.darker(105);
    } else if (m_clickable && isHovered()) {
        bgColor = bgColor.lighter(102);
    }

    painter->setPen(Qt::NoPen);
    painter->setBrush(bgColor);
    painter->drawRoundedRect(rect, r, r);

    // 边框
    if (m_showBorder) {
        QColor borderColor = t->borderColor();
        if (m_clickable && isHovered()) {
            borderColor = t->primaryColor();
        }
        painter->setPen(QPen(borderColor, 1));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(rect.adjusted(0.5, 0.5, -0.5, -0.5), r, r);
    }

    // 悬停时顶部强调线（仅可点击模式）
    if (m_clickable && isHovered()) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(t->primaryColor());
        QPainterPath clipPath;
        clipPath.addRoundedRect(rect, r, r);
        painter->save();
        painter->setClipPath(clipPath);
        painter->drawRect(QRectF(0, 0, width(), 3));
        painter->restore();
    }
}

// ============================================================
// 事件
// ============================================================

void FluentCard::mousePressEvent(QMouseEvent* event) {
    if (m_clickable && event->button() == Qt::LeftButton) {
        m_pressed = true;
        update();
    }
    FluentWidget::mousePressEvent(event);
}

void FluentCard::mouseReleaseEvent(QMouseEvent* event) {
    if (m_clickable && event->button() == Qt::LeftButton) {
        m_pressed = false;
        update();
        if (rect().contains(event->pos())) {
            emit clicked();
        }
    }
    FluentWidget::mouseReleaseEvent(event);
}

bool FluentCard::eventFilter(QObject* obj, QEvent* event) {
    // 封面区域自绘
    if (obj == m_coverWidget && event->type() == QEvent::Paint) {
        if (m_coverPixmap.isNull()) return FluentWidget::eventFilter(obj, event);

        QPainter p(m_coverWidget);
        p.setRenderHint(QPainter::Antialiasing);
        QRectF cr(0, 0, m_coverWidget->width(), m_coverWidget->height());
        qreal r = m_borderRadius - 2;
        if (r < 0) r = 0;

        // 圆角裁剪
        QPainterPath clip;
        clip.addRoundedRect(cr, r, r);
        p.setClipPath(clip);

        // 保持比例居中绘制
        QPixmap scaled = m_coverPixmap.scaled(
            m_coverWidget->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        int x = (m_coverWidget->width() - scaled.width()) / 2;
        int y = (m_coverWidget->height() - scaled.height()) / 2;
        p.drawPixmap(x, y, scaled);

        return true;
    }
    return FluentWidget::eventFilter(obj, event);
}

// ============================================================
// 内部初始化
// ============================================================

void FluentCard::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(m_padding, m_padding, m_padding, m_padding);
    m_mainLayout->setSpacing(8);

    // 封面图片区域（自绘）
    m_coverWidget = new QWidget(this);
    m_coverWidget->setFixedHeight(m_coverHeight);
    m_coverWidget->setVisible(false);
    m_coverWidget->setObjectName("fluentCardCover");

    // 标题
    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("fluentCardTitle");
    m_titleLabel->setVisible(false);
    m_titleLabel->setWordWrap(true);

    // 副标题
    m_subtitleLabel = new QLabel(this);
    m_subtitleLabel->setObjectName("fluentCardSubtitle");
    m_subtitleLabel->setVisible(false);
    m_subtitleLabel->setWordWrap(true);

    m_mainLayout->addWidget(m_coverWidget);
    m_mainLayout->addWidget(m_titleLabel);
    m_mainLayout->addWidget(m_subtitleLabel);
    m_mainLayout->addStretch();

    // 封面自绘：覆盖 paintEvent
    m_coverWidget->installEventFilter(this);
}

void FluentCard::applyTheme() {
    auto* t = FluentThemeManager::instance().currentTheme();

    // 封面区域样式（通过 objectName 限定）
    m_coverWidget->setStyleSheet(
        QString("#fluentCardCover {"
                "  background-color: %1;"
                "  border-radius: %2px;"
                "}")
        .arg(t->dividerColor().name())
        .arg(m_borderRadius - 2)
    );

    m_titleLabel->setStyleSheet(
        QString("#fluentCardTitle {"
                "  color: %1;"
                "  font-size: 16px;"
                "  font-weight: bold;"
                "  border: none;"
                "}")
        .arg(t->textColorPrimary().name())
    );

    m_subtitleLabel->setStyleSheet(
        QString("#fluentCardSubtitle {"
                "  color: %1;"
                "  font-size: 13px;"
                "  border: none;"
                "}")
        .arg(t->textColorSecondary().name())
    );
}

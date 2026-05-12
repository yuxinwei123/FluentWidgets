#include "FluentButton.h"
#include "Core/FluentTheme.h"
#include "Core/FluentConfig.h"
#include "Animations/FluentAnimation.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QKeyEvent>

FluentButton::FluentButton(const QString& text, FluentButtonVariant variant, QWidget* parent)
    : FluentWidget(parent)
    , m_text(text)
    , m_variant(variant)
    , m_accentColor(theme()->primaryColor())
{
    setupSize();
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::StrongFocus);

    connect(theme(), &FluentTheme::themeChanged, this, [this]() {
        m_accentColor = theme()->primaryColor();
        update();
    });
}

QString FluentButton::text() const { return m_text; }
void FluentButton::setText(const QString& text) {
    if (m_text != text) { m_text = text; update(); emit textChanged(); }
}

FluentButtonVariant FluentButton::variant() const { return m_variant; }
void FluentButton::setVariant(FluentButtonVariant variant) {
    if (m_variant != variant) { m_variant = variant; update(); emit variantChanged(); }
}

FluentSize FluentButton::size() const { return m_size; }
void FluentButton::setSize(FluentSize size) {
    if (m_size != size) { m_size = size; setupSize(); update(); emit sizeChanged(); }
}

QColor FluentButton::accentColor() const { return m_accentColor; }
void FluentButton::setAccentColor(const QColor& color) {
    if (m_accentColor != color) { m_accentColor = color; update(); emit accentColorChanged(); }
}

QSize FluentButton::sizeHint() const {
    QFontMetrics fm(theme()->bodyFont());
    int textWidth = fm.horizontalAdvance(m_text);
    int h = height();
    return QSize(textWidth + 24, h);
}

QSize FluentButton::minimumSizeHint() const {
    return QSize(60, height());
}

void FluentButton::paintFluent(QPainter* painter) {
    auto* t = theme();
    const QRect r = rect();
    const int radius = t->cornerRadius();

    // 1. 绘制背景
    QPainterPath path;
    path.addRoundedRect(r, radius, radius);

    painter->setPen(Qt::NoPen);
    painter->setBrush(backgroundColorForState());
    painter->drawPath(path);

    // 2. 绘制边框 (Secondary 变体)
    if (m_variant == FluentButtonVariant::Secondary && !hasFocus()) {
        QPen pen(borderColorForState(), t->borderWidth());
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(path);
    }

    // 3. 绘制焦点边框
    if (hasFocus()) {
        QPen pen(t->primaryColor(), t->focusBorderWidth());
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        QPainterPath focusPath;
        focusPath.addRoundedRect(r.adjusted(1, 1, -1, -1), radius, radius);
        painter->drawPath(focusPath);
    }

    // 4. 绘制 Hover 叠加
    if (isHovered() && !isPressed()) {
        QColor hoverColor = t->textColorPrimary();
        hoverColor.setAlphaF(0.06);
        painter->fillPath(path, hoverColor);
    }

    // 5. 绘制 Pressed 叠加
    if (isPressed()) {
        QColor pressColor = t->textColorPrimary();
        pressColor.setAlphaF(0.1);
        painter->fillPath(path, pressColor);
    }

    // 6. 绘制文字
    painter->setPen(textColorForState());
    painter->setFont(t->bodyFont());
    painter->drawText(r, Qt::AlignCenter, m_text);
}

void FluentButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        FluentWidget::mousePressEvent(event);
    }
}

void FluentButton::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        FluentWidget::mouseReleaseEvent(event);
        if (rect().contains(event->pos())) {
            emit clicked();
        }
    }
}

void FluentButton::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        emit clicked();
    }
    FluentWidget::keyPressEvent(event);
}

void FluentButton::focusInEvent(QFocusEvent* event) {
    FluentWidget::focusInEvent(event);
    update();
}

void FluentButton::focusOutEvent(QFocusEvent* event) {
    FluentWidget::focusOutEvent(event);
    update();
}

void FluentButton::setupSize() {
    int h = 0;
    switch (m_size) {
    case FluentSize::Small:  h = theme()->controlHeightSmall(); break;
    case FluentSize::Medium: h = theme()->controlHeight(); break;
    case FluentSize::Large:  h = theme()->controlHeightLarge(); break;
    }
    setFixedHeight(h);
    setMinimumWidth(60);
}

QColor FluentButton::backgroundColorForState() const {
    auto* t = theme();
    switch (m_variant) {
    case FluentButtonVariant::Primary:
    case FluentButtonVariant::Accent:
        return m_accentColor;
    case FluentButtonVariant::Secondary:
        return t->surfaceColor();
    case FluentButtonVariant::Subtle:
    case FluentButtonVariant::Transparent:
        return Qt::transparent;
    case FluentButtonVariant::Danger:
        return t->errorColor();
    case FluentButtonVariant::Toggle:
    case FluentButtonVariant::Split:
        return t->surfaceColor();
    }
    return t->surfaceColor();
}

QColor FluentButton::textColorForState() const {
    auto* t = theme();
    if (!isEnabled()) return t->textColorDisabled();

    switch (m_variant) {
    case FluentButtonVariant::Primary:
    case FluentButtonVariant::Accent:
    case FluentButtonVariant::Danger:
        return QColor("#FFFFFF");
    case FluentButtonVariant::Secondary:
    case FluentButtonVariant::Subtle:
    case FluentButtonVariant::Transparent:
    case FluentButtonVariant::Toggle:
    case FluentButtonVariant::Split:
        return t->textColorPrimary();
    }
    return t->textColorPrimary();
}

QColor FluentButton::borderColorForState() const {
    return theme()->borderColor();
}

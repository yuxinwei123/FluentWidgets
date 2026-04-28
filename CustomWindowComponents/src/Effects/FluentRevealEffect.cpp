#include "FluentRevealEffect.h"
#include <QWidget>
#include <QPainter>
#include <QRadialGradient>

FluentRevealEffect::FluentRevealEffect(QObject* parent)
    : QObject(parent)
    , m_revealColor(255, 255, 255)
{
}

QColor FluentRevealEffect::revealColor() const { return m_revealColor; }
void FluentRevealEffect::setRevealColor(const QColor& color) {
    if (m_revealColor != color) { m_revealColor = color; emit effectChanged(); }
}

qreal FluentRevealEffect::revealRadius() const { return m_revealRadius; }
void FluentRevealEffect::setRevealRadius(qreal radius) {
    if (!qFuzzyCompare(m_revealRadius, radius)) { m_revealRadius = radius; emit effectChanged(); }
}

qreal FluentRevealEffect::revealOpacity() const { return m_revealOpacity; }
void FluentRevealEffect::setRevealOpacity(qreal opacity) {
    if (!qFuzzyCompare(m_revealOpacity, opacity)) { m_revealOpacity = opacity; emit effectChanged(); }
}

void FluentRevealEffect::attachTo(QWidget* target) {
    m_target = target;
    if (target) {
        target->setMouseTracking(true);
    }
}

void FluentRevealEffect::detachFrom(QWidget* /*target*/) {
    m_target = nullptr;
}

void FluentRevealEffect::setMousePosition(const QPoint& pos) {
    m_mousePos = pos;
}

void FluentRevealEffect::paint(QPainter* painter, const QRect& rect) {
    if (!m_target) return;

    // 以鼠标位置为中心绘制径向渐变
    QRadialGradient gradient(m_mousePos, m_revealRadius);
    QColor centerColor = m_revealColor;
    centerColor.setAlphaF(m_revealOpacity);
    QColor edgeColor = m_revealColor;
    edgeColor.setAlphaF(0.0);

    gradient.setColorAt(0.0, centerColor);
    gradient.setColorAt(1.0, edgeColor);

    painter->save();
    painter->setClipRect(rect);
    painter->setBrush(gradient);
    painter->setPen(Qt::NoPen);
    painter->drawRect(rect);
    painter->restore();
}

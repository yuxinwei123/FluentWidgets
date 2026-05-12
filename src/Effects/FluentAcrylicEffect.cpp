#include "FluentAcrylicEffect.h"
#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QGraphicsBlurEffect>

FluentAcrylicEffect::FluentAcrylicEffect(QObject* parent)
    : QObject(parent)
    , m_tintColor(255, 255, 255)
{
}

int FluentAcrylicEffect::blurRadius() const { return m_blurRadius; }
void FluentAcrylicEffect::setBlurRadius(int radius) {
    if (m_blurRadius != radius) { m_blurRadius = radius; emit effectChanged(); }
}

qreal FluentAcrylicEffect::opacity() const { return m_opacity; }
void FluentAcrylicEffect::setOpacity(qreal opacity) {
    if (!qFuzzyCompare(m_opacity, opacity)) { m_opacity = opacity; emit effectChanged(); }
}

QColor FluentAcrylicEffect::tintColor() const { return m_tintColor; }
void FluentAcrylicEffect::setTintColor(const QColor& color) {
    if (m_tintColor != color) { m_tintColor = color; emit effectChanged(); }
}

qreal FluentAcrylicEffect::tintOpacity() const { return m_tintOpacity; }
void FluentAcrylicEffect::setTintOpacity(qreal opacity) {
    if (!qFuzzyCompare(m_tintOpacity, opacity)) { m_tintOpacity = opacity; emit effectChanged(); }
}

qreal FluentAcrylicEffect::noiseOpacity() const { return m_noiseOpacity; }
void FluentAcrylicEffect::setNoiseOpacity(qreal opacity) {
    if (!qFuzzyCompare(m_noiseOpacity, opacity)) { m_noiseOpacity = opacity; emit effectChanged(); }
}

void FluentAcrylicEffect::attachTo(QWidget* target) {
    m_target = target;
}

void FluentAcrylicEffect::detachFrom(QWidget* /*target*/) {
    m_target = nullptr;
}

void FluentAcrylicEffect::paint(QPainter* painter, const QRect& rect) {
    if (!m_target) return;

    // 1. 捕获背景
    QImage behind = m_target->grab().toImage();

    // 2. 应用模糊 (简化版，生产环境应使用 GPU 加速)
    QImage blurred = behind.scaled(
        behind.size() / (m_blurRadius / 2 + 1),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
    ).scaled(behind.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // 3. 绘制模糊背景
    painter->drawImage(rect, blurred);

    // 4. 叠加色彩层
    QColor tint = m_tintColor;
    tint.setAlphaF(m_tintOpacity);
    painter->fillRect(rect, tint);
}

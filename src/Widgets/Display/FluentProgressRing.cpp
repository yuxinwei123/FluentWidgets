#include "FluentProgressRing.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QConicalGradient>
#include <QtMath>

// ============================================================
// FluentProgressRing
// ============================================================

FluentProgressRing::FluentProgressRing(QWidget* parent)
    : FluentWidget(parent)
{
    setMinimumSize(minimumSizeHint());

    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

FluentProgressRing::~FluentProgressRing() {
    if (m_timerId) killTimer(m_timerId);
}

// ============================================================
// 属性
// ============================================================

int FluentProgressRing::value() const { return m_value; }

void FluentProgressRing::setValue(int val) {
    val = qBound(m_minimum, val, m_maximum);
    if (m_value == val) return;
    m_value = val;
    update();
    emit valueChanged(val);
}

int FluentProgressRing::maximum() const { return m_maximum; }

void FluentProgressRing::setMaximum(int max) {
    if (m_maximum == max) return;
    m_maximum = max;
    if (m_value > m_maximum) setValue(m_maximum);
    emit maximumChanged();
}

int FluentProgressRing::minimum() const { return m_minimum; }

void FluentProgressRing::setMinimum(int min) {
    if (m_minimum == min) return;
    m_minimum = min;
    if (m_value < m_minimum) setValue(m_minimum);
    emit minimumChanged();
}

void FluentProgressRing::setRange(int min, int max) {
    setMinimum(min);
    setMaximum(max);
}

qreal FluentProgressRing::percentage() const {
    if (m_maximum <= m_minimum) return 0.0;
    return qreal(m_value - m_minimum) / qreal(m_maximum - m_minimum);
}

bool FluentProgressRing::indeterminate() const { return m_indeterminate; }

void FluentProgressRing::setIndeterminate(bool on) {
    if (m_indeterminate == on) return;
    m_indeterminate = on;
    if (on) {
        m_rotationAngle = 0.0;
        m_timerId = startTimer(1000 / ANIM_FPS);
    } else {
        if (m_timerId) { killTimer(m_timerId); m_timerId = 0; }
    }
    update();
    emit indeterminateChanged();
}

int FluentProgressRing::strokeWidth() const { return m_strokeWidth; }

void FluentProgressRing::setStrokeWidth(int width) {
    if (m_strokeWidth == width) return;
    m_strokeWidth = qMax(2, width);
    update();
    emit strokeWidthChanged();
}

QColor FluentProgressRing::customColor() const { return m_customColor; }

void FluentProgressRing::setCustomColor(const QColor& color) {
    if (m_customColor == color) return;
    m_customColor = color;
    update();
    emit customColorChanged();
}

bool FluentProgressRing::showValue() const { return m_showValue; }

void FluentProgressRing::setShowValue(bool on) {
    if (m_showValue == on) return;
    m_showValue = on;
    update();
    emit showValueChanged();
}

QString FluentProgressRing::label() const { return m_label; }

void FluentProgressRing::setLabel(const QString& text) {
    if (m_label == text) return;
    m_label = text;
    update();
    emit labelChanged();
}

bool FluentProgressRing::useGradient() const { return m_useGradient; }

void FluentProgressRing::setUseGradient(bool on) {
    if (m_useGradient == on) return;
    m_useGradient = on;
    update();
    emit useGradientChanged();
}

QSize FluentProgressRing::sizeHint() const { return QSize(DEFAULT_SIZE, DEFAULT_SIZE); }
QSize FluentProgressRing::minimumSizeHint() const { return QSize(MIN_SIZE, MIN_SIZE); }

// ============================================================
// 颜色
// ============================================================

QColor FluentProgressRing::progressColor() const {
    if (m_customColor.isValid()) return m_customColor;
    return theme()->primaryColor();
}

// ============================================================
// 绘制
// ============================================================

void FluentProgressRing::paintFluent(QPainter* painter) {
    painter->setRenderHint(QPainter::Antialiasing);

    auto* t = theme();
    int size = qMin(width(), height());
    int cx = width() / 2;
    int cy = height() / 2;

    int sw = qMin(m_strokeWidth, size / 4); // 线宽不超过尺寸的1/4
    int radius = (size - sw * 2) / 2;
    if (radius < 4) radius = 4;

    QRectF arcRect(cx - radius - sw / 2.0, cy - radius - sw / 2.0,
                   (radius + sw / 2.0) * 2, (radius + sw / 2.0) * 2);

    QColor color = progressColor();

    if (m_indeterminate) {
        // --- 不确定模式：旋转弧线 ---
        QConicalGradient grad(cx, cy, m_rotationAngle);
        QColor startColor = color;
        startColor.setAlphaF(0.0);
        grad.setColorAt(0.0, startColor);
        grad.setColorAt(0.25, color);
        grad.setColorAt(0.35, color);
        grad.setColorAt(0.35, startColor);
        grad.setColorAt(1.0, startColor);

        // 轨道背景
        painter->setPen(QPen(t->dividerColor(), sw, Qt::SolidLine, Qt::RoundCap));
        painter->setBrush(Qt::NoBrush);
        painter->drawArc(arcRect, 0, 360 * 16);

        // 旋转弧
        painter->setPen(QPen(QBrush(grad), sw, Qt::SolidLine, Qt::RoundCap));
        painter->drawArc(arcRect, 0, 126 * 16); // 约 126° 弧

    } else {
        // --- 确定模式 ---
        qreal pct = percentage();

        // 轨道背景
        painter->setPen(QPen(t->dividerColor(), sw, Qt::SolidLine, Qt::RoundCap));
        painter->setBrush(Qt::NoBrush);
        painter->drawArc(arcRect, 0, 360 * 16);

        // 进度弧
        int spanAngle = qRound(pct * 360 * 16);
        if (spanAngle > 0) {
            QBrush brush;
            if (m_useGradient) {
                // 渐变色弧
                QConicalGradient coniGrad(cx, cy, 90 * 16); // 从顶部开始
                QColor c1 = color;
                QColor c2 = color.lighter(140);
                QColor c3 = color.darker(120);
                coniGrad.setColorAt(0.0, c1);
                coniGrad.setColorAt(0.5, c2);
                coniGrad.setColorAt(1.0, c3);
                brush = QBrush(coniGrad);
            } else {
                brush = QBrush(color);
            }

            painter->setPen(QPen(brush, sw, Qt::SolidLine, Qt::RoundCap));
            painter->drawArc(arcRect, 90 * 16, -spanAngle); // 从顶部顺时针
        }

        // 中心文字
        if (m_showValue || !m_label.isEmpty()) {
            QRectF textRect(cx - radius, cy - radius, radius * 2, radius * 2);
            if (m_showValue) {
                painter->setPen(t->textColorPrimary());
                painter->setFont(t->subtitleFont());
                painter->drawText(textRect, Qt::AlignCenter,
                                  QString::number(qRound(pct * 100)) + u8"%");
            }
            if (!m_label.isEmpty()) {
                painter->setPen(t->textColorSecondary());
                painter->setFont(t->captionFont());
                QRectF labelRect(cx - radius, cy + radius * 0.15, radius * 2, radius * 0.7);
                painter->drawText(labelRect, Qt::AlignCenter, m_label);
            }
        }
    }
}

// ============================================================
// 定时器动画
// ============================================================

void FluentProgressRing::timerEvent(QTimerEvent* event) {
    if (event->timerId() == m_timerId) {
        m_rotationAngle += 6.0; // 每帧旋转 6°
        if (m_rotationAngle >= 360.0) m_rotationAngle -= 360.0;
        update();
    }
}

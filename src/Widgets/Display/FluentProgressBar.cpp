#include "FluentProgressBar.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QConicalGradient>
#include <QtMath>

// ============================================================
// FluentProgressBar
// ============================================================

FluentProgressBar::FluentProgressBar(QWidget* parent)
    : FluentWidget(parent)
{
    setMinimumSize(minimumSizeHint());

    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

FluentProgressBar::~FluentProgressBar() {
    if (m_animTimerId) killTimer(m_animTimerId);
    if (m_stripeTimerId) killTimer(m_stripeTimerId);
}

// ============================================================
// 属性
// ============================================================

int FluentProgressBar::value() const { return m_value; }

void FluentProgressBar::setValue(int val) {
    val = qBound(m_minimum, val, m_maximum);
    if (m_value == val) return;
    m_value = val;
    update();
    emit valueChanged(val);
}

int FluentProgressBar::maximum() const { return m_maximum; }
void FluentProgressBar::setMaximum(int max) {
    if (m_maximum == max) return;
    m_maximum = max;
    if (m_value > m_maximum) setValue(m_maximum);
    emit maximumChanged();
}

int FluentProgressBar::minimum() const { return m_minimum; }
void FluentProgressBar::setMinimum(int min) {
    if (m_minimum == min) return;
    m_minimum = min;
    if (m_value < m_minimum) setValue(m_minimum);
    emit minimumChanged();
}

void FluentProgressBar::setRange(int min, int max) {
    setMinimum(min);
    setMaximum(max);
}

qreal FluentProgressBar::percentage() const {
    if (m_maximum <= m_minimum) return 0.0;
    return qreal(m_value - m_minimum) / qreal(m_maximum - m_minimum);
}

bool FluentProgressBar::indeterminate() const { return m_indeterminate; }

void FluentProgressBar::setIndeterminate(bool on) {
    if (m_indeterminate == on) return;
    m_indeterminate = on;
    if (on) {
        m_indeterminateOffset = 0.0;
        m_animTimerId = startTimer(1000 / ANIM_FPS);
    } else {
        if (m_animTimerId) { killTimer(m_animTimerId); m_animTimerId = 0; }
    }
    update();
    emit indeterminateChanged();
}

FluentProgressBar::BarStyle FluentProgressBar::barStyle() const { return m_barStyle; }

void FluentProgressBar::setBarStyle(BarStyle style) {
    if (m_barStyle == style) return;
    m_barStyle = style;
    update();
    emit barStyleChanged();
}

bool FluentProgressBar::striped() const { return m_striped; }
void FluentProgressBar::setStriped(bool on) { m_striped = on; update(); emit stripedChanged(); }

bool FluentProgressBar::stripedAnimated() const { return m_stripedAnimated; }

void FluentProgressBar::setStripedAnimated(bool on) {
    if (m_stripedAnimated == on) return;
    m_stripedAnimated = on;
    if (on) {
        m_stripeOffset = 0;
        m_stripeTimerId = startTimer(1000 / ANIM_FPS);
    } else {
        if (m_stripeTimerId) { killTimer(m_stripeTimerId); m_stripeTimerId = 0; }
    }
    update();
    emit stripedAnimatedChanged();
}

QColor FluentProgressBar::customColor() const { return m_customColor; }
void FluentProgressBar::setCustomColor(const QColor& color) { m_customColor = color; update(); emit customColorChanged(); }

QString FluentProgressBar::label() const { return m_label; }
void FluentProgressBar::setLabel(const QString& text) { m_label = text; update(); emit labelChanged(); }

bool FluentProgressBar::showValue() const { return m_showValue; }
void FluentProgressBar::setShowValue(bool on) { m_showValue = on; update(); emit showValueChanged(); }

QSize FluentProgressBar::sizeHint() const {
    if (m_barStyle == RingBar) return QSize(RING_SIZE + 40, RING_SIZE + 40);
    return QSize(300, LINE_BAR_HEIGHT + 20);
}

QSize FluentProgressBar::minimumSizeHint() const {
    if (m_barStyle == RingBar) return QSize(RING_SIZE, RING_SIZE);
    return QSize(50, LINE_BAR_HEIGHT + 10);
}

// ============================================================
// 颜色
// ============================================================

QColor FluentProgressBar::progressColor() const {
    if (m_customColor.isValid()) return m_customColor;
    return theme()->primaryColor();
}

// ============================================================
// 绘制
// ============================================================

void FluentProgressBar::paintFluent(QPainter* painter) {
    painter->setRenderHint(QPainter::Antialiasing);

    if (m_barStyle == RingBar) {
        drawRingBar(painter);
    } else {
        drawLineBar(painter);
    }
}

void FluentProgressBar::drawLineBar(QPainter* painter) {
    auto* t = theme();
    int barH = LINE_BAR_HEIGHT;
    int barY = (height() - barH) / 2;

    // 标签和数值
    bool hasText = !m_label.isEmpty() || m_showValue;
    if (hasText) barY = 16; // 文字占上方空间

    QRectF barRect(0, barY, width(), barH);

    // 轨道背景
    painter->setPen(Qt::NoPen);
    painter->setBrush(t->dividerColor());
    painter->drawRoundedRect(barRect, LINE_BAR_RADIUS, LINE_BAR_RADIUS);

    // 进度填充
    QColor color = progressColor();

    if (m_indeterminate) {
        // 不确定模式：滑动条
        qreal slideWidth = width() * 0.3;
        qreal slideX = m_indeterminateOffset * (width() + slideWidth) - slideWidth;
        QRectF slideRect(slideX, barY, slideWidth, barH);

        // 渐变
        QLinearGradient grad(slideRect.left(), 0, slideRect.right(), 0);
        QColor startColor = color;
        startColor.setAlphaF(0.0);
        grad.setColorAt(0.0, startColor);
        grad.setColorAt(0.3, color);
        grad.setColorAt(0.7, color);
        grad.setColorAt(1.0, startColor);

        painter->setBrush(grad);
        painter->drawRoundedRect(slideRect, LINE_BAR_RADIUS, LINE_BAR_RADIUS);
    } else {
        qreal fillW = barRect.width() * percentage();
        if (fillW > 0) {
            QRectF fillRect(0, barY, fillW, barH);

            // 条纹
            if (m_striped) {
                QPainterPath clipPath;
                clipPath.addRoundedRect(fillRect, LINE_BAR_RADIUS, LINE_BAR_RADIUS);
                painter->save();
                painter->setClipPath(clipPath);

                // 先填充底色
                painter->setBrush(color);
                painter->drawRect(fillRect);

                // 再绘制条纹
                QColor stripeColor = QColor(255, 255, 255, 40);
                painter->setBrush(stripeColor);
                int offset = m_stripedAnimated ? m_stripeOffset : 0;
                for (int x = -STRIPE_WIDTH * 2 + offset; x < fillW + STRIPE_WIDTH * 2; x += STRIPE_WIDTH * 2) {
                    QPolygonF stripe;
                    stripe << QPointF(x, barY + barH)
                           << QPointF(x + STRIPE_WIDTH, barY + barH)
                           << QPointF(x + STRIPE_WIDTH + barH, barY)
                           << QPointF(x + barH, barY);
                    painter->drawPolygon(stripe);
                }

                painter->restore();
            } else {
                painter->setBrush(color);
                painter->drawRoundedRect(fillRect, LINE_BAR_RADIUS, LINE_BAR_RADIUS);
            }
        }
    }

    // 文字
    if (hasText) {
        painter->setPen(t->textColorPrimary());
        painter->setFont(t->captionFont());

        QString leftText = m_label;
        QString rightText;
        if (m_showValue && !m_indeterminate) {
            rightText = QString::number(qRound(percentage() * 100)) + u8"%";
        }

        if (!leftText.isEmpty()) {
            painter->drawText(QRect(0, 0, width() / 2, 14), Qt::AlignVCenter | Qt::AlignLeft, leftText);
        }
        if (!rightText.isEmpty()) {
            painter->drawText(QRect(width() / 2, 0, width() / 2, 14), Qt::AlignVCenter | Qt::AlignRight, rightText);
        }
    }
}

void FluentProgressBar::drawRingBar(QPainter* painter) {
    auto* t = theme();
    int size = qMin(width(), height());
    int cx = width() / 2;
    int cy = height() / 2;

    int strokeWidth = RING_STROKE_WIDTH;
    int radius = (size - strokeWidth * 2) / 2;
    if (radius < 10) radius = 10;

    QRectF arcRect(cx - radius - strokeWidth / 2, cy - radius - strokeWidth / 2,
                   (radius + strokeWidth / 2) * 2, (radius + strokeWidth / 2) * 2);

    QColor color = progressColor();

    if (m_indeterminate) {
        // 不确定模式：旋转弧线
        QConicalGradient grad(cx, cy, m_indeterminateOffset * 360.0);
        QColor startColor = color;
        startColor.setAlphaF(0.0);
        grad.setColorAt(0.0, startColor);
        grad.setColorAt(0.3, color);
        grad.setColorAt(0.5, color);
        grad.setColorAt(0.5, startColor);
        grad.setColorAt(1.0, startColor);

        // 轨道
        painter->setPen(QPen(t->dividerColor(), strokeWidth, Qt::SolidLine, Qt::RoundCap));
        painter->setBrush(Qt::NoBrush);
        painter->drawArc(arcRect, 0, 360 * 16);

        // 进度弧
        painter->setPen(QPen(QBrush(grad), strokeWidth, Qt::SolidLine, Qt::RoundCap));
        painter->drawArc(arcRect, 0, 180 * 16);
    } else {
        // 轨道
        painter->setPen(QPen(t->dividerColor(), strokeWidth, Qt::SolidLine, Qt::RoundCap));
        painter->setBrush(Qt::NoBrush);
        painter->drawArc(arcRect, 0, 360 * 16);

        // 进度弧
        qreal pct = percentage();
        int spanAngle = qRound(pct * 360 * 16);
        if (spanAngle > 0) {
            // 条纹效果对环形不太好做，跳过
            painter->setPen(QPen(color, strokeWidth, Qt::SolidLine, Qt::RoundCap));
            painter->drawArc(arcRect, 90 * 16, -spanAngle); // 从顶部开始，顺时针
        }

        // 中心文字
        if (m_showValue || !m_label.isEmpty()) {
            painter->setPen(t->textColorPrimary());
            if (m_showValue) {
                painter->setFont(t->subtitleFont());
                painter->drawText(arcRect, Qt::AlignCenter, QString::number(qRound(pct * 100)) + u8"%");
            } else {
                painter->setFont(t->bodyFont());
                painter->drawText(arcRect, Qt::AlignCenter, m_label);
            }
        }
    }
}

// ============================================================
// 定时器动画
// ============================================================

void FluentProgressBar::timerEvent(QTimerEvent* event) {
    if (event->timerId() == m_animTimerId) {
        if (m_barStyle == LineBar) {
            m_indeterminateOffset += 0.008 * INDETERMINATE_SPEED;
            if (m_indeterminateOffset > 1.0 + 0.3) m_indeterminateOffset = -0.3;
        } else {
            m_indeterminateOffset += 0.01 * INDETERMINATE_SPEED;
            if (m_indeterminateOffset >= 1.0) m_indeterminateOffset = 0.0;
        }
        update();
    } else if (event->timerId() == m_stripeTimerId) {
        m_stripeOffset = (m_stripeOffset + STRIPE_SPEED) % (STRIPE_WIDTH * 2);
        update();
    }
}

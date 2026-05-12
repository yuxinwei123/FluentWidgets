#include "FluentSlider.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QtMath>

FluentSlider::FluentSlider(Qt::Orientation orient, QWidget* parent)
    : FluentWidget(parent)
    , m_orientation(orient)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    if (m_orientation == Qt::Horizontal)
        setMinimumHeight(36);
    else
        setMinimumWidth(36);
}

FluentSlider::~FluentSlider() = default;

// === 属性 ===

Qt::Orientation FluentSlider::orientation() const { return m_orientation; }
void FluentSlider::setOrientation(Qt::Orientation orient) {
    if (m_orientation != orient) {
        m_orientation = orient;
        if (orient == Qt::Horizontal)
            setMinimumHeight(36);
        else
            setMinimumWidth(36);
        emit orientationChanged();
        update();
    }
}

qreal FluentSlider::minimum() const { return m_minimum; }
void FluentSlider::setMinimum(qreal min) {
    if (m_minimum != min) {
        m_minimum = min;
        if (m_value < m_minimum) setValue(m_minimum);
        if (m_secondValue < m_minimum) setSecondValue(m_minimum);
        emit minimumChanged();
        update();
    }
}

qreal FluentSlider::maximum() const { return m_maximum; }
void FluentSlider::setMaximum(qreal max) {
    if (m_maximum != max) {
        m_maximum = max;
        if (m_value > m_maximum) setValue(m_maximum);
        if (m_secondValue > m_maximum) setSecondValue(m_maximum);
        emit maximumChanged();
        update();
    }
}

void FluentSlider::setRange(qreal min, qreal max) {
    m_minimum = min;
    m_maximum = qMax(min, max);
    setValue(clampedValue(m_value));
    if (m_rangeMode) setSecondValue(clampedValue(m_secondValue));
    emit minimumChanged();
    emit maximumChanged();
    update();
}

qreal FluentSlider::value() const { return m_value; }
void FluentSlider::setValue(qreal val) {
    val = clampedValue(val);
    if (!qFuzzyCompare(m_value, val)) {
        m_value = val;
        emit valueChanged(val);
        if (m_rangeMode) emit rangeChanged(m_value, m_secondValue);
        update();
    }
}

qreal FluentSlider::secondValue() const { return m_secondValue; }
void FluentSlider::setSecondValue(qreal val) {
    val = clampedValue(val);
    if (!qFuzzyCompare(m_secondValue, val)) {
        m_secondValue = val;
        emit secondValueChanged(val);
        emit rangeChanged(m_value, m_secondValue);
        update();
    }
}

qreal FluentSlider::firstValue() const { return m_value; }

qreal FluentSlider::singleStep() const { return m_singleStep; }
void FluentSlider::setSingleStep(qreal step) {
    if (m_singleStep != step) { m_singleStep = qMax(0.0, step); emit singleStepChanged(); }
}

qreal FluentSlider::pageStep() const { return m_pageStep; }
void FluentSlider::setPageStep(qreal step) {
    if (m_pageStep != step) { m_pageStep = qMax(0.0, step); emit pageStepChanged(); }
}

bool FluentSlider::rangeMode() const { return m_rangeMode; }
void FluentSlider::setRangeMode(bool on) {
    if (m_rangeMode != on) { m_rangeMode = on; emit rangeModeChanged(); update(); }
}

bool FluentSlider::showTicks() const { return m_showTicks; }
void FluentSlider::setShowTicks(bool on) { if (m_showTicks != on) { m_showTicks = on; emit showTicksChanged(); update(); } }

FluentSlider::TickPosition FluentSlider::tickPosition() const { return m_tickPosition; }
void FluentSlider::setTickPosition(TickPosition pos) { m_tickPosition = pos; update(); }

int FluentSlider::tickInterval() const { return m_tickInterval; }
void FluentSlider::setTickInterval(int interval) { m_tickInterval = qMax(0, interval); update(); }

bool FluentSlider::showValueLabel() const { return m_showValueLabel; }
void FluentSlider::setShowValueLabel(bool on) { if (m_showValueLabel != on) { m_showValueLabel = on; emit showValueLabelChanged(); update(); } }

bool FluentSlider::invertedAppearance() const { return m_invertedAppearance; }
void FluentSlider::setInvertedAppearance(bool inv) { if (m_invertedAppearance != inv) { m_invertedAppearance = inv; emit invertedAppearanceChanged(); update(); } }

QString FluentSlider::label() const { return m_label; }
void FluentSlider::setLabel(const QString& text) { if (m_label != text) { m_label = text; emit labelChanged(); update(); } }

qreal FluentSlider::thumbProgress() const { return m_thumbProgress; }
void FluentSlider::setThumbProgress(qreal p) { m_thumbProgress = p; emit thumbProgressChanged(); }

QSize FluentSlider::sizeHint() const {
    return m_orientation == Qt::Horizontal ? QSize(200, 36) : QSize(36, 200);
}

QSize FluentSlider::minimumSizeHint() const {
    return m_orientation == Qt::Horizontal ? QSize(MIN_SLIDER_LENGTH, 36) : QSize(36, MIN_SLIDER_LENGTH);
}

// === 布局计算 ===

QRectF FluentSlider::trackRect() const {
    if (m_orientation == Qt::Horizontal) {
        qreal x = THUMB_SIZE / 2.0;
        qreal y = height() / 2.0 - TRACK_HEIGHT / 2.0;
        qreal w = width() - THUMB_SIZE;
        return QRectF(x, y, w, TRACK_HEIGHT);
    } else {
        qreal x = width() / 2.0 - TRACK_HEIGHT / 2.0;
        qreal y = THUMB_SIZE / 2.0;
        qreal h = height() - THUMB_SIZE;
        return QRectF(x, y, TRACK_HEIGHT, h);
    }
}

QRectF FluentSlider::firstThumbRect() const {
    qreal pos = valueToPos(m_value);
    if (m_orientation == Qt::Horizontal) {
        return QRectF(pos - THUMB_SIZE / 2.0, height() / 2.0 - THUMB_SIZE / 2.0,
                      THUMB_SIZE, THUMB_SIZE);
    } else {
        return QRectF(width() / 2.0 - THUMB_SIZE / 2.0, pos - THUMB_SIZE / 2.0,
                      THUMB_SIZE, THUMB_SIZE);
    }
}

QRectF FluentSlider::secondThumbRect() const {
    qreal pos = valueToPos(m_secondValue);
    if (m_orientation == Qt::Horizontal) {
        return QRectF(pos - THUMB_SIZE / 2.0, height() / 2.0 - THUMB_SIZE / 2.0,
                      THUMB_SIZE, THUMB_SIZE);
    } else {
        return QRectF(width() / 2.0 - THUMB_SIZE / 2.0, pos - THUMB_SIZE / 2.0,
                      THUMB_SIZE, THUMB_SIZE);
    }
}

QRectF FluentSlider::activeTrackRect() const {
    QRectF tr = trackRect();
    qreal p1, p2;
    if (m_rangeMode) {
        p1 = valueToPos(m_value);
        p2 = valueToPos(m_secondValue);
    } else {
        p1 = m_invertedAppearance ? valueToPos(m_maximum) : valueToPos(m_minimum);
        p2 = valueToPos(m_value);
    }
    if (p1 > p2) std::swap(p1, p2);

    if (m_orientation == Qt::Horizontal) {
        return QRectF(p1, tr.y(), p2 - p1, tr.height());
    } else {
        return QRectF(tr.x(), p1, tr.width(), p2 - p1);
    }
}

QRectF FluentSlider::valueLabelRect() const {
    if (!m_showValueLabel) return QRectF();
    QRectF thumb = m_rangeMode ? secondThumbRect() : firstThumbRect();
    if (m_orientation == Qt::Horizontal) {
        return QRectF(thumb.center().x() - 24, thumb.top() - 28, 48, 22);
    } else {
        return QRectF(thumb.right() + 6, thumb.center().y() - 11, 48, 22);
    }
}

qreal FluentSlider::valueToPos(qreal val) const {
    QRectF tr = trackRect();
    qreal ratio = (m_maximum != m_minimum) ? (val - m_minimum) / (m_maximum - m_minimum) : 0.0;
    if (m_invertedAppearance) ratio = 1.0 - ratio;

    if (m_orientation == Qt::Horizontal) {
        return tr.left() + ratio * tr.width();
    } else {
        return tr.bottom() - ratio * tr.height();
    }
}

qreal FluentSlider::posToValue(qreal pos) const {
    QRectF tr = trackRect();
    qreal ratio;
    if (m_orientation == Qt::Horizontal) {
        ratio = (tr.width() > 0) ? (pos - tr.left()) / tr.width() : 0.0;
    } else {
        ratio = (tr.height() > 0) ? (tr.bottom() - pos) / tr.height() : 0.0;
    }
    if (m_invertedAppearance) ratio = 1.0 - ratio;
    ratio = qBound(0.0, ratio, 1.0);
    return m_minimum + ratio * (m_maximum - m_minimum);
}

qreal FluentSlider::clampedValue(qreal val) const {
    return qBound(m_minimum, val, m_maximum);
}

int FluentSlider::effectiveTickInterval() const {
    if (m_tickInterval > 0) return m_tickInterval;
    // 自动计算
    qreal range = m_maximum - m_minimum;
    if (range <= 0) return 1;
    int count = m_orientation == Qt::Horizontal ? width() / 40 : height() / 40;
    count = qMax(count, 2);
    return qMax(1, static_cast<int>(range / count));
}

// === 绘制 ===

void FluentSlider::paintFluent(QPainter* painter) {
    auto* theme = FluentThemeManager::instance().currentTheme();

    // 标签
    if (!m_label.isEmpty()) {
        QFont labelFont = theme->captionFont();
        painter->setFont(labelFont);
        painter->setPen(theme->textColorSecondary());
        if (m_orientation == Qt::Horizontal) {
            painter->drawText(QRectF(0, 0, width(), height() / 2 - 10),
                              Qt::AlignLeft | Qt::AlignBottom, m_label);
        } else {
            painter->save();
            painter->translate(width() / 2 - 6, height());
            painter->rotate(-90);
            painter->drawText(QRectF(0, -14, height(), 14),
                              Qt::AlignLeft | Qt::AlignVCenter, m_label);
            painter->restore();
        }
    }

    // 刻度
    if (m_showTicks && m_tickPosition != NoTicks) {
        QPen tickPen(theme->dividerColor(), 1);
        painter->setPen(tickPen);

        int interval = effectiveTickInterval();
        QRectF tr = trackRect();

        for (qreal v = m_minimum; v <= m_maximum + 0.01; v += interval) {
            qreal pos = valueToPos(v);

            if (m_orientation == Qt::Horizontal) {
                if (m_tickPosition == TicksAbove || m_tickPosition == TicksBothSides) {
                    painter->drawLine(QPointF(pos, tr.top() - TICK_LENGTH), QPointF(pos, tr.top()));
                }
                if (m_tickPosition == TicksBelow || m_tickPosition == TicksBothSides) {
                    painter->drawLine(QPointF(pos, tr.bottom()), QPointF(pos, tr.bottom() + TICK_LENGTH));
                }
            } else {
                if (m_tickPosition == TicksAbove || m_tickPosition == TicksBothSides) {
                    painter->drawLine(QPointF(tr.left() - TICK_LENGTH, pos), QPointF(tr.left(), pos));
                }
                if (m_tickPosition == TicksBelow || m_tickPosition == TicksBothSides) {
                    painter->drawLine(QPointF(tr.right(), pos), QPointF(tr.right() + TICK_LENGTH, pos));
                }
            }
        }
    }

    // 轨道背景
    QRectF tr = trackRect();
    painter->setPen(Qt::NoPen);
    painter->setBrush(theme->dividerColor());
    painter->drawRoundedRect(tr, tr.height() / 2.0, tr.height() / 2.0);

    // 活跃轨道
    QRectF activeTr = activeTrackRect();
    painter->setBrush(theme->primaryColor());
    painter->drawRoundedRect(activeTr, tr.height() / 2.0, tr.height() / 2.0);

    // 滑块
    auto drawThumb = [&](const QRectF& thumbRect, bool isHover, bool isPressed, bool isDrag) {
        // 悬停时轨道区域变粗
        if (isHover || isDrag) {
            QRectF hoverTrack = tr;
            qreal expand = (TRACK_HOVER_HEIGHT - TRACK_HEIGHT) / 2.0;
            hoverTrack.adjust(0, -expand, 0, expand);
            painter->setPen(Qt::NoPen);
            painter->setBrush(theme->dividerColor());
            painter->drawRoundedRect(hoverTrack, hoverTrack.height() / 2.0, hoverTrack.height() / 2.0);

            // 活跃轨道也变粗
            QRectF hoverActive = activeTrackRect();
            hoverActive.adjust(0, -expand, 0, expand);
            painter->setBrush(theme->primaryColor());
            painter->drawRoundedRect(hoverActive, hoverActive.height() / 2.0, hoverActive.height() / 2.0);
        }

        // 缩放动画
        qreal size = THUMB_SIZE;
        if (isDrag) {
            size = THUMB_SIZE * 1.15;
        } else if (isHover) {
            size = THUMB_SIZE * 1.05;
        }
        QPointF center = thumbRect.center();
        QRectF drawRect(center.x() - size / 2.0, center.y() - size / 2.0, size, size);

        // 阴影
        if (isHover || isDrag) {
            QColor shadowColor = theme->primaryColor();
            shadowColor.setAlpha(50);
            painter->setBrush(shadowColor);
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(drawRect.adjusted(-2, -2, 2, 2));
        }

        // 白色底圆
        painter->setBrush(theme->backgroundColor());
        painter->setPen(QPen(theme->primaryColor(), 2));
        painter->drawEllipse(drawRect);

        // 内部实心小圆
        qreal innerSize = isDrag ? THUMB_SMALL + 2 : THUMB_SMALL;
        QRectF innerRect(center.x() - innerSize / 2.0, center.y() - innerSize / 2.0,
                         innerSize, innerSize);
        painter->setBrush(theme->primaryColor());
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(innerRect);
    };

    bool firstHover = (m_hoverTarget == FirstThumb);
    bool firstDrag = (m_dragTarget == FirstThumb);
    drawThumb(firstThumbRect(), firstHover, firstDrag, firstDrag);

    if (m_rangeMode) {
        bool secondHover = (m_hoverTarget == SecondThumb);
        bool secondDrag = (m_dragTarget == SecondThumb);
        drawThumb(secondThumbRect(), secondHover, secondDrag, secondDrag);
    }

    // 值标签气泡
    if (m_showValueLabel && (m_hoverTarget != None || m_dragTarget != None || hasFocus())) {
        QRectF labelRect = valueLabelRect();
        QString valText;

        if (m_rangeMode) {
            // 显示当前拖拽的那个值
            if (m_dragTarget == SecondThumb || m_hoverTarget == SecondThumb) {
                valText = QString::number(m_secondValue, 'f', qMax(0, -static_cast<int>(qFloor(qLn(m_singleStep) / qLn(10)))));
            } else {
                valText = QString::number(m_value, 'f', qMax(0, -static_cast<int>(qFloor(qLn(m_singleStep) / qLn(10)))));
            }
        } else {
            int decimals = m_singleStep < 1 ? 2 : (m_singleStep < 10 ? 1 : 0);
            valText = QString::number(m_value, 'f', decimals);
        }

        // 气泡背景
        painter->setPen(Qt::NoPen);
        painter->setBrush(theme->cardColor());
        painter->drawRoundedRect(labelRect, 6, 6);

        // 气泡边框
        painter->setPen(QPen(theme->dividerColor(), 1));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(labelRect, 6, 6);

        // 小三角（底部朝下）
        QPointF triCenter = labelRect.center();
        if (m_orientation == Qt::Horizontal) {
            QPolygonF triangle;
            triangle << QPointF(triCenter.x() - 4, labelRect.bottom())
                     << QPointF(triCenter.x() + 4, labelRect.bottom())
                     << QPointF(triCenter.x(), labelRect.bottom() + 5);
            painter->setBrush(theme->cardColor());
            painter->setPen(QPen(theme->dividerColor(), 1));
            painter->drawPolygon(triangle);
        }

        // 文本
        painter->setFont(theme->captionFont());
        painter->setPen(theme->textColorPrimary());
        painter->drawText(labelRect, Qt::AlignCenter, valText);
    }
}

// === 事件 ===

void FluentSlider::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    QPointF pos = event->pos();

    // 检查是否点击了滑块
    QRectF firstR = firstThumbRect().adjusted(-4, -4, 4, 4);
    if (firstR.contains(pos)) {
        m_dragTarget = FirstThumb;
        emit sliderPressed();
        update();
        return;
    }

    if (m_rangeMode) {
        QRectF secondR = secondThumbRect().adjusted(-4, -4, 4, 4);
        if (secondR.contains(pos)) {
            m_dragTarget = SecondThumb;
            emit sliderPressed();
            update();
            return;
        }
    }

    // 点击轨道
    qreal val = posToValue(m_orientation == Qt::Horizontal ? pos.x() : pos.y());

    if (m_rangeMode) {
        // 范围模式：点击离哪个滑块近就移动哪个
        qreal d1 = qAbs(val - m_value);
        qreal d2 = qAbs(val - m_secondValue);
        if (d1 <= d2) {
            m_dragTarget = FirstThumb;
            setValue(val);
        } else {
            m_dragTarget = SecondThumb;
            setSecondValue(val);
        }
    } else {
        m_dragTarget = Track;
        setValue(val);
    }
    emit sliderPressed();
    update();
}

void FluentSlider::mouseMoveEvent(QMouseEvent* event) {
    QPointF pos = event->pos();

    if (m_dragTarget != None) {
        qreal val = posToValue(m_orientation == Qt::Horizontal ? pos.x() : pos.y());

        // 对齐步进
        if (m_singleStep > 0) {
            val = m_minimum + qRound((val - m_minimum) / m_singleStep) * m_singleStep;
        }
        val = clampedValue(val);

        if (m_dragTarget == FirstThumb) {
            if (m_rangeMode) {
                // 确保不超过 secondValue
                val = qMin(val, m_secondValue);
            }
            setValue(val);
        } else if (m_dragTarget == SecondThumb) {
            val = qMax(val, m_value);
            setSecondValue(val);
        } else if (m_dragTarget == Track) {
            setValue(val);
        }
        return;
    }

    // 悬停检测
    DragTarget newHover = None;
    QRectF firstR = firstThumbRect().adjusted(-4, -4, 4, 4);
    if (firstR.contains(pos)) {
        newHover = FirstThumb;
    } else if (m_rangeMode) {
        QRectF secondR = secondThumbRect().adjusted(-4, -4, 4, 4);
        if (secondR.contains(pos)) {
            newHover = SecondThumb;
        }
    }

    if (newHover != m_hoverTarget) {
        m_hoverTarget = newHover;
        setCursor(newHover != None ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
    }
}

void FluentSlider::mouseReleaseEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    if (m_dragTarget != None) {
        emit sliderReleased();
        m_dragTarget = None;
        update();
    }
}

void FluentSlider::wheelEvent(QWheelEvent* event) {
    qreal delta = event->angleDelta().y() > 0 ? m_singleStep : -m_singleStep;

    // 范围模式下调整离光标近的滑块
    if (m_rangeMode) {
        QPointF pos = event->pos();
        qreal val = posToValue(m_orientation == Qt::Horizontal ? pos.x() : pos.y());
        qreal d1 = qAbs(val - m_value);
        qreal d2 = qAbs(val - m_secondValue);
        if (d1 <= d2) {
            setValue(clampedValue(m_value + delta));
        } else {
            setSecondValue(clampedValue(m_secondValue + delta));
        }
    } else {
        setValue(clampedValue(m_value + delta));
    }
    event->accept();
}

void FluentSlider::keyPressEvent(QKeyEvent* event) {
    qreal step = m_singleStep;
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Down:
        setValue(clampedValue(m_value - step));
        break;
    case Qt::Key_Right:
    case Qt::Key_Up:
        setValue(clampedValue(m_value + step));
        break;
    case Qt::Key_PageDown:
        setValue(clampedValue(m_value - m_pageStep));
        break;
    case Qt::Key_PageUp:
        setValue(clampedValue(m_value + m_pageStep));
        break;
    case Qt::Key_Home:
        setValue(m_minimum);
        break;
    case Qt::Key_End:
        setValue(m_maximum);
        break;
    default:
        FluentWidget::keyPressEvent(event);
    }
}

void FluentSlider::enterEvent(QEvent* /*event*/) {
    update();
}

void FluentSlider::leaveEvent(QEvent* /*event*/) {
    m_hoverTarget = None;
    setCursor(Qt::ArrowCursor);
    update();
}

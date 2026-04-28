#include "FluentToggleSwitch.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QKeyEvent>

FluentToggleSwitch::FluentToggleSwitch(const QString& text, QWidget* parent)
    : FluentWidget(parent)
    , m_text(text)
{
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::StrongFocus);

    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

QString FluentToggleSwitch::text() const { return m_text; }
void FluentToggleSwitch::setText(const QString& text) {
    if (m_text != text) { m_text = text; update(); emit textChanged(); }
}

bool FluentToggleSwitch::checked() const { return m_checked; }
void FluentToggleSwitch::setChecked(bool checked) {
    if (m_checked == checked) return;
    m_checked = checked;

    qreal target = checked ? 1.0 : 0.0;
    auto* anim = new QPropertyAnimation(this, "toggleProgress");
    anim->setStartValue(m_toggleProgress);
    anim->setEndValue(target);
    anim->setDuration(200);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &QAbstractAnimation::finished, anim, &QObject::deleteLater);
    anim->start();

    update();
    emit checkedChanged(checked);
}

qreal FluentToggleSwitch::toggleProgress() const { return m_toggleProgress; }
void FluentToggleSwitch::setToggleProgress(qreal progress) {
    if (qFuzzyCompare(m_toggleProgress, progress)) return;
    m_toggleProgress = progress;
    update();
    emit toggleProgressChanged();
}

QSize FluentToggleSwitch::sizeHint() const {
    QFontMetrics fm(theme()->bodyFont());
    int w = TRACK_WIDTH + (m_text.isEmpty() ? 0 : TEXT_SPACING + fm.horizontalAdvance(m_text));
    return QSize(w, qMax(int(TRACK_HEIGHT), fm.height() + 4));
}

QSize FluentToggleSwitch::minimumSizeHint() const {
    return QSize(TRACK_WIDTH, TRACK_HEIGHT);
}

QRect FluentToggleSwitch::trackRect() const {
    int y = (height() - TRACK_HEIGHT) / 2;
    return QRect(0, y, TRACK_WIDTH, TRACK_HEIGHT);
}

qreal FluentToggleSwitch::thumbX() const {
    QRect track = trackRect();
    // thumb 从左侧移到右侧
    qreal minThumbX = track.left() + THUMB_MARGIN;
    qreal maxThumbX = track.right() - THUMB_MARGIN - THUMB_SIZE;
    return minThumbX + (maxThumbX - minThumbX) * m_toggleProgress;
}

void FluentToggleSwitch::paintFluent(QPainter* painter) {
    auto* t = theme();
    QRect track = trackRect();
    int trackRadius = TRACK_HEIGHT / 2;

    // 1. 轨道背景 — 在 off/on 之间插值颜色
    QColor offColor = isHovered() ? t->surfaceColor().darker(110) : t->surfaceColor();
    QColor onColor = t->primaryColor();

    QColor trackColor;
    trackColor.setRedF(offColor.redF() + (onColor.redF() - offColor.redF()) * m_toggleProgress);
    trackColor.setGreenF(offColor.greenF() + (onColor.greenF() - offColor.greenF()) * m_toggleProgress);
    trackColor.setBlueF(offColor.blueF() + (onColor.blueF() - offColor.blueF()) * m_toggleProgress);

    if (!isEnabled()) {
        trackColor = t->surfaceColor();
    }

    QPainterPath trackPath;
    trackPath.addRoundedRect(track, trackRadius, trackRadius);
    painter->setPen(Qt::NoPen);
    painter->setBrush(trackColor);
    painter->drawPath(trackPath);

    // 轨道边框 (off 状态)
    if (m_toggleProgress < 1.0) {
        QColor borderColor = t->borderColor();
        borderColor.setAlphaF(1.0 - m_toggleProgress);
        painter->setPen(QPen(borderColor, t->borderWidth()));
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(trackPath);
    }

    // 2. 滑块 (Thumb)
    qreal tx = thumbX();
    int ty = track.center().y() - THUMB_SIZE / 2;
    QRectF thumbRect(tx, ty, THUMB_SIZE, THUMB_SIZE);

    // 滑块颜色：off=白色，on=白色
    QColor thumbColor = Qt::white;
    if (!isEnabled()) {
        thumbColor = t->textColorDisabled();
    }

    painter->setPen(Qt::NoPen);
    painter->setBrush(thumbColor);
    painter->drawEllipse(thumbRect);

    // 滑块阴影
    QColor shadowColor(0, 0, 0, 30);
    painter->setPen(Qt::NoPen);
    painter->setBrush(shadowColor);
    QRectF shadowRect(thumbRect.adjusted(0, 1, 0, 2));
    painter->drawEllipse(shadowRect);
    // 重绘滑块在阴影上面
    painter->setBrush(thumbColor);
    painter->drawEllipse(thumbRect);

    // 3. 焦点边框
    if (hasFocus()) {
        QPainterPath focusPath;
        focusPath.addRoundedRect(track.adjusted(-2, -2, 2, 2), trackRadius + 1, trackRadius + 1);
        QColor focusColor = t->primaryColor();
        focusColor.setAlphaF(0.3);
        painter->setPen(QPen(focusColor, 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(focusPath);
    }

    // 4. 文字
    if (!m_text.isEmpty()) {
        painter->setPen(isEnabled() ? t->textColorPrimary() : t->textColorDisabled());
        painter->setFont(t->bodyFont());
        int textX = track.right() + TEXT_SPACING;
        QRect textRect(textX, 0, width() - textX, height());
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, m_text);
    }
}

void FluentToggleSwitch::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // 消费事件
    }
}

void FluentToggleSwitch::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && rect().contains(event->pos())) {
        setChecked(!m_checked);
    }
}

void FluentToggleSwitch::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space) {
        setChecked(!m_checked);
    }
}

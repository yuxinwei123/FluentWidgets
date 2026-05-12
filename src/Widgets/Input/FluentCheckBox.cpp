#include "FluentCheckBox.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QKeyEvent>

FluentCheckBox::FluentCheckBox(const QString& text, QWidget* parent)
    : FluentWidget(parent)
    , m_text(text)
{
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::StrongFocus);

    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

QString FluentCheckBox::text() const { return m_text; }
void FluentCheckBox::setText(const QString& text) {
    if (m_text != text) { m_text = text; update(); emit textChanged(); }
}

bool FluentCheckBox::checked() const { return m_state != Qt::Unchecked; }
void FluentCheckBox::setChecked(bool checked) {
    setCheckState(checked ? Qt::Checked : Qt::Unchecked);
}

bool FluentCheckBox::tristate() const { return m_tristate; }
void FluentCheckBox::setTristate(bool enabled) { m_tristate = enabled; }

Qt::CheckState FluentCheckBox::checkState() const { return m_state; }
void FluentCheckBox::setCheckState(Qt::CheckState state) {
    if (m_state == state) return;

    m_state = state;

    // 动画
    qreal targetProgress = 0.0;
    if (state == Qt::Checked) targetProgress = 1.0;
    else if (state == Qt::PartiallyChecked) targetProgress = 0.5;

    auto* anim = new QPropertyAnimation(this, "checkProgress");
    anim->setStartValue(m_checkProgress);
    anim->setEndValue(targetProgress);
    anim->setDuration(150);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &QAbstractAnimation::finished, anim, &QObject::deleteLater);
    anim->start();

    update();
    emit checkedChanged(state != Qt::Unchecked);
    emit checkStateChanged(state);
}

qreal FluentCheckBox::checkProgress() const { return m_checkProgress; }
void FluentCheckBox::setCheckProgress(qreal progress) {
    if (qFuzzyCompare(m_checkProgress, progress)) return;
    m_checkProgress = progress;
    update();
    emit checkProgressChanged();
}

QSize FluentCheckBox::sizeHint() const {
    QFontMetrics fm(theme()->bodyFont());
    int w = BOX_SIZE + TEXT_SPACING + fm.horizontalAdvance(m_text);
    return QSize(w, qMax(int(BOX_SIZE + BOX_MARGIN * 2), fm.height() + 4));
}

QSize FluentCheckBox::minimumSizeHint() const {
    return QSize(BOX_SIZE + TEXT_SPACING, BOX_SIZE + BOX_MARGIN * 2);
}

QRect FluentCheckBox::boxRect() const {
    int y = (height() - BOX_SIZE) / 2;
    return QRect(BOX_MARGIN, y, BOX_SIZE, BOX_SIZE);
}

void FluentCheckBox::paintFluent(QPainter* painter) {
    auto* t = theme();
    const int radius = t->cornerRadiusSmall();
    QRect box = boxRect();

    // 1. 复选框背景
    QPainterPath boxPath;
    boxPath.addRoundedRect(box, radius, radius);

    if (m_checkProgress > 0.0) {
        // 选中/部分选中：填充强调色
        QColor fillColor = t->primaryColor();
        if (m_state == Qt::PartiallyChecked) {
            fillColor.setAlphaF(m_checkProgress * 2); // 0.5 → 1.0 alpha
        }
        painter->setPen(Qt::NoPen);
        painter->setBrush(fillColor);
        painter->drawPath(boxPath);

        // 2. 勾选标记
        if (m_state == Qt::Checked) {
            // ✓ 勾号
            QPen checkPen(Qt::white, 2.0);
            checkPen.setCapStyle(Qt::RoundCap);
            checkPen.setJoinStyle(Qt::RoundJoin);
            painter->setPen(checkPen);

            int cx = box.center().x();
            int cy = box.center().y();
            // 缩放动画
            qreal scale = m_checkProgress;
            painter->save();
            painter->translate(cx, cy);
            painter->scale(scale, scale);
            painter->translate(-cx, -cy);

            // 第一段: 左下
            painter->drawLine(box.left() + 4, box.center().y() + 1,
                              box.center().x() - 1, box.bottom() - 5);
            // 第二段: 右上
            painter->drawLine(box.center().x() - 1, box.bottom() - 5,
                              box.right() - 4, box.top() + 4);
            painter->restore();
        } else if (m_state == Qt::PartiallyChecked) {
            // — 横线（部分选中）
            painter->setPen(Qt::NoPen);
            painter->setBrush(Qt::white);
            QRect lineRect(box.left() + 4, box.center().y() - 1, box.width() - 8, 2);
            painter->drawRoundedRect(lineRect, 1, 1);
        }
    } else {
        // 未选中：边框
        QColor borderColor = hasFocus() ? t->primaryColor() :
                              isHovered() ? t->textColorSecondary() : t->borderColor();
        painter->setPen(QPen(borderColor, t->borderWidth()));
        painter->setBrush(t->cardColor());
        painter->drawPath(boxPath);
    }

    // 3. 焦点边框
    if (hasFocus()) {
        QPainterPath focusPath;
        focusPath.addRoundedRect(box.adjusted(-2, -2, 2, 2), radius + 1, radius + 1);
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
        int textX = box.right() + TEXT_SPACING;
        QRect textRect(textX, 0, width() - textX, height());
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, m_text);
    }
}

void FluentCheckBox::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // 只消费事件，不调用 FluentWidget::mousePressEvent（避免 pressed 状态影响外观）
    }
}

void FluentCheckBox::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && rect().contains(event->pos())) {
        toggle();
    }
}

void FluentCheckBox::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space) {
        toggle();
    }
}

void FluentCheckBox::toggle() {
    if (m_tristate) {
        switch (m_state) {
        case Qt::Unchecked:      setCheckState(Qt::Checked); break;
        case Qt::Checked:        setCheckState(Qt::PartiallyChecked); break;
        case Qt::PartiallyChecked: setCheckState(Qt::Unchecked); break;
        }
    } else {
        setCheckState(m_state == Qt::Checked ? Qt::Unchecked : Qt::Checked);
    }
}

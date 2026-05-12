#include "FluentNavBar.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QFontMetrics>

// ============================================================
// FluentNavBarItem
// ============================================================

FluentNavBarItem::FluentNavBarItem(const QString& key, const QString& text, const QIcon& icon)
    : m_key(key), m_text(text), m_icon(icon)
{
}

QString FluentNavBarItem::key() const { return m_key; }
QString FluentNavBarItem::text() const { return m_text; }
QIcon FluentNavBarItem::icon() const { return m_icon; }
bool FluentNavBarItem::isEnabled() const { return m_enabled; }
void FluentNavBarItem::setEnabled(bool enabled) { m_enabled = enabled; }

// ============================================================
// FluentNavBar
// ============================================================

FluentNavBar::FluentNavBar(QWidget* parent)
    : FluentWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

FluentNavBar::~FluentNavBar() = default;

// ============================================================
// 项目操作
// ============================================================

void FluentNavBar::addItem(const QString& key, const QString& text, const QIcon& icon) {
    m_items.append(FluentNavBarItem(key, text, icon));
    if (m_currentKey.isEmpty()) {
        m_currentKey = key;
        updateIndicatorTarget();
    }
    update();
}

void FluentNavBar::removeItem(const QString& key) {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].key() == key) {
            m_items.removeAt(i);
            if (m_currentKey == key) {
                m_currentKey = m_items.isEmpty() ? QString() : m_items.first().key();
                updateIndicatorTarget();
            }
            break;
        }
    }
    update();
}

int FluentNavBar::itemCount() const { return m_items.size(); }

// ============================================================
// 当前选中
// ============================================================

QString FluentNavBar::currentKey() const { return m_currentKey; }

void FluentNavBar::setCurrentKey(const QString& key) {
    if (m_currentKey == key) return;
    m_currentKey = key;
    updateIndicatorTarget();
    emit currentChanged(key);
    update();
}

int FluentNavBar::currentIndex() const {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].key() == m_currentKey) return i;
    }
    return -1;
}

// ============================================================
// 样式/方向
// ============================================================

FluentNavBar::NavStyle FluentNavBar::navStyle() const { return m_navStyle; }
void FluentNavBar::setNavStyle(NavStyle style) {
    if (m_navStyle == style) return;
    m_navStyle = style;
    updateIndicatorTarget();
    update();
    emit navStyleChanged();
}

FluentNavBar::Orientation FluentNavBar::orientation() const { return m_orientation; }
void FluentNavBar::setOrientation(Orientation orient) {
    if (m_orientation == orient) return;
    m_orientation = orient;
    updateIndicatorTarget();
    update();
    emit orientationChanged();
}

// ============================================================
// 尺寸
// ============================================================

QSize FluentNavBar::sizeHint() const {
    if (m_orientation == Orientation::Horizontal) {
        int w = 0;
        for (int i = 0; i < m_items.size(); ++i) {
            w += itemRect(i).width();
        }
        return QSize(qMax(w, 200), ITEM_HEIGHT);
    } else {
        int maxW = 0;
        for (int i = 0; i < m_items.size(); ++i) {
            maxW = qMax(maxW, itemRect(i).width());
        }
        return QSize(maxW, m_items.size() * ITEM_HEIGHT);
    }
}

QSize FluentNavBar::minimumSizeHint() const { return QSize(100, ITEM_HEIGHT); }

// ============================================================
// 布局计算
// ============================================================

QRect FluentNavBar::itemRect(int index) const {
    if (index < 0 || index >= m_items.size()) return QRect();

    auto* t = theme();
    QFontMetrics fm(t->bodyFont());
    QFontMetrics fmBold(t->bodyStrongFont());

    if (m_orientation == Orientation::Horizontal) {
        int x = 0;
        for (int i = 0; i < index; ++i) {
            int textW = qMax(fm.horizontalAdvance(m_items[i].text()),
                             fmBold.horizontalAdvance(m_items[i].text()));
            int itemW = textW + ITEM_H_PADDING * 2;
            if (!m_items[i].icon().isNull()) itemW += ICON_SIZE + ICON_TEXT_GAP;
            if (m_navStyle == NavStyle::Breadcrumb && i > 0) itemW += BREADCRUMB_GAP;
            x += itemW;
        }

        int textW = qMax(fm.horizontalAdvance(m_items[index].text()),
                         fmBold.horizontalAdvance(m_items[index].text()));
        int itemW = textW + ITEM_H_PADDING * 2;
        if (!m_items[index].icon().isNull()) itemW += ICON_SIZE + ICON_TEXT_GAP;
        if (m_navStyle == NavStyle::Breadcrumb && index > 0) {
            x += BREADCRUMB_GAP;
        }

        return QRect(x, 0, itemW, ITEM_HEIGHT);
    } else {
        // 垂直布局
        int y = index * ITEM_HEIGHT;
        int maxW = width();
        int textW = qMax(fm.horizontalAdvance(m_items[index].text()),
                         fmBold.horizontalAdvance(m_items[index].text()));
        int needW = textW + ITEM_H_PADDING * 2;
        if (!m_items[index].icon().isNull()) needW += ICON_SIZE + ICON_TEXT_GAP;
        return QRect(0, y, qMax(maxW, needW), ITEM_HEIGHT);
    }
}

int FluentNavBar::itemAtPos(const QPoint& pos) const {
    for (int i = 0; i < m_items.size(); ++i) {
        if (itemRect(i).contains(pos) && m_items[i].isEnabled()) return i;
    }
    return -1;
}

int FluentNavBar::indicatorX() const { return int(m_indicatorX); }
int FluentNavBar::indicatorWidth() const { return int(m_indicatorW); }

void FluentNavBar::updateIndicatorTarget() {
    int idx = currentIndex();
    if (idx < 0) return;

    QRect rect = itemRect(idx);

    // 保存当前指示器位置为起始
    m_indicatorFromX = int(m_indicatorX);
    m_indicatorFromW = int(m_indicatorW);

    if (m_navStyle == NavStyle::Tab) {
        // 指示器在文字下方，宽度稍短
        auto* t = theme();
        QFontMetrics fm(t->bodyFont());
        int textW = fm.horizontalAdvance(m_items[idx].text());
        int iconExtra = m_items[idx].icon().isNull() ? 0 : ICON_SIZE + ICON_TEXT_GAP;
        int textStartX = rect.x() + ITEM_H_PADDING + iconExtra;
        m_indicatorToX = textStartX;
        m_indicatorToW = textW;
    } else {
        m_indicatorToX = rect.x();
        m_indicatorToW = rect.width();
    }

    // 如果是第一次初始化，直接跳到目标位置
    if (m_indicatorX == 0 && m_indicatorW == 0) {
        m_indicatorX = m_indicatorToX;
        m_indicatorW = m_indicatorToW;
        m_indicatorFromX = m_indicatorToX;
        m_indicatorFromW = m_indicatorToW;
        return;
    }

    // 启动动画
    m_indicatorProgress = 0.0;
    auto* anim = new QPropertyAnimation(this, "indicatorProgress");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(200);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &QAbstractAnimation::finished, anim, &QObject::deleteLater);
    anim->start();
}

// ============================================================
// 指示器动画
// ============================================================

qreal FluentNavBar::indicatorProgress() const { return m_indicatorProgress; }

void FluentNavBar::setIndicatorProgress(qreal progress) {
    if (qFuzzyCompare(m_indicatorProgress, progress)) return;
    m_indicatorProgress = progress;

    // 线性插值
    qreal eased = QEasingCurve(QEasingCurve::OutCubic).valueForProgress(progress);
    m_indicatorX = m_indicatorFromX + (m_indicatorToX - m_indicatorFromX) * eased;
    m_indicatorW = m_indicatorFromW + (m_indicatorToW - m_indicatorFromW) * eased;

    update();
    emit indicatorProgressChanged();
}

// ============================================================
// 绘制
// ============================================================

void FluentNavBar::paintFluent(QPainter* painter) {
    auto* t = theme();

    // 背景
    painter->fillRect(rect(), t->cardColor());

    // 底部分隔线
    painter->setPen(QPen(t->dividerColor(), 1));
    painter->drawLine(0, height() - 1, width(), height() - 1);

    // 绘制各项
    for (int i = 0; i < m_items.size(); ++i) {
        QRect rect = itemRect(i);
        bool isSelected = (m_items[i].key() == m_currentKey);
        bool isHovered = (m_hoveredIndex == i);
        bool isEnabled = m_items[i].isEnabled();

        if (m_navStyle == NavStyle::Tab) {
            // --- Tab 样式 ---
            // 悬停背景
            if (isHovered && !isSelected && isEnabled) {
                QColor hoverColor = t->textColorPrimary();
                hoverColor.setAlphaF(0.04);
                painter->fillRect(rect, hoverColor);
            }
            // 选中背景
            if (isSelected) {
                QColor selColor = t->primaryColor();
                selColor.setAlphaF(0.06);
                painter->fillRect(rect, selColor);
            }

            // 图标
            int textX = rect.x() + ITEM_H_PADDING;
            if (!m_items[i].icon().isNull()) {
                QRect iconRect(textX, rect.y() + (ITEM_HEIGHT - ICON_SIZE) / 2, ICON_SIZE, ICON_SIZE);
                m_items[i].icon().paint(painter, iconRect);
                textX += ICON_SIZE + ICON_TEXT_GAP;
            }

            // 文字
            if (isSelected) {
                painter->setPen(t->primaryColor());
                painter->setFont(t->bodyStrongFont());
            } else if (!isEnabled) {
                painter->setPen(t->textColorDisabled());
                painter->setFont(t->bodyFont());
            } else {
                painter->setPen(t->textColorPrimary());
                painter->setFont(t->bodyFont());
            }
            QRect textRect(textX, rect.y(), rect.width() - (textX - rect.x()) - ITEM_H_PADDING, ITEM_HEIGHT);
            painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, m_items[i].text());

            // 选中指示线
            if (isSelected) {
                int ix = int(m_indicatorX);
                int iw = qMax(int(m_indicatorW), 16);
                int iy = rect.bottom() - INDICATOR_HEIGHT;
                painter->setPen(Qt::NoPen);
                painter->setBrush(t->primaryColor());
                painter->drawRoundedRect(QRect(ix, iy, iw, INDICATOR_HEIGHT), INDICATOR_HEIGHT / 2, INDICATOR_HEIGHT / 2);
            }

        } else if (m_navStyle == NavStyle::Pill) {
            // --- Pill 样式 ---
            if (isSelected) {
                // 选中项填充背景
                QPainterPath pillPath;
                pillPath.addRoundedRect(rect.adjusted(2, 4, -2, -4), PILL_RADIUS, PILL_RADIUS);
                painter->setPen(Qt::NoPen);
                painter->setBrush(t->primaryColor());
                painter->drawPath(pillPath);

                painter->setPen(Qt::white);
                painter->setFont(t->bodyStrongFont());
            } else if (isHovered && isEnabled) {
                QColor hoverColor = t->textColorPrimary();
                hoverColor.setAlphaF(0.06);
                QPainterPath pillPath;
                pillPath.addRoundedRect(rect.adjusted(2, 4, -2, -4), PILL_RADIUS, PILL_RADIUS);
                painter->setPen(Qt::NoPen);
                painter->setBrush(hoverColor);
                painter->drawPath(pillPath);

                painter->setPen(t->textColorPrimary());
                painter->setFont(t->bodyFont());
            } else {
                painter->setPen(isEnabled ? t->textColorPrimary() : t->textColorDisabled());
                painter->setFont(t->bodyFont());
            }

            int textX = rect.x() + ITEM_H_PADDING;
            if (!m_items[i].icon().isNull()) {
                QRect iconRect(textX, rect.y() + (ITEM_HEIGHT - ICON_SIZE) / 2, ICON_SIZE, ICON_SIZE);
                m_items[i].icon().paint(painter, iconRect);
                textX += ICON_SIZE + ICON_TEXT_GAP;
            }
            QRect textRect(textX, rect.y(), rect.width() - (textX - rect.x()) - ITEM_H_PADDING, ITEM_HEIGHT);
            painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, m_items[i].text());

        } else {
            // --- Breadcrumb 样式 ---
            // 分隔符 ">"
            if (i > 0) {
                int sepX = rect.x() - BREADCRUMB_GAP / 2;
                painter->setPen(t->textColorSecondary());
                painter->setFont(t->bodyFont());
                painter->drawText(QRect(sepX - 4, rect.y(), 12, ITEM_HEIGHT),
                                  Qt::AlignVCenter | Qt::AlignCenter, u8"›");
            }

            if (isSelected) {
                painter->setPen(t->primaryColor());
                painter->setFont(t->bodyStrongFont());
            } else if (isHovered && isEnabled) {
                painter->setPen(t->primaryColor());
                painter->setFont(t->bodyFont());
            } else if (!isEnabled) {
                painter->setPen(t->textColorDisabled());
                painter->setFont(t->bodyFont());
            } else {
                painter->setPen(t->textColorSecondary());
                painter->setFont(t->bodyFont());
            }

            int textX = rect.x() + ITEM_H_PADDING;
            if (!m_items[i].icon().isNull()) {
                QRect iconRect(textX, rect.y() + (ITEM_HEIGHT - ICON_SIZE) / 2, ICON_SIZE, ICON_SIZE);
                m_items[i].icon().paint(painter, iconRect);
                textX += ICON_SIZE + ICON_TEXT_GAP;
            }
            QRect textRect(textX, rect.y(), rect.width() - (textX - rect.x()) - ITEM_H_PADDING, ITEM_HEIGHT);
            painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, m_items[i].text());
        }
    }
}

// ============================================================
// 事件处理
// ============================================================

void FluentNavBar::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;
    // 消费事件
}

void FluentNavBar::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    int idx = itemAtPos(event->pos());
    if (idx >= 0) {
        setCurrentKey(m_items[idx].key());
        emit itemClicked(m_items[idx].key());
    }
}

void FluentNavBar::mouseMoveEvent(QMouseEvent* event) {
    int oldHover = m_hoveredIndex;
    m_hoveredIndex = itemAtPos(event->pos());
    if (oldHover != m_hoveredIndex) {
        setCursor(m_hoveredIndex >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
    }
}

void FluentNavBar::leaveEvent(QEvent* event) {
    m_hoveredIndex = -1;
    setCursor(Qt::ArrowCursor);
    update();
    FluentWidget::leaveEvent(event);
}

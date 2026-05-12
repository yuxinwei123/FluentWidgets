#include "FluentCarousel.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QFontMetrics>
#include <QtMath>

// ============================================================
// FluentCarousel
// ============================================================

FluentCarousel::FluentCarousel(QWidget* parent)
    : FluentWidget(parent)
{
    setMouseTracking(true);
    setMinimumSize(minimumSizeHint());

    connect(theme(), &FluentTheme::themeChanged, this, [this]() { update(); });
}

FluentCarousel::~FluentCarousel() {
    stopAutoPlay();
    if (m_animTimerId) killTimer(m_animTimerId);
}

// ============================================================
// 属性
// ============================================================

int FluentCarousel::slideCount() const { return m_slides.size(); }

int FluentCarousel::currentIndex() const { return m_currentIndex; }

void FluentCarousel::setCurrentIndex(int index) {
    if (index < 0 || index >= m_slides.size()) return;
    if (m_currentIndex == index) return;
    m_currentIndex = index;
    update();
    emit currentChanged(index);
}

bool FluentCarousel::autoPlay() const { return m_autoPlay; }
void FluentCarousel::setAutoPlay(bool on) {
    if (m_autoPlay == on) return;
    m_autoPlay = on;
    if (on) startAutoPlay(); else stopAutoPlay();
    emit autoPlayChanged();
}

int FluentCarousel::interval() const { return m_interval; }
void FluentCarousel::setInterval(int ms) {
    if (m_interval == ms) return;
    m_interval = qMax(500, ms);
    if (m_autoPlay) { stopAutoPlay(); startAutoPlay(); }
    emit intervalChanged();
}

FluentCarousel::IndicatorPosition FluentCarousel::indicatorPosition() const { return m_indicatorPosition; }
void FluentCarousel::setIndicatorPosition(IndicatorPosition pos) { m_indicatorPosition = pos; update(); emit indicatorPositionChanged(); }

FluentCarousel::ArrowVisibility FluentCarousel::arrowVisibility() const { return m_arrowVisibility; }
void FluentCarousel::setArrowVisibility(ArrowVisibility vis) { m_arrowVisibility = vis; update(); emit arrowVisibilityChanged(); }

int FluentCarousel::borderRadius() const { return m_borderRadius; }
void FluentCarousel::setBorderRadius(int r) { m_borderRadius = r; update(); }

QSize FluentCarousel::sizeHint() const { return QSize(600, 300); }
QSize FluentCarousel::minimumSizeHint() const { return QSize(200, 120); }

// ============================================================
// 幻灯片操作
// ============================================================

void FluentCarousel::addSlide(const QPixmap& pixmap) {
    m_slides.append({pixmap, Qt::white, QString(), true});
    if (m_currentIndex < 0) { m_currentIndex = 0; emit currentChanged(0); }
    if (m_autoPlay && m_slides.size() == 2) startAutoPlay();
    update();
}

void FluentCarousel::addSlide(const QColor& color, const QString& text) {
    m_slides.append({QPixmap(), color, text, false});
    if (m_currentIndex < 0) { m_currentIndex = 0; emit currentChanged(0); }
    if (m_autoPlay && m_slides.size() == 2) startAutoPlay();
    update();
}

void FluentCarousel::removeSlide(int index) {
    if (index < 0 || index >= m_slides.size()) return;
    m_slides.removeAt(index);
    if (m_slides.isEmpty()) { m_currentIndex = -1; stopAutoPlay(); }
    else if (m_currentIndex >= m_slides.size()) m_currentIndex = m_slides.size() - 1;
    update();
}

void FluentCarousel::clearSlides() {
    m_slides.clear();
    m_currentIndex = -1;
    stopAutoPlay();
    update();
}

// ============================================================
// 导航
// ============================================================

void FluentCarousel::goTo(int index) {
    if (index < 0 || index >= m_slides.size()) return;
    if (index == m_currentIndex) return;

    // 启动滑动动画
    m_slideFrom = m_currentIndex;
    m_slideTo = index;
    m_slideProgress = 0.0;

    if (m_animTimerId) killTimer(m_animTimerId);
    m_animTimerId = startTimer(1000 / ANIM_FPS);

    m_currentIndex = index;
    emit currentChanged(index);

    // 重置自动播放计时
    if (m_autoPlay) { stopAutoPlay(); startAutoPlay(); }
}

void FluentCarousel::goToNext() {
    if (m_slides.size() <= 1) return;
    goTo((m_currentIndex + 1) % m_slides.size());
}

void FluentCarousel::goToPrev() {
    if (m_slides.size() <= 1) return;
    goTo((m_currentIndex - 1 + m_slides.size()) % m_slides.size());
}

void FluentCarousel::startAutoPlay() {
    if (m_autoPlayTimerId || !m_autoPlay || m_slides.size() <= 1) return;
    m_autoPlayTimerId = startTimer(m_interval);
}

void FluentCarousel::stopAutoPlay() {
    if (m_autoPlayTimerId) { killTimer(m_autoPlayTimerId); m_autoPlayTimerId = 0; }
}

// ============================================================
// 布局计算
// ============================================================

QRect FluentCarousel::slideRect() const {
    return rect();
}

QRect FluentCarousel::leftArrowRect() const {
    return QRect(ARROW_MARGIN, (height() - ARROW_SIZE) / 2, ARROW_SIZE, ARROW_SIZE);
}

QRect FluentCarousel::rightArrowRect() const {
    return QRect(width() - ARROW_MARGIN - ARROW_SIZE, (height() - ARROW_SIZE) / 2, ARROW_SIZE, ARROW_SIZE);
}

QRect FluentCarousel::indicatorRect() const {
    int totalW = m_slides.size() * DOT_SIZE + (m_slides.size() - 1) * DOT_GAP;
    int x = (width() - totalW) / 2;
    int y = (m_indicatorPosition == Bottom) ?
            height() - DOT_MARGIN_BOTTOM - DOT_SIZE :
            DOT_MARGIN_BOTTOM;
    return QRect(x, y, totalW, DOT_SIZE);
}

int FluentCarousel::indicatorDotX(int index) const {
    int totalW = m_slides.size() * DOT_SIZE + (m_slides.size() - 1) * DOT_GAP;
    int startX = (width() - totalW) / 2;
    return startX + index * (DOT_SIZE + DOT_GAP);
}

// ============================================================
// 绘制
// ============================================================

void FluentCarousel::paintFluent(QPainter* painter) {
    auto* t = theme();

    if (m_slides.isEmpty()) {
        // 空状态
        QPainterPath bgPath;
        bgPath.addRoundedRect(rect(), m_borderRadius, m_borderRadius);
        painter->setPen(Qt::NoPen);
        painter->setBrush(t->backgroundColor());
        painter->drawPath(bgPath);
        painter->setPen(t->textColorSecondary());
        painter->setFont(t->bodyFont());
        painter->drawText(rect(), Qt::AlignCenter, u8"添加幻灯片以开始轮播");
        return;
    }

    painter->setRenderHint(QPainter::Antialiasing);
    QRect sr = slideRect();

    // 裁剪圆角
    QPainterPath clipPath;
    clipPath.addRoundedRect(sr, m_borderRadius, m_borderRadius);
    painter->setClipPath(clipPath);

    // 绘制当前幻灯片
    auto drawSlide = [&](int index, int offsetX) {
        if (index < 0 || index >= m_slides.size()) return;
        const auto& slide = m_slides[index];

        painter->save();
        painter->translate(offsetX, 0);

        if (slide.isPixmap && !slide.pixmap.isNull()) {
            QPixmap scaled = slide.pixmap.scaled(sr.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            painter->drawPixmap(sr, scaled);
        } else {
            painter->fillRect(sr, slide.color);
            if (!slide.text.isEmpty()) {
                painter->setPen(Qt::white);
                painter->setFont(t->titleFont());
                painter->drawText(sr, Qt::AlignCenter, slide.text);
            }
        }
        painter->restore();
    };

    if (m_slideProgress < 1.0 && m_slideFrom >= 0 && m_slideTo >= 0) {
        // 滑动动画
        int offset = qRound((1.0 - m_slideProgress) * sr.width());
        bool slideRight = (m_slideTo > m_slideFrom) ||
                          (m_slideFrom == m_slides.size() - 1 && m_slideTo == 0);

        if (slideRight) {
            drawSlide(m_slideFrom, -offset);
            drawSlide(m_slideTo, sr.width() - offset);
        } else {
            drawSlide(m_slideFrom, offset);
            drawSlide(m_slideTo, -sr.width() + offset);
        }
    } else {
        drawSlide(m_currentIndex, 0);
    }

    painter->setClipping(false);

    // 半透明渐变遮罩（底部，让指示器更清晰）
    if (m_indicatorPosition != None) {
        QLinearGradient gradient(0, height() - 60, 0, height());
        gradient.setColorAt(0, QColor(0, 0, 0, 0));
        gradient.setColorAt(1, QColor(0, 0, 0, 80));
        painter->fillRect(QRect(0, height() - 60, width(), 60), gradient);
    }

    // 箭头
    bool showArrows = (m_arrowVisibility == Always) ||
                      (m_arrowVisibility == OnHover && m_hovered);
    if (showArrows && m_slides.size() > 1) {
        // 左箭头
        QRect leftR = leftArrowRect();
        QColor arrowBg(0, 0, 0, m_pressedLeft ? 100 : (leftR.contains(mapFromGlobal(QCursor::pos())) ? 70 : 40));
        painter->setPen(Qt::NoPen);
        painter->setBrush(arrowBg);
        painter->drawRoundedRect(leftR, leftR.width() / 2, leftR.height() / 2);
        painter->setPen(Qt::white);
        painter->setFont(t->bodyFont());
        painter->drawText(leftR, Qt::AlignCenter, u8"‹");

        // 右箭头
        QRect rightR = rightArrowRect();
        arrowBg = QColor(0, 0, 0, m_pressedRight ? 100 : (rightR.contains(mapFromGlobal(QCursor::pos())) ? 70 : 40));
        painter->setPen(Qt::NoPen);
        painter->setBrush(arrowBg);
        painter->drawRoundedRect(rightR, rightR.width() / 2, rightR.height() / 2);
        painter->setPen(Qt::white);
        painter->drawText(rightR, Qt::AlignCenter, u8"›");
    }

    // 指示器
    if (m_indicatorPosition != None && m_slides.size() > 1) {
        for (int i = 0; i < m_slides.size(); ++i) {
            int dotX = indicatorDotX(i);
            int dotY = (m_indicatorPosition == Bottom) ?
                       height() - DOT_MARGIN_BOTTOM - DOT_SIZE :
                       DOT_MARGIN_BOTTOM;
            QRect dotR(dotX, dotY, DOT_SIZE, DOT_SIZE);

            painter->setPen(Qt::NoPen);
            if (i == m_currentIndex) {
                painter->setBrush(Qt::white);
                painter->drawRoundedRect(dotR, DOT_SIZE / 2.0, DOT_SIZE / 2.0);
            } else {
                bool dotHovered = (m_hoveredIndicator == i);
                painter->setBrush(dotHovered ? QColor(255, 255, 255, 200) : QColor(255, 255, 255, 120));
                painter->drawEllipse(dotR.center(), DOT_SIZE / 2.0 - 1, DOT_SIZE / 2.0 - 1);
            }
        }
    }
}

// ============================================================
// 事件
// ============================================================

void FluentCarousel::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    if (leftArrowRect().contains(event->pos())) {
        m_pressedLeft = true;
        goToPrev();
    } else if (rightArrowRect().contains(event->pos())) {
        m_pressedRight = true;
        goToNext();
    } else if (m_indicatorPosition != None) {
        // 点击指示器
        for (int i = 0; i < m_slides.size(); ++i) {
            int dotX = indicatorDotX(i);
            int dotY = (m_indicatorPosition == Bottom) ?
                       height() - DOT_MARGIN_BOTTOM - DOT_SIZE :
                       DOT_MARGIN_BOTTOM;
            QRect dotR(dotX - 4, dotY - 4, DOT_SIZE + 8, DOT_SIZE + 8);
            if (dotR.contains(event->pos())) {
                goTo(i);
                break;
            }
        }
    }
    update();
}

void FluentCarousel::mouseReleaseEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    m_pressedLeft = false;
    m_pressedRight = false;
    update();
}

void FluentCarousel::mouseMoveEvent(QMouseEvent* event) {
    bool wasHovered = m_hovered;
    m_hovered = rect().contains(event->pos());

    // 检测指示器悬停
    int oldHoverDot = m_hoveredIndicator;
    m_hoveredIndicator = -1;
    if (m_indicatorPosition != None) {
        for (int i = 0; i < m_slides.size(); ++i) {
            int dotX = indicatorDotX(i);
            int dotY = (m_indicatorPosition == Bottom) ?
                       height() - DOT_MARGIN_BOTTOM - DOT_SIZE :
                       DOT_MARGIN_BOTTOM;
            QRect dotR(dotX - 4, dotY - 4, DOT_SIZE + 8, DOT_SIZE + 8);
            if (dotR.contains(event->pos())) { m_hoveredIndicator = i; break; }
        }
    }

    if (wasHovered != m_hovered || oldHoverDot != m_hoveredIndicator) {
        setCursor((leftArrowRect().contains(event->pos()) ||
                   rightArrowRect().contains(event->pos()) ||
                   m_hoveredIndicator >= 0) ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
    }
}

void FluentCarousel::leaveEvent(QEvent* event) {
    m_hovered = false;
    m_pressedLeft = false;
    m_pressedRight = false;
    m_hoveredIndicator = -1;
    setCursor(Qt::ArrowCursor);
    update();
    FluentWidget::leaveEvent(event);
}

void FluentCarousel::resizeEvent(QResizeEvent* event) {
    FluentWidget::resizeEvent(event);
    update();
}

void FluentCarousel::timerEvent(QTimerEvent* event) {
    if (event->timerId() == m_autoPlayTimerId) {
        goToNext();
    } else if (event->timerId() == m_animTimerId) {
        m_slideProgress += 1.0 / (ANIM_DURATION * ANIM_FPS / 1000.0);
        if (m_slideProgress >= 1.0) {
            m_slideProgress = 1.0;
            killTimer(m_animTimerId);
            m_animTimerId = 0;
        }
        update();
    }
}

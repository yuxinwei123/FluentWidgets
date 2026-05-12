#ifndef FLUENTCAROUSEL_H
#define FLUENTCAROUSEL_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QPixmap>
#include <QTimer>

// Fluent UI 风格轮播图组件
// 支持: 自动轮播、手动切换、指示器、左右箭头、滑动动画、主题适配

class FLUENT_EXPORT FluentCarousel : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY(bool autoPlay READ autoPlay WRITE setAutoPlay NOTIFY autoPlayChanged)
    Q_PROPERTY(IndicatorPosition indicatorPosition READ indicatorPosition WRITE setIndicatorPosition NOTIFY indicatorPositionChanged)
    Q_PROPERTY(ArrowVisibility arrowVisibility READ arrowVisibility WRITE setArrowVisibility NOTIFY arrowVisibilityChanged)

public:
    // 指示器位置
    enum IndicatorPosition {
        Bottom,     // 底部居中
        Top,        // 顶部居中
        None        // 不显示
    };
    Q_ENUM(IndicatorPosition)

    // 箭头可见性
    enum ArrowVisibility {
        Always,     // 始终显示
        OnHover,    // 悬停时显示
        Never       // 不显示
    };
    Q_ENUM(ArrowVisibility)

    explicit FluentCarousel(QWidget* parent = nullptr);
    ~FluentCarousel() override;

    // 幻灯片操作
    void addSlide(const QPixmap& pixmap);
    void addSlide(const QColor& color, const QString& text = QString());
    void removeSlide(int index);
    void clearSlides();
    int slideCount() const;

    // 当前索引
    int currentIndex() const;
    void setCurrentIndex(int index);

    // 自动播放
    bool autoPlay() const;
    void setAutoPlay(bool on);
    int interval() const;
    void setInterval(int ms);

    // 指示器
    IndicatorPosition indicatorPosition() const;
    void setIndicatorPosition(IndicatorPosition pos);

    // 箭头
    ArrowVisibility arrowVisibility() const;
    void setArrowVisibility(ArrowVisibility vis);

    // 圆角
    int borderRadius() const;
    void setBorderRadius(int r);

    // 导航（公开接口）
    void goTo(int index);
    void goToNext();
    void goToPrev();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void currentChanged(int index);
    void intervalChanged();
    void autoPlayChanged();
    void indicatorPositionChanged();
    void arrowVisibilityChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void timerEvent(QTimerEvent* event) override;

private:
    void startAutoPlay();
    void stopAutoPlay();

    // 布局计算
    QRect slideRect() const;
    QRect leftArrowRect() const;
    QRect rightArrowRect() const;
    QRect indicatorRect() const;
    int indicatorDotX(int index) const;

    // 幻灯片数据
    struct Slide {
        QPixmap pixmap;
        QColor color;
        QString text;
        bool isPixmap;
    };
    QList<Slide> m_slides;
    int m_currentIndex = -1;

    // 自动播放
    bool m_autoPlay = true;
    int m_interval = 4000;
    int m_autoPlayTimerId = 0;

    // 切换动画
    qreal m_slideProgress = 1.0;   // 0.0=开始切换, 1.0=切换完成
    int m_slideFrom = -1;
    int m_slideTo = -1;
    int m_animTimerId = 0;

    // 指示器与箭头
    IndicatorPosition m_indicatorPosition = Bottom;
    ArrowVisibility m_arrowVisibility = OnHover;
    int m_borderRadius = 8;

    // 交互状态
    bool m_hovered = false;
    bool m_pressedLeft = false;
    bool m_pressedRight = false;
    int m_hoveredIndicator = -1;

    // 常量
    enum { ARROW_SIZE = 36, ARROW_MARGIN = 12, DOT_SIZE = 8, DOT_GAP = 12,
           DOT_MARGIN_BOTTOM = 12, ANIM_DURATION = 300, ANIM_FPS = 60 };
};

#endif // FLUENTCAROUSEL_H

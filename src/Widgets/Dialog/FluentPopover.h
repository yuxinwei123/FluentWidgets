#ifndef FLUENTPOPOVER_H
#define FLUENTPOPOVER_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QPointer>
#include <QTimerEvent>
// Fluent UI 风格悬浮式窗口控件
// 支持: 锚定目标控件、多种弹出位置、箭头指示、自动关闭、主题适配

class FLUENT_EXPORT FluentPopover : public QWidget {
    Q_OBJECT
    Q_PROPERTY(PopoverPlacement placement READ placement WRITE setPlacement NOTIFY placementChanged)
    Q_PROPERTY(int arrowSize READ arrowSize WRITE setArrowSize NOTIFY arrowSizeChanged)
    Q_PROPERTY(int margin READ margin WRITE setMargin NOTIFY marginChanged)
    Q_PROPERTY(bool autoClose READ autoClose WRITE setAutoClose NOTIFY autoCloseChanged)
    Q_PROPERTY(int autoCloseDelay READ autoCloseDelay WRITE setAutoCloseDelay NOTIFY autoCloseDelayChanged)
    Q_PROPERTY(int borderRadius READ borderRadius WRITE setBorderRadius NOTIFY borderRadiusChanged)

public:
    // 弹出位置
    enum PopoverPlacement {
        Top,            // 目标上方
        Bottom,         // 目标下方
        Left,           // 目标左侧
        Right,          // 目标右侧
        TopLeft,        // 左上方
        TopRight,       // 右上方
        BottomLeft,     // 左下方
        BottomRight,    // 右下方
        Auto            // 自动选择最佳位置
    };
    Q_ENUM(PopoverPlacement)

    explicit FluentPopover(QWidget* parent = nullptr);
    ~FluentPopover() override;

    // 内容控件
    void setContent(QWidget* content);
    QWidget* content() const;

    // 目标控件
    void setTarget(QWidget* target);
    QWidget* target() const;

    // 弹出位置
    PopoverPlacement placement() const;
    void setPlacement(PopoverPlacement p);

    // 箭头大小
    int arrowSize() const;
    void setArrowSize(int size);

    // 与目标的间距
    int margin() const;
    void setMargin(int m);

    // 自动关闭
    bool autoClose() const;
    void setAutoClose(bool on);
    int autoCloseDelay() const;
    void setAutoCloseDelay(int ms);

    // 触发模式
    enum TriggerMode {
        Click,      // 点击目标控件显示
        Hover       // 悬停目标控件显示
    };
    Q_ENUM(TriggerMode)

    TriggerMode triggerMode() const;
    void setTriggerMode(TriggerMode mode);

    // 悬停显示延迟
    int hoverShowDelay() const;
    void setHoverShowDelay(int ms);

    // 悬停隐藏延迟
    int hoverHideDelay() const;
    void setHoverHideDelay(int ms);

    // 圆角
    int borderRadius() const;
    void setBorderRadius(int r);

    // 显示/隐藏
    void showPopover();
    void hidePopover();
    bool isPopoverVisible() const;

    // 便捷方法: 设置标题+内容文本
    void setTitleAndContent(const QString& title, const QString& content);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void placementChanged();
    void arrowSizeChanged();
    void marginChanged();
    void autoCloseChanged();
    void autoCloseDelayChanged();
    void borderRadiusChanged();
    void triggerModeChanged();
    void popoverShown();
    void popoverHidden();

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void timerEvent(QTimerEvent* event) override;

private:
    void updatePosition();
    void updateContentSize();
    PopoverPlacement bestPlacement() const;
    QPolygon arrowPolygon(PopoverPlacement p) const;
    QRect contentRect(PopoverPlacement p) const;
    QRectF bubbleRect(PopoverPlacement p) const;

    QPointer<QWidget> m_target;
    QPointer<QWidget> m_content;
    PopoverPlacement m_placement = Auto;
    int m_arrowSize = 10;
    int m_margin = 6;
    bool m_autoClose = true;
    int m_autoCloseDelay = 0; // 0=点击外部关闭
    int m_borderRadius = 8;
    TriggerMode m_triggerMode = Click;
    int m_hoverShowDelay = 300;
    int m_hoverHideDelay = 200;
    int m_hoverShowTimerId = 0;
    int m_hoverHideTimerId = 0;

    int m_autoCloseTimerId = 0;

    // 内置标题/内容
    QString m_title;
    QString m_contentText;

    // 常量
    enum { MIN_WIDTH = 120, MAX_WIDTH = 360, PADDING = 16, TITLE_CONTENT_GAP = 8,
           SHADOW_SIZE = 8 };
};

#endif // FLUENTPOPOVER_H

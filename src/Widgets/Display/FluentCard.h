#ifndef FLUENTCARD_H
#define FLUENTCARD_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QPixmap>

// Fluent UI 风格卡片容器
// 支持: 标题/副标题、媒体图片区、内容区、底部操作栏、悬停提升、可点击、主题适配

class QVBoxLayout;
class QLabel;

class FLUENT_EXPORT FluentCard : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString subtitle READ subtitle WRITE setSubtitle NOTIFY subtitleChanged)
    Q_PROPERTY(QPixmap coverPixmap READ coverPixmap WRITE setCoverPixmap NOTIFY coverPixmapChanged)
    Q_PROPERTY(bool clickable READ clickable WRITE setClickable NOTIFY clickableChanged)
    Q_PROPERTY(bool hoverElevation READ hoverElevation WRITE setHoverElevation NOTIFY hoverElevationChanged)
    Q_PROPERTY(int borderRadius READ borderRadius WRITE setBorderRadius NOTIFY borderRadiusChanged)
    Q_PROPERTY(int padding READ padding WRITE setPadding NOTIFY paddingChanged)
    Q_PROPERTY(bool showBorder READ showBorder WRITE setShowBorder NOTIFY showBorderChanged)

public:
    explicit FluentCard(QWidget* parent = nullptr);
    ~FluentCard() override;

    // 标题
    QString title() const;
    void setTitle(const QString& text);

    // 副标题
    QString subtitle() const;
    void setSubtitle(const QString& text);

    // 封面图片
    QPixmap coverPixmap() const;
    void setCoverPixmap(const QPixmap& pixmap);
    void setCoverPixmap(const QString& resourcePath);
    int coverHeight() const;
    void setCoverHeight(int h);

    // 可点击
    bool clickable() const;
    void setClickable(bool on);

    // 悬停提升（阴影加深）
    bool hoverElevation() const;
    void setHoverElevation(bool on);

    // 圆角
    int borderRadius() const;
    void setBorderRadius(int r);

    // 内边距
    int padding() const;
    void setPadding(int p);

    // 显示边框
    bool showBorder() const;
    void setShowBorder(bool on);

    // 内容区域 — 用户自定义 widget
    void setContentWidget(QWidget* widget);
    QWidget* contentWidget() const;

    // 底部操作栏 — 用户自定义 widget
    void setFooterWidget(QWidget* widget);
    QWidget* footerWidget() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void titleChanged();
    void subtitleChanged();
    void coverPixmapChanged();
    void clickableChanged();
    void hoverElevationChanged();
    void borderRadiusChanged();
    void paddingChanged();
    void showBorderChanged();
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupUI();
    void applyTheme();

    QString m_title;
    QString m_subtitle;
    QPixmap m_coverPixmap;
    int m_coverHeight = 140;
    bool m_clickable = false;
    bool m_hoverElevation = true;
    int m_borderRadius = 8;
    int m_padding = 16;
    bool m_showBorder = true;

    // 内部子控件
    QWidget* m_coverWidget = nullptr;
    QLabel* m_titleLabel = nullptr;
    QLabel* m_subtitleLabel = nullptr;
    QWidget* m_contentWidget = nullptr;
    QWidget* m_footerWidget = nullptr;
    QVBoxLayout* m_mainLayout = nullptr;

    bool m_pressed = false;
};

#endif // FLUENTCARD_H

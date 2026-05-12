#ifndef FLUENTDRAWER_H
#define FLUENTDRAWER_H

#include "Core/FluentGlobal.h"
#include <QWidget>
#include <QPropertyAnimation>

// Fluent UI 风格抽屉组件
// 支持: 左/右/上/下四个方向、遮罩层、滑入/滑出动画、可设置内容区域、主题适配

class QVBoxLayout;
class QLabel;

class FLUENT_EXPORT FluentDrawer : public QWidget {
    Q_OBJECT
    Q_PROPERTY(DrawerEdge edge READ edge WRITE setEdge NOTIFY edgeChanged)
    Q_PROPERTY(int drawerWidth READ drawerWidth WRITE setDrawerWidth NOTIFY drawerWidthChanged)
    Q_PROPERTY(int drawerHeight READ drawerHeight WRITE setDrawerHeight NOTIFY drawerHeightChanged)
    Q_PROPERTY(bool overlay READ overlay WRITE setOverlay NOTIFY overlayChanged)
    Q_PROPERTY(bool closable READ closable WRITE setClosable NOTIFY closableChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(qreal slideProgress READ slideProgress WRITE setSlideProgress NOTIFY slideProgressChanged)

public:
    // 抽屉弹出方向
    enum DrawerEdge {
        Left,       // 从左侧滑入
        Right,      // 从右侧滑入
        Top,        // 从顶部滑入
        Bottom      // 从底部滑入
    };
    Q_ENUM(DrawerEdge)

    explicit FluentDrawer(QWidget* parent = nullptr);
    ~FluentDrawer() override;

    // 方向
    DrawerEdge edge() const;
    void setEdge(DrawerEdge edge);

    // 尺寸（水平抽屉控制宽度，垂直抽屉控制高度）
    int drawerWidth() const;
    void setDrawerWidth(int width);
    int drawerHeight() const;
    void setDrawerHeight(int height);

    // 遮罩层
    bool overlay() const;
    void setOverlay(bool on);

    // 点击遮罩是否可关闭
    bool closable() const;
    void setClosable(bool on);

    // 标题
    QString title() const;
    void setTitle(const QString& text);

    // 内容区域
    void setContent(QWidget* content);
    QWidget* content() const;

    // 显示/隐藏
    void open();
    void close();
    bool isOpen() const;

    // 动画时长 (ms)
    int animationDuration() const;
    void setAnimationDuration(int ms);

    // 滑动进度 (0.0 ~ 1.0)
    qreal slideProgress() const;
    void setSlideProgress(qreal progress);

    QSize sizeHint() const override;

signals:
    void edgeChanged();
    void drawerWidthChanged();
    void drawerHeightChanged();
    void overlayChanged();
    void closableChanged();
    void titleChanged();
    void slideProgressChanged();
    void opened();
    void closed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void setupUI();
    void applyTheme();
    void updateMaskGeometry();
    void updatePanelGeometry();
    void updatePanelPosition();
    QRect panelTargetRect() const;
    QRect panelStartRect() const;

    DrawerEdge m_edge = Right;
    int m_drawerWidth = 320;
    int m_drawerHeight = 320;
    bool m_overlay = true;
    bool m_closable = true;
    QString m_title;
    int m_animDuration = 250;
    qreal m_slideProgress = 0.0;
    bool m_isOpen = false;

    QWidget* m_maskWidget = nullptr;        // 遮罩层
    QWidget* m_drawerPanel = nullptr;       // 抽屉面板
    QVBoxLayout* m_panelLayout = nullptr;   // 面板布局
    QLabel* m_titleLabel = nullptr;         // 标题
    QWidget* m_contentWidget = nullptr;     // 用户内容
    QPropertyAnimation* m_slideAnim = nullptr;

    // 常量
    enum { HEADER_HEIGHT = 48, PADDING = 0, BORDER_RADIUS = 8 };
};

#endif // FLUENTDRAWER_H

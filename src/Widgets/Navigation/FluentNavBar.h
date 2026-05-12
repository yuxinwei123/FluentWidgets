#ifndef FLUENTNAVBAR_H
#define FLUENTNAVBAR_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QIcon>
#include <QMap>

// Fluent UI 风格导航栏控件
// 支持: 选项卡式(Tab)导航、面包屑式(Breadcrumb)导航、胶囊式(Pill)导航
// 特性: 选中指示器动画、溢出滚动、主题适配

// ========== 导航栏项 ==========
class FLUENT_EXPORT FluentNavBarItem {
public:
    FluentNavBarItem(const QString& key, const QString& text, const QIcon& icon = QIcon());
    QString key() const;
    QString text() const;
    QIcon icon() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);

private:
    QString m_key, m_text;
    QIcon m_icon;
    bool m_enabled = true;
};

// ========== 导航栏控件 ==========
class FLUENT_EXPORT FluentNavBar : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(QString currentKey READ currentKey WRITE setCurrentKey NOTIFY currentChanged)

public:
    // 导航样式
    enum NavStyle {
        Tab = 0,        // 选项卡（底部指示线）
        Pill = 1,       // 胶囊（选中项填充背景）
        Breadcrumb = 2  // 面包屑（> 分隔符）
    };
    Q_ENUM(NavStyle)

    // 方向
    enum Orientation {
        Horizontal = 0,
        Vertical = 1
    };
    Q_ENUM(Orientation)

    Q_PROPERTY(NavStyle navStyle READ navStyle WRITE setNavStyle NOTIFY navStyleChanged)
    Q_PROPERTY(Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)

    explicit FluentNavBar(QWidget* parent = nullptr);
    ~FluentNavBar() override;

    // 添加导航项
    void addItem(const QString& key, const QString& text, const QIcon& icon = QIcon());
    void removeItem(const QString& key);
    int itemCount() const;

    // 当前选中
    QString currentKey() const;
    void setCurrentKey(const QString& key);
    int currentIndex() const;

    // 样式
    NavStyle navStyle() const;
    void setNavStyle(NavStyle style);

    // 方向
    Orientation orientation() const;
    void setOrientation(Orientation orient);

    // 尺寸
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

    // 指示器动画（Q_PROPERTY 需要 public 访问）
    Q_PROPERTY(qreal indicatorProgress READ indicatorProgress WRITE setIndicatorProgress NOTIFY indicatorProgressChanged)
    qreal indicatorProgress() const;
    void setIndicatorProgress(qreal progress);

signals:
    void currentChanged(const QString& key);
    void navStyleChanged();
    void orientationChanged();
    void itemClicked(const QString& key);
    void indicatorProgressChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    // 布局计算
    QRect itemRect(int index) const;
    int itemAtPos(const QPoint& pos) const;
    int indicatorX() const;
    int indicatorWidth() const;
    void updateIndicatorTarget();

    // 数据
    QList<FluentNavBarItem> m_items;
    QString m_currentKey;
    NavStyle m_navStyle = NavStyle::Tab;
    Orientation m_orientation = Orientation::Horizontal;

    // 指示器动画
    qreal m_indicatorProgress = 0.0;
    int m_indicatorFromX = 0;
    int m_indicatorFromW = 0;
    int m_indicatorToX = 0;
    int m_indicatorToW = 0;
    qreal m_indicatorX = 0;
    qreal m_indicatorW = 0;

    // 悬停
    int m_hoveredIndex = -1;

    // 常量
    enum { ITEM_HEIGHT = 40, ITEM_H_PADDING = 16, INDICATOR_HEIGHT = 3,
           PILL_RADIUS = 6, BREADCRUMB_GAP = 8, ICON_SIZE = 16, ICON_TEXT_GAP = 6 };
};

#endif // FLUENTNAVBAR_H

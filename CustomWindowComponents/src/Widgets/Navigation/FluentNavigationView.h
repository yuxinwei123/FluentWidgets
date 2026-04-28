#ifndef FLUENTNAVIGATIONVIEW_H
#define FLUENTNAVIGATIONVIEW_H

#include "Core/FluentGlobal.h"
#include <QWidget>
#include <QScrollArea>
#include <QMap>
#include <QIcon>
class FluentTheme;
class FluentNavigationItem;
class QVBoxLayout;
class QStackedWidget;

// Fluent UI 风格侧边多级导航视图
// 左侧导航菜单 + 右侧页面切换，支持多级折叠展开
class FLUENT_EXPORT FluentNavigationView : public QWidget {
    Q_OBJECT
    Q_PROPERTY(FluentNavigationDisplayMode displayMode READ displayMode WRITE setDisplayMode NOTIFY displayModeChanged)
    Q_PROPERTY(int navigationWidth READ navigationWidth WRITE setNavigationWidth NOTIFY navigationWidthChanged)
    Q_PROPERTY(QString currentPageKey READ currentPageKey NOTIFY currentPageChanged)

public:
    explicit FluentNavigationView(QWidget* parent = nullptr);

    // 显示模式
    FluentNavigationDisplayMode displayMode() const;
    void setDisplayMode(FluentNavigationDisplayMode mode);

    // 导航栏宽度
    int navigationWidth() const;
    void setNavigationWidth(int width);

    // 添加顶层导航项
    FluentNavigationItem* addItem(const QString& text, const QIcon& icon = QIcon(),
                                   const QString& key = QString());

    // 在指定项下添加子项
    FluentNavigationItem* addSubItem(FluentNavigationItem* parent,
                                      const QString& text, const QIcon& icon = QIcon(),
                                      const QString& key = QString());

    // 页面管理
    void addPage(const QString& key, QWidget* page);
    void removePage(const QString& key);
    QWidget* page(const QString& key) const;

    // 当前页面
    QString currentPageKey() const;
    void setCurrentPage(const QString& key);

    // 导航项查找
    FluentNavigationItem* findItem(const QString& key) const;

    // 右侧内容区域的堆叠页面容器
    QStackedWidget* stackedWidget() const;

signals:
    void displayModeChanged();
    void navigationWidthChanged();
    void currentPageChanged(const QString& key);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onItemClicked(const QString& key);

private:
    void setupUI();
    void selectItem(const QString& key);
    void clearSelection(FluentNavigationItem* item);

    FluentNavigationDisplayMode m_displayMode = FluentNavigationDisplayMode::Expanded;
    int m_navigationWidth = 240;
    QString m_currentPageKey;

    // 左侧导航区域
    QWidget* m_navPanel = nullptr;
    QScrollArea* m_scrollArea = nullptr;
    QWidget* m_navContent = nullptr;
    QVBoxLayout* m_navLayout = nullptr;

    // 右侧内容区域
    QStackedWidget* m_stackWidget = nullptr;

    // 所有导航项
    QList<FluentNavigationItem*> m_topItems;

    // 页面映射
    QMap<QString, QWidget*> m_pages;
    QMap<QString, FluentNavigationItem*> m_itemMap;
};

#endif // FLUENTNAVIGATIONVIEW_H

#ifndef FLUENTNAVIGATIONITEM_H
#define FLUENTNAVIGATIONITEM_H

#include "Core/FluentGlobal.h"
#include <QWidget>
#include <QIcon>

class FluentTheme;
class QVBoxLayout;
class QPropertyAnimation;

// 单个导航菜单项（可展开/折叠子项）
class FLUENT_EXPORT FluentNavigationItem : public QWidget {
    Q_OBJECT
    Q_PROPERTY(bool expanded READ expanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(qreal expandProgress READ expandProgress WRITE setExpandProgress NOTIFY expandProgressChanged)

public:
    // 构造顶层菜单项
    FluentNavigationItem(const QString& text, const QIcon& icon = QIcon(),
                          const QString& key = QString(), QWidget* parent = nullptr);

    // 添加子菜单项
    FluentNavigationItem* addChild(const QString& text, const QIcon& icon = QIcon(),
                                    const QString& key = QString());

    // 属性
    QString text() const;
    void setText(const QString& text);

    QIcon icon() const;
    void setIcon(const QIcon& icon);

    QString key() const;
    void setKey(const QString& key);

    bool expanded() const;
    void setExpanded(bool expanded);

    bool selected() const;
    void setSelected(bool selected);

    bool hasChildren() const;

    qreal expandProgress() const;
    void setExpandProgress(qreal progress);

    QList<FluentNavigationItem*> children() const;
    FluentNavigationItem* parentItem() const;
    void setParentItem(FluentNavigationItem* parent);

    int indentLevel() const;

    QSize sizeHint() const override;

signals:
    void clicked(const QString& key);
    void expandedChanged(bool expanded);
    void selectedChanged(bool selected);
    void expandProgressChanged(qreal progress);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void setupUI();
    void updateChildVisibility();
    void toggleExpanded();
    int contentHeight() const;

    QString m_text;
    QIcon m_icon;
    QString m_key;
    bool m_expanded = false;
    bool m_selected = false;
    bool m_hovered = false;
    bool m_pressed = false;
    qreal m_expandProgress = 0.0;

    QVBoxLayout* m_layout = nullptr;
    QWidget* m_childrenContainer = nullptr;
    QVBoxLayout* m_childrenLayout = nullptr;
    QList<FluentNavigationItem*> m_children;
    FluentNavigationItem* m_parentItem = nullptr;

    static const int ITEM_HEIGHT = 36;
    static const int INDENT_PER_LEVEL = 20;
    static const int ICON_SIZE = 16;
    static const int LEFT_PADDING = 12;
    static const int ARROW_SIZE = 8;
};

#endif // FLUENTNAVIGATIONITEM_H

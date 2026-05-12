#include "FluentNavigationView.h"
#include "FluentNavigationItem.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QStackedWidget>
#include <QPainter>
#include <QPainterPath>

FluentNavigationView::FluentNavigationView(QWidget* parent)
    : QWidget(parent)
{
    setupUI();

    auto* theme = FluentThemeManager::instance().currentTheme();
    connect(theme, &FluentTheme::themeChanged, this, [this]() {
        update();
        // 更新滚动区域样式跟随主题
        auto* t = FluentThemeManager::instance().currentTheme();
        m_scrollArea->setStyleSheet(
            QString("QScrollArea { background: transparent; border: none; }"
                    "QScrollBar:vertical { background: transparent; width: 6px; margin: 0; }"
                    "QScrollBar::handle:vertical { background: %1; border-radius: 3px; min-height: 20px; }"
                    "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }")
            .arg(t->borderColor().name())
        );
    });
}

FluentNavigationDisplayMode FluentNavigationView::displayMode() const { return m_displayMode; }
void FluentNavigationView::setDisplayMode(FluentNavigationDisplayMode mode) {
    if (m_displayMode != mode) {
        m_displayMode = mode;
        emit displayModeChanged();
    }
}

int FluentNavigationView::navigationWidth() const { return m_navigationWidth; }
void FluentNavigationView::setNavigationWidth(int width) {
    if (m_navigationWidth != width) {
        m_navigationWidth = width;
        if (m_navPanel) {
            m_navPanel->setFixedWidth(width);
        }
        update();
        emit navigationWidthChanged();
    }
}

QString FluentNavigationView::currentPageKey() const { return m_currentPageKey; }

QStackedWidget* FluentNavigationView::stackedWidget() const { return m_stackWidget; }

FluentNavigationItem* FluentNavigationView::addItem(const QString& text, const QIcon& icon,
                                                      const QString& key) {
    QString itemKey = key.isEmpty() ? text : key;
    auto* item = new FluentNavigationItem(text, icon, itemKey, m_navContent);
    m_topItems.append(item);
    m_itemMap[itemKey] = item;

    // 插入到 stretch 之前
    int idx = m_navLayout->count() - 1; // 最后一个是 stretch
    m_navLayout->insertWidget(idx, item);

    connect(item, &FluentNavigationItem::clicked, this, &FluentNavigationView::onItemClicked);

    return item;
}

FluentNavigationItem* FluentNavigationView::addSubItem(FluentNavigationItem* parent,
                                                        const QString& text, const QIcon& icon,
                                                        const QString& key) {
    if (!parent) return nullptr;

    QString itemKey = key.isEmpty() ? text : key;
    auto* child = parent->addChild(text, icon, itemKey);
    m_itemMap[itemKey] = child;

    connect(child, &FluentNavigationItem::clicked, this, &FluentNavigationView::onItemClicked);

    return child;
}

void FluentNavigationView::addPage(const QString& key, QWidget* page) {
    if (!page) return;
    m_pages[key] = page;
    m_stackWidget->addWidget(page);

    // 默认选中第一个
    if (m_currentPageKey.isEmpty()) {
        setCurrentPage(key);
    }
}

void FluentNavigationView::removePage(const QString& key) {
    if (m_pages.contains(key)) {
        QWidget* p = m_pages.take(key);
        m_stackWidget->removeWidget(p);
        p->deleteLater();
    }
}

QWidget* FluentNavigationView::page(const QString& key) const {
    return m_pages.value(key, nullptr);
}

void FluentNavigationView::setCurrentPage(const QString& key) {
    if (m_currentPageKey == key) return;

    if (m_pages.contains(key)) {
        m_currentPageKey = key;
        m_stackWidget->setCurrentWidget(m_pages[key]);
        selectItem(key);
        emit currentPageChanged(key);
    }
}

FluentNavigationItem* FluentNavigationView::findItem(const QString& key) const {
    return m_itemMap.value(key, nullptr);
}

void FluentNavigationView::selectItem(const QString& key) {
    // 清除所有选中
    for (auto* item : m_topItems) {
        clearSelection(item);
    }

    // 设置新选中
    auto* target = findItem(key);
    if (target) {
        target->setSelected(true);

        // 确保父项展开
        FluentNavigationItem* parent = target->parentItem();
        while (parent) {
            if (!parent->expanded()) {
                parent->setExpanded(true);
            }
            parent = parent->parentItem();
        }
    }
}

void FluentNavigationView::clearSelection(FluentNavigationItem* item) {
    if (!item) return;
    item->setSelected(false);
    for (auto* child : item->children()) {
        clearSelection(child);
    }
}

void FluentNavigationView::onItemClicked(const QString& key) {
    // 如果是叶子节点（无子项），切换页面
    auto* item = findItem(key);
    if (item && !item->hasChildren()) {
        setCurrentPage(key);
    }
    // 如果有子项，点击行为由 FluentNavigationItem 自行处理展开/折叠
}

void FluentNavigationView::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    auto* theme = FluentThemeManager::instance().currentTheme();

    // 导航栏背景
    QRect navRect(0, 0, m_navigationWidth, height());
    p.fillRect(navRect, theme->surfaceColor());

    // 导航栏右侧分割线
    p.setPen(theme->dividerColor());
    p.drawLine(m_navigationWidth, 0, m_navigationWidth, height());
}

void FluentNavigationView::setupUI() {
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航面板
    m_navPanel = new QWidget(this);
    m_navPanel->setFixedWidth(m_navigationWidth);
    m_navPanel->setStyleSheet("background: transparent;");

    auto* navPanelLayout = new QVBoxLayout(m_navPanel);
    navPanelLayout->setContentsMargins(0, 0, 0, 0);
    navPanelLayout->setSpacing(0);

    // 滚动区域
    m_scrollArea = new QScrollArea(m_navPanel);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");

    // 导航内容
    m_navContent = new QWidget(m_scrollArea);
    m_navContent->setStyleSheet("background: transparent;");
    m_navLayout = new QVBoxLayout(m_navContent);
    m_navLayout->setContentsMargins(4, 8, 4, 8);
    m_navLayout->setSpacing(2);
    m_navLayout->addStretch();

    m_scrollArea->setWidget(m_navContent);
    navPanelLayout->addWidget(m_scrollArea);

    // 右侧内容区域
    m_stackWidget = new QStackedWidget(this);
    m_stackWidget->setStyleSheet("background: transparent;");

    mainLayout->addWidget(m_navPanel);
    mainLayout->addWidget(m_stackWidget, 1);
}

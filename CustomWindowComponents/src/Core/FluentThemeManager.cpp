#include "FluentThemeManager.h"
#include <QWidget>
#include <QApplication>

FluentThemeManager& FluentThemeManager::instance() {
    static FluentThemeManager mgr;
    return mgr;
}

FluentThemeManager::FluentThemeManager()
    : m_theme(new FluentTheme(FluentThemeMode::Light, this))
{
}

FluentThemeManager::~FluentThemeManager() = default;

FluentTheme* FluentThemeManager::currentTheme() const { return m_theme; }

FluentThemeMode FluentThemeManager::themeMode() const {
    return m_theme->themeMode();
}

void FluentThemeManager::setThemeMode(FluentThemeMode mode) {
    m_theme->setThemeMode(mode);
    // 通知所有注册的 widget 刷新
    for (auto* w : QApplication::allWidgets()) {
        applyThemeToWidget(w);
    }
    emit themeChanged();
}

void FluentThemeManager::registerWidget(QWidget* widget) {
    Q_UNUSED(widget)
    // 可扩展: 维护 widget 列表用于精确刷新
}

void FluentThemeManager::unregisterWidget(QWidget* widget) {
    Q_UNUSED(widget)
}

void FluentThemeManager::applyThemeToWidget(QWidget* widget) {
    if (widget) {
        widget->update();
    }
}

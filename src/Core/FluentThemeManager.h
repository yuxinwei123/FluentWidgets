#ifndef FLUENTTHEMEMANAGER_H
#define FLUENTTHEMEMANAGER_H

#include "FluentGlobal.h"
#include "FluentTheme.h"
#include <QObject>

class FLUENT_EXPORT FluentThemeManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(FluentTheme* currentTheme READ currentTheme NOTIFY themeChanged)
    Q_PROPERTY(FluentThemeMode themeMode READ themeMode WRITE setThemeMode NOTIFY themeChanged)

public:
    static FluentThemeManager& instance();

    FluentTheme* currentTheme() const;
    FluentThemeMode themeMode() const;
    void setThemeMode(FluentThemeMode mode);

    // 注册 widget 到主题管理，主题变更时自动更新
    void registerWidget(QWidget* widget);
    void unregisterWidget(QWidget* widget);

signals:
    void themeChanged();

private:
    FluentThemeManager();
    ~FluentThemeManager() override;

    void applyThemeToWidget(QWidget* widget);

    FluentTheme* m_theme;
};

#endif // FLUENTTHEMEMANAGER_H

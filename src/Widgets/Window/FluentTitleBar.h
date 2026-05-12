#ifndef FLUENTTITLEBAR_H
#define FLUENTTITLEBAR_H

#include "Core/FluentGlobal.h"
#include <QWidget>

class QLabel;
class QPushButton;
class FluentToggleSwitch;

class FLUENT_EXPORT FluentTitleBar : public QWidget {
    Q_OBJECT

public:
    explicit FluentTitleBar(QWidget* parent = nullptr);

    void setTitle(const QString& title);
    void setIcon(const QIcon& icon);

    int titleBarHeight() const;

    // 主题切换
    void setThemeToggleVisible(bool visible);
    bool isThemeToggleVisible() const;

signals:
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();
    void themeToggleRequested(bool dark);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    void setupUI();
    void updateThemeToggleStyle();

    QLabel* m_iconLabel = nullptr;
    QLabel* m_titleLabel = nullptr;
    QPushButton* m_themeBtn = nullptr;
    FluentToggleSwitch* m_themeToggle = nullptr;
    QPushButton* m_minBtn = nullptr;
    QPushButton* m_maxBtn = nullptr;
    QPushButton* m_closeBtn = nullptr;
    bool m_themeToggleVisible = true;
    static constexpr int TITLE_BAR_HEIGHT = 32;
    static constexpr int ICON_SIZE = 16;
};

#endif // FLUENTTITLEBAR_H

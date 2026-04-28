#ifndef FLUENTWIDGET_H
#define FLUENTWIDGET_H

#include "Core/FluentGlobal.h"
#include <QWidget>

class FluentTheme;

class FLUENT_EXPORT FluentWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(FluentTheme* theme READ theme WRITE setTheme NOTIFY themeChanged)

public:
    explicit FluentWidget(QWidget* parent = nullptr);
    ~FluentWidget() override;

    FluentTheme* theme() const;
    void setTheme(FluentTheme* theme);

    // 子类必须实现: 使用 theme() Token 绘制
    virtual void paintFluent(QPainter* painter) = 0;

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    bool isHovered() const;
    bool isPressed() const;

signals:
    void themeChanged();

private:
    FluentTheme* m_theme;
    bool m_hovered = false;
    bool m_pressed = false;
};

#endif // FLUENTWIDGET_H

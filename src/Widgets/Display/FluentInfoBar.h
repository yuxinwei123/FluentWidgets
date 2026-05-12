#ifndef FLUENTINFOBAR_H
#define FLUENTINFOBAR_H

#include "Core/FluentGlobal.h"
#include <QWidget>

class FluentTheme;
class QTimer;
class QPropertyAnimation;

// Fluent UI 风格气泡消息条 (InfoBar)
// 支持顶部/底部弹出，自动消失，Info/Success/Warning/Error 四种类型
class FLUENT_EXPORT FluentInfoBar : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal slideProgress READ slideProgress WRITE setSlideProgress NOTIFY slideProgressChanged)

public:
    enum class BarType {
        Info,
        Success,
        Warning,
        Error
    };
    Q_ENUM(BarType)

    enum class Position {
        Top,
        Bottom
    };
    Q_ENUM(Position)

    explicit FluentInfoBar(QWidget* parent = nullptr);

    // 弹出一条消息
    static void info(QWidget* parent, const QString& title, const QString& content,
                     int duration = 3000, Position pos = Position::Top);
    static void success(QWidget* parent, const QString& title, const QString& content,
                        int duration = 3000, Position pos = Position::Top);
    static void warning(QWidget* parent, const QString& title, const QString& content,
                        int duration = 3000, Position pos = Position::Top);
    static void error(QWidget* parent, const QString& title, const QString& content,
                      int duration = 5000, Position pos = Position::Top);

    // 手动创建
    void setBarType(BarType type);
    BarType barType() const;

    void setTitle(const QString& title);
    QString title() const;

    void setContent(const QString& content);
    QString content() const;

    void setPosition(Position pos);
    Position position() const;

    void setDuration(int ms);
    int duration() const;

    void setCloseButtonVisible(bool visible);
    bool closeButtonVisible() const;

    // 显示/隐藏动画
    void showBar();
    void dismissBar();

    // 滑动进度 (动画用)
    qreal slideProgress() const;
    void setSlideProgress(qreal progress);

    QSize sizeHint() const override;

signals:
    void slideProgressChanged();
    void dismissed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void setupUI();
    QColor accentColor() const;
    QString iconChar() const;
    QRect closeBtnRect() const;
    bool isOverCloseButton(const QPoint& pos) const;

    BarType m_type = BarType::Info;
    Position m_position = Position::Top;
    QString m_title;
    QString m_content;
    int m_duration = 3000;
    bool m_closeBtnVisible = true;
    qreal m_slideProgress = 0.0;

    QTimer* m_autoCloseTimer = nullptr;
    bool m_hovered = false;

    static const int BAR_HEIGHT = 48;
    static const int ICON_SIZE = 16;
    static const int CLOSE_BTN_SIZE = 20;
    static const int LEFT_PADDING = 16;
    static const int RIGHT_PADDING = 12;
    static const int SPACING = 8;
};

#endif // FLUENTINFOBAR_H

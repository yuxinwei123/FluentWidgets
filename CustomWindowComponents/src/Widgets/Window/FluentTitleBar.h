#ifndef FLUENTTITLEBAR_H
#define FLUENTTITLEBAR_H

#include "Core/FluentGlobal.h"
#include <QWidget>

class QLabel;
class QPushButton;

class FLUENT_EXPORT FluentTitleBar : public QWidget {
    Q_OBJECT

public:
    explicit FluentTitleBar(QWidget* parent = nullptr);

    void setTitle(const QString& title);
    void setIcon(const QIcon& icon);

    int titleBarHeight() const;

signals:
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    void setupUI();

    QLabel* m_titleLabel = nullptr;
    QPushButton* m_minBtn = nullptr;
    QPushButton* m_maxBtn = nullptr;
    QPushButton* m_closeBtn = nullptr;
    static constexpr int TITLE_BAR_HEIGHT = 32;
};

#endif // FLUENTTITLEBAR_H

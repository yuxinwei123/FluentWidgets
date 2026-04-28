#ifndef FLUENTWINDOW_H
#define FLUENTWINDOW_H

#include "Core/FluentGlobal.h"
#include <QWidget>

class FluentTitleBar;
class FluentTheme;

class FLUENT_EXPORT FluentWindow : public QWidget {
    Q_OBJECT
    Q_PROPERTY(bool micaEnabled READ micaEnabled WRITE setMicaEnabled NOTIFY micaChanged)
    Q_PROPERTY(bool acrylicEnabled READ acrylicEnabled WRITE setAcrylicEnabled NOTIFY acrylicChanged)

public:
    explicit FluentWindow(QWidget* parent = nullptr);

    FluentTitleBar* titleBar() const;
    void setCentralWidget(QWidget* widget);

    bool micaEnabled() const;
    void setMicaEnabled(bool enabled);

    bool acrylicEnabled() const;
    void setAcrylicEnabled(bool enabled);

    void setWindowTitle(const QString& title);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

#ifdef Q_OS_WIN
    bool hitTestWindows(MSG* msg, long* result);
#endif

signals:
    void micaChanged();
    void acrylicChanged();

private slots:
    void onMinimize();
    void onMaximizeRestore();
    void onClose();

private:
    void setupUI();
    void applyMicaEffect();
    void removeMicaEffect();

    FluentTitleBar* m_titleBar = nullptr;
    QWidget* m_centralWidget = nullptr;
    bool m_micaEnabled = false;
    bool m_acrylicEnabled = false;
};

#endif // FLUENTWINDOW_H

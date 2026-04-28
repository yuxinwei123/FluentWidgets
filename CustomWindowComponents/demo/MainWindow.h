#ifndef DEMO_MAINWINDOW_H
#define DEMO_MAINWINDOW_H

#include <FluentWidgets.h>

class QLabel;

class DemoMainWindow : public FluentWindow {
    Q_OBJECT

public:
    explicit DemoMainWindow(QWidget* parent = nullptr);

private:
    void setupNavigation();

    // 页面创建
    QWidget* createHomePage();
    QWidget* createButtonPage();
    QWidget* createInputPage();
    QWidget* createDialogPage();
    QWidget* createSettingsPage();
    QWidget* createAboutPage();

    FluentNavigationView* m_navView = nullptr;
    QLabel* m_themeLabel = nullptr;

    void toggleTheme();
};

#endif // DEMO_MAINWINDOW_H

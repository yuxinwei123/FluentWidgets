#ifndef DEMO_MAINWINDOW_H
#define DEMO_MAINWINDOW_H

#include <FluentWidgets.h>
#include <QMap>

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
    QWidget* createDrawerPage();
    QWidget* createTablePage();
    QWidget* createTreePage();
    QWidget* createGraphPage();
    QWidget* createNavBarPage();
    QWidget* createCalendarPage();
    QWidget* createCarouselPage();
    QWidget* createCardPage();
    QWidget* createProgressBarPage();
    QWidget* createProgressRingPage();
    QWidget* createLineChartPage();
    QWidget* createBarChartPage();
    QWidget* createTimelinePage();
    QWidget* createPieChartPage();
    QWidget* createSliderPage();
    QWidget* createDatePickerPage();
    QWidget* createTimePickerPage();
    QWidget* createColorPickerPage();
    QWidget* createSettingsPage();
    QWidget* createAboutPage();

    FluentNavigationView* m_navView = nullptr;
    QLabel* m_themeLabel = nullptr;

    void toggleTheme();
    void updateWindowIcon();

    QMap<QWidget*, QString> m_cardPageMap;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // DEMO_MAINWINDOW_H

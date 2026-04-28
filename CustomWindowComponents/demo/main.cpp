#include "MainWindow.h"
#include "Core/FluentTheme.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 注册 QColor 插值函数，使 QPropertyAnimation 能驱动颜色渐变
    FluentTheme::registerColorInterpolator();

    app.setApplicationName(u8"FluentWidgets Demo");
    app.setOrganizationName(u8"FluentWidgets");

    DemoMainWindow window;
    window.show();

    return app.exec();
}

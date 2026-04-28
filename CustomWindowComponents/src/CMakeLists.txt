project(FluentWidgets VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 定义导出宏
add_definitions(-DFLUENTWIDGETS_LIB)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

set(FLUENT_SOURCES
    # Core
    Core/FluentConfig.cpp
    Core/FluentTheme.cpp
    Core/FluentThemeManager.cpp

    # Animations
    Animations/FluentAnimation.cpp

    # Effects
    Effects/FluentAcrylicEffect.cpp
    Effects/FluentRevealEffect.cpp

    # Widgets - Base
    Widgets/Base/FluentWidget.cpp

    # Widgets - Dialog
    Widgets/Dialog/FluentContentDialog.cpp

    # Widgets - Display
    Widgets/Display/FluentInfoBar.cpp

    # Widgets - Buttons
    Widgets/Buttons/FluentButton.cpp

    # Widgets - Input
    Widgets/Input/FluentLineEdit.cpp
    Widgets/Input/FluentCheckBox.cpp
    Widgets/Input/FluentToggleSwitch.cpp

    # Widgets - Navigation
    Widgets/Navigation/FluentNavigationItem.cpp
    Widgets/Navigation/FluentNavigationView.cpp

    # Widgets - Window
    Widgets/Window/FluentWindow.cpp
    Widgets/Window/FluentTitleBar.cpp
)

set(FLUENT_HEADERS
    FluentWidgets.h
    Core/FluentGlobal.h
    Core/FluentConfig.h
    Core/FluentTheme.h
    Core/FluentThemeManager.h
    Animations/FluentAnimation.h
    Effects/FluentAcrylicEffect.h
    Effects/FluentRevealEffect.h
    Widgets/Base/FluentWidget.h
    Widgets/Buttons/FluentButton.h
    Widgets/Dialog/FluentContentDialog.h
    Widgets/Display/FluentInfoBar.h
    Widgets/Input/FluentLineEdit.h
    Widgets/Input/FluentCheckBox.h
    Widgets/Input/FluentToggleSwitch.h
    Widgets/Navigation/FluentNavigationItem.h
    Widgets/Navigation/FluentNavigationView.h
    Widgets/Window/FluentWindow.h
    Widgets/Window/FluentTitleBar.h
)

add_library(FluentWidgets STATIC ${FLUENT_SOURCES} ${FLUENT_HEADERS})

target_include_directories(FluentWidgets PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(FluentWidgets
    PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets
)

# Windows 平台特殊依赖
if(WIN32)
    target_link_libraries(FluentWidgets PRIVATE dwmapi user32)
endif()

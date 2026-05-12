project(FluentWidgets VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    Widgets/Dialog/FluentPopover.cpp
    Widgets/Dialog/FluentDrawer.cpp

    # Widgets - Display
    Widgets/Display/FluentInfoBar.cpp
    Widgets/Display/FluentCarousel.cpp
    Widgets/Display/FluentProgressBar.cpp
    Widgets/Display/FluentProgressRing.cpp
    Widgets/Display/FluentCard.cpp

    # Widgets - Buttons
    Widgets/Buttons/FluentButton.cpp

    # Widgets - Input
    Widgets/Input/FluentLineEdit.cpp
    Widgets/Input/FluentCheckBox.cpp
    Widgets/Input/FluentToggleSwitch.cpp
    Widgets/Input/FluentSpinBox.cpp
    Widgets/Input/FluentComboBox.cpp
    Widgets/Input/FluentSlider.cpp
    Widgets/Input/FluentDatePicker.cpp
    Widgets/Input/FluentTimePicker.cpp
    Widgets/Input/FluentColorPicker.cpp

    # Widgets - Data
    Widgets/Data/FluentTable.cpp
    Widgets/Data/FluentTree.cpp
    Widgets/Data/FluentGraph.cpp
    Widgets/Data/FluentCalendar.cpp
    Widgets/Data/FluentLineChart.cpp
    Widgets/Data/FluentBarChart.cpp
    Widgets/Data/FluentTimeline.cpp
    Widgets/Data/FluentPieChart.cpp

    # Widgets - Navigation
    Widgets/Navigation/FluentNavigationItem.cpp
    Widgets/Navigation/FluentNavigationView.cpp
    Widgets/Navigation/FluentNavBar.cpp

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
    Widgets/Dialog/FluentPopover.h
    Widgets/Dialog/FluentDrawer.h
    Widgets/Display/FluentInfoBar.h
    Widgets/Display/FluentCarousel.h
    Widgets/Display/FluentProgressBar.h
    Widgets/Display/FluentProgressRing.h
    Widgets/Display/FluentCard.h
    Widgets/Input/FluentLineEdit.h
    Widgets/Input/FluentCheckBox.h
    Widgets/Input/FluentToggleSwitch.h
    Widgets/Input/FluentSpinBox.h
    Widgets/Input/FluentComboBox.h
    Widgets/Input/FluentSlider.h
    Widgets/Input/FluentDatePicker.h
    Widgets/Input/FluentTimePicker.h
    Widgets/Input/FluentColorPicker.h
    Widgets/Data/FluentTable.h
    Widgets/Data/FluentTree.h
    Widgets/Data/FluentGraph.h
    Widgets/Data/FluentCalendar.h
    Widgets/Data/FluentLineChart.h
    Widgets/Data/FluentBarChart.h
    Widgets/Data/FluentTimeline.h
    Widgets/Navigation/FluentNavigationItem.h
    Widgets/Navigation/FluentNavigationView.h
    Widgets/Navigation/FluentNavBar.h
    Widgets/Window/FluentWindow.h
    Widgets/Window/FluentTitleBar.h
)

add_library(FluentWidgets SHARED ${FLUENT_SOURCES} ${FLUENT_HEADERS})

# 构建库时定义两个宏：FLUENTWIDGETS_LIB（正在编译库）+ FLUENTWIDGETS_SHARED（动态库模式）
target_compile_definitions(FluentWidgets
    PRIVATE FLUENTWIDGETS_LIB
    PUBLIC FLUENTWIDGETS_SHARED
)

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

# 输出 dll 到统一 bin 目录，方便 exe 运行时找到
set_target_properties(FluentWidgets PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

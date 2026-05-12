project(FluentWidgetsDemo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

set(DEMO_SOURCES
    main.cpp
    MainWindow.cpp
    resources.qrc
)

add_executable(FluentWidgetsDemo ${DEMO_SOURCES})

#target_include_directories(FluentWidgetsDemo PRIVATE
#    ${CMAKE_SOURCE_DIR}/src
#)

target_link_libraries(FluentWidgetsDemo PRIVATE FluentWidgets Qt5::Widgets)

# 将 demo 输出到和动态库同一目录，避免运行时找不到 dll
set_target_properties(FluentWidgetsDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(WIN32)
    set_target_properties(FluentWidgetsDemo PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

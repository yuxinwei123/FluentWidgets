cmake_minimum_required(VERSION 3.5)

project(CustomWindowComponents VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt5 兼容: 如果用户未设置 CMAKE_PREFIX_PATH，尝试常见安装路径
if(NOT DEFINED CMAKE_PREFIX_PATH)
    if(EXISTS "C:/Qt/5.15.2/msvc2019_64")
        set(CMAKE_PREFIX_PATH "C:/Qt/5.15.2/msvc2019_64")
    elseif(EXISTS "C:/Qt/5.15.2/msvc2019")
        set(CMAKE_PREFIX_PATH "C:/Qt/5.15.2/msvc2019")
    elseif(EXISTS "D:/Qt/5.15.2/msvc2019_64")
        set(CMAKE_PREFIX_PATH "D:/Qt/5.15.2/msvc2019_64")
    endif()
endif()

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# 组件库
add_subdirectory(src)

# Demo 应用
add_subdirectory(demo)

#ifndef FLUENTGLOBAL_H
#define FLUENTGLOBAL_H

#include <QtGlobal>

// 导出宏 — 静态库构建时不需要特殊导出
#ifdef FLUENTWIDGETS_SHARED
#  ifdef FLUENTWIDGETS_LIB
#    define FLUENT_EXPORT Q_DECL_EXPORT
#  else
#    define FLUENT_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define FLUENT_EXPORT
#endif

// Fluent Design 主题模式
enum class FluentThemeMode {
    Light,
    Dark,
    Auto     // 跟随系统
};

// 按钮变体
enum class FluentButtonVariant {
    Primary,        // 主要操作，填充色
    Secondary,      // 次要操作，边框
    Subtle,         // 轻量，无边框
    Accent,         // 强调色
    Transparent,    // 透明背景
    Danger,         // 危险操作，红色
    Toggle,         // 切换
    Split           // 分体
};

// 组件尺寸
enum class FluentSize {
    Small,
    Medium,
    Large
};

// 导航显示模式
enum class FluentNavigationDisplayMode {
    Minimal,       // 仅图标
    Compact,       // 图标+小文字
    Expanded,      // 完整展开
    Auto           // 跟随窗口宽度
};

// 信息条级别
enum class FluentInfoLevel {
    Info,
    Success,
    Warning,
    Error
};

#endif // FLUENTGLOBAL_H

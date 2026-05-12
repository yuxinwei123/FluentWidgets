# FluentWidgets — 基于 Qt5 的 Fluent Design 组件库

## 项目概述

FluentWidgets 是一个参考 Microsoft Fluent Design System 设计规范的 Qt5 自绘组件库。所有核心组件均通过 `QPainter` 完全自绘，不依赖 QSS 或系统原生样式，可精确还原 Fluent Design 的视觉细节。

**构建要求**：Qt 5.15.2 + CMake 3.5+ + C++17

---

## 项目结构

```
CustomWindowComponents/
├── CMakeLists.txt                  # 顶层 CMake 构建入口
├── README.md                       # 本文件
│
├── src/                            # ★ 组件库源码
│   ├── CMakeLists.txt              # 组件库构建配置 (静态库 FluentWidgets)
│   ├── FluentWidgets.h             # 统一头文件，引入即可使用全部 API
│   │
│   ├── Core/                       # 核心基础设施层
│   │   ├── FluentGlobal.h          # 导出宏 + 全局枚举定义
│   │   ├── FluentConfig.h/cpp      # 全局配置单例 (动画开关/DPI/平台特性)
│   │   ├── FluentTheme.h/cpp       # 主题 Token 集合 (颜色/排版/间距)
│   │   └── FluentThemeManager.h/cpp# 主题管理器单例 (切换/分发/刷新)
│   │
│   ├── Animations/                 # 动画系统
│   │   └── FluentAnimation.h/cpp   # 缓动曲线集 + 属性动画工厂
│   │
│   ├── Effects/                    # 视觉效果层
│   │   ├── FluentAcrylicEffect.h/cpp  # 亚克力毛玻璃效果
│   │   └── FluentRevealEffect.h/cpp   # 揭示高亮效果
│   │
│   └── Widgets/                    # 自绘组件层
│       ├── Base/
│       │   └── FluentWidget.h/cpp  # 组件公共基类
│       ├── Buttons/
│       │   └── FluentButton.h/cpp  # 标准按钮 (8 种变体)
│       └── Window/
│           ├── FluentWindow.h/cpp  # 无边框窗口
│           └── FluentTitleBar.h/cpp# 自定义标题栏
│
└── demo/                           # 组件展示应用
    ├── CMakeLists.txt
    ├── main.cpp
    ├── MainWindow.h/cpp            # Demo 主窗口
```

---

## 架构分层

```
┌───────────────────────────────────────────┐
│            Application Layer              │  ← 用户应用
├───────────────────────────────────────────┤
│             Demo / Gallery                │  ← 组件展示
├───────────────────────────────────────────┤
│         Widgets (组件层)                   │  ← 对外 API
│   FluentWidget → FluentButton            │
│   FluentWindow / FluentTitleBar          │
├───────────────────────────────────────────┤
│      Effects (效果层)                      │  ← 可组合的视觉效果
│   Acrylic / Reveal / Shadow / Ripple     │
├───────────────────────────────────────────┤
│     Animations (动画层)                    │  ← 统一缓动 & 动画工厂
│   FluentAnimation                         │
├───────────────────────────────────────────┤
│     Core (核心层)                          │  ← 主题 / 配置 / 枚举
│   FluentTheme / FluentThemeManager       │
│   FluentConfig / FluentGlobal            │
└───────────────────────────────────────────┘
```

**设计原则**：
- **数据驱动主题**：所有视觉属性用 `Q_PROPERTY` 声明，组件通过 `FluentTheme` 读取值
- **完全自绘**：重写 `paintEvent`，用 `QPainter` 绘制所有状态，不硬编码颜色/尺寸
- **可组合效果**：效果通过 `attachTo()` 挂载到 Widget，可叠加
- **信号驱动刷新**：主题切换 → `themeChanged` 信号 → 组件 `update()` 重绘

---

## 核心层 (Core)

### FluentGlobal

**文件**：`src/Core/FluentGlobal.h`

全局定义文件，无类，仅包含宏和枚举。

| 定义 | 说明 |
|------|------|
| `FLUENT_EXPORT` | 导出宏。静态库构建时为空；定义 `FLUENTWIDGETS_SHARED` 时启用 `Q_DECL_EXPORT/IMPORT` |
| `FluentThemeMode` | `Light` / `Dark` / `Auto`（跟随系统） |
| `FluentButtonVariant` | `Primary` / `Secondary` / `Subtle` / `Accent` / `Transparent` / `Danger` / `Toggle` / `Split` |
| `FluentSize` | `Small` / `Medium` / `Large` |
| `FluentNavigationDisplayMode` | `Minimal` / `Compact` / `Expanded` / `Auto` |
| `FluentInfoLevel` | `Info` / `Success` / `Warning` / `Error` |

**Qt 知识点**：
- `Q_DECL_EXPORT` / `Q_DECL_IMPORT`：跨 DLL 边界的符号导出/导入
- `enum class`：C++11 强类型枚举，防止枚举值命名冲突

---

### FluentConfig

**文件**：`src/Core/FluentConfig.h` / `.cpp`

全局配置单例，管理运行时特性开关。

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `animationEnabled` | bool | true | 全局动画开关 |
| `nativeBlur` | bool | true | 是否使用系统原生模糊 API |
| `devicePixelRatio` | qreal | 只读 | 当前屏幕 DPI 缩放比 |

**类结构**：
```
FluentConfig : QObject
├── Q_OBJECT + Q_PROPERTY
├── static instance()           # 单例获取
├── animationEnabled() / setAnimationEnabled()
├── nativeBlur() / setNativeBlur()
├── devicePixelRatio()
├── signal: configChanged()
└── private: m_animationEnabled, m_nativeBlur
```

**Qt 知识点**：
- **单例模式**：`static FluentConfig& instance()`，利用静态局部变量保证线程安全初始化
- `QApplication::devicePixelRatio()`：获取设备像素比，用于高 DPI 适配
- `Q_PROPERTY`：属性系统，支持信号通知、QSS 绑定、QML 互操作

---

### FluentTheme

**文件**：`src/Core/FluentTheme.h` / `.cpp`

**核心类**。集中定义所有 Fluent Design Token（颜色/排版/间距），是整个组件库的数据中心。

#### 颜色 Token (12 个 Q_PROPERTY)

| 属性 | Light 模式 | Dark 模式 | 说明 |
|------|-----------|-----------|------|
| `primaryColor` | `#0078D4` | `#0078D4` | 主题色（可写，其他颜色只读） |
| `backgroundColor` | `#FFFFFF` | `#202020` | 窗口背景 |
| `surfaceColor` | `#F3F3F3` | `#2D2D2D` | 卡片/面板底色 |
| `cardColor` | `#FFFFFF` | `#2D2D2D` | 卡片背景 |
| `textColorPrimary` | `#1A1A1A` | `#FFFFFF` | 主要文字 |
| `textColorSecondary` | `#616161` | `#C5C5C5` | 次要文字 |
| `textColorDisabled` | `#A0A0A0` | `#5C5C5C` | 禁用文字 |
| `borderColor` | `#E0E0E0` | `#3D3D3D` | 边框 |
| `dividerColor` | `#EDEDED` | `#3D3D3D` | 分割线 |
| `errorColor` | `#D13438` | `#F47067` | 错误 |
| `successColor` | `#0F7B0F` | `#6CCB5F` | 成功 |
| `warningColor` | `#9D5D00` | `#FFC930` | 警告 |
| `infoColor` | `#0078D4` | `#4CC2FF` | 信息 |

#### 排版 Token (7 个 Q_PROPERTY)

| 属性 | 值 | 说明 |
|------|-----|------|
| `captionFont` | Segoe UI 12px | 说明文字 |
| `bodyFont` | Segoe UI 14px | 正文 |
| `bodyStrongFont` | Segoe UI 14px Bold | 正文强调 |
| `subtitleFont` | Segoe UI 20px | 副标题 |
| `titleFont` | Segoe UI 28px | 标题 |
| `titleLargeFont` | Segoe UI 40px | 大标题 |
| `displayFont` | Segoe UI 68px | 展示文字 |

#### 间距 Token (7 个 Q_PROPERTY)

| 属性 | 值 | 说明 |
|------|-----|------|
| `controlHeight` | 32px | 标准控件高度 |
| `controlHeightSmall` | 24px | 小控件高度 |
| `controlHeightLarge` | 40px | 大控件高度 |
| `cornerRadius` | 4px | 标准圆角 |
| `cornerRadiusSmall` | 2px | 小圆角 |
| `borderWidth` | 1px | 边框宽度 |
| `focusBorderWidth` | 2px | 焦点边框宽度 |

**类结构**：
```
FluentTheme : QObject
├── Q_OBJECT + 21 个 Q_PROPERTY
├── 构造: FluentTheme(FluentThemeMode mode, QObject* parent)
├── 颜色: 12 个 getter + primaryColor setter
├── 排版: 7 个 QFont getter
├── 间距: 7 个 int getter
├── themeMode() / setThemeMode() / isDark()
├── signal: themeChanged()           # 所有 Token 变更时触发
├── private: applyLightPalette() / applyDarkPalette()
└── private: m_mode, m_primaryColor, ThemeColors m_colors
```

**Qt 知识点**：
- `Q_PROPERTY` 声明 + `NOTIFY themeChanged`：属性变更通知机制，组件 connect 此信号后自动刷新
- `QColor`：Qt 颜色类，支持 `#RRGGBB` 构造和 `setAlphaF()` 设置透明度
- `QFont`：字体类，构造时指定字体族和像素大小，`setBold()` 设置粗体
- 内部用 `struct ThemeColors` 聚合所有颜色值，`applyLightPalette()` / `applyDarkPalette()` 一次性切换

---

### FluentThemeManager

**文件**：`src/Core/FluentThemeManager.h` / `.cpp`

主题管理器单例，负责主题切换和全局刷新分发。

| 属性 | 类型 | 说明 |
|------|------|------|
| `currentTheme` | FluentTheme* 只读 | 当前主题对象 |
| `themeMode` | FluentThemeMode | 读写，切换 Light/Dark |

**类结构**：
```
FluentThemeManager : QObject
├── Q_OBJECT + 2 个 Q_PROPERTY
├── static instance()               # 单例
├── currentTheme()                  # 获取当前 FluentTheme
├── themeMode() / setThemeMode()    # 切换主题
├── registerWidget() / unregisterWidget()  # 注册 Widget (扩展用)
├── signal: themeChanged()
├── private: applyThemeToWidget()   # 遍历所有 Widget 调用 update()
└── private: FluentTheme* m_theme
```

**Qt 知识点**：
- `QApplication::allWidgets()`：获取应用中所有 Widget 实例，用于全局刷新
- `QWidget::update()`：触发 Widget 重绘（不会立即绘制，加入事件队列）
- **信号链**：`FluentThemeManager::setThemeMode()` → `FluentTheme::setThemeMode()` → `FluentTheme::themeChanged` → 各组件 `update()`

---

## 动画层 (Animations)

### FluentAnimation

**文件**：`src/Animations/FluentAnimation.h` / `.cpp`

工具类（非 QObject），提供 Fluent Design 标准缓动曲线和动画创建工厂。

#### 缓动曲线

| 方法 | 曲线类型 | 用途 |
|------|---------|------|
| `decelerate()` | `OutCubic` | 进入动效（减速停止） |
| `accelerate()` | `InCubic` | 退出动效（加速离开） |
| `standard()` | BezierSpline (0.8,0)→(0.2,1)→(1,1) | Fluent 标准缓动 |
| `sharp()` | BezierSpline (0.8,0)→(0.6,1)→(1,1) | Fluent 强烈缓动 |

#### 时长常量

| 常量 | 值 (ms) | 用途 |
|------|---------|------|
| `fastDuration` | 150 | 快速过渡 |
| `normalDuration` | 250 | 标准过渡 |
| `slowDuration` | 400 | 慢速过渡 |
| `entranceDuration` | 300 | 入场动画 |
| `exitDuration` | 200 | 退场动画 |

#### 工厂方法

```cpp
static QPropertyAnimation* create(
    QObject* target,              // 动画目标对象
    const QByteArray& propertyName, // Q_PROPERTY 名称
    const QVariant& endValue,     // 目标值
    int duration = normalDuration,
    const QEasingCurve& curve = standard()
);
```

**类结构**：
```
FluentAnimation (非 QObject，纯工具类)
├── static decelerate() / accelerate() / standard() / sharp()
├── static constexpr: fastDuration / normalDuration / slowDuration / ...
└── static create()               # 创建并启动 QPropertyAnimation
```

**Qt 知识点**：
- `QEasingCurve`：缓动曲线类，`BezierSpline` 类型可通过 `addCubicBezierSegment()` 定义自定义贝塞尔曲线
- `QPropertyAnimation`：属性动画，驱动任何 `Q_PROPERTY` 从当前值到目标值的插值动画
- `QAbstractAnimation::DeleteWhenStopped`：动画结束后自动 delete，防止内存泄漏
- `QPointF`：贝塞尔控制点坐标
- `static constexpr int`：编译期常量，无运行时开销

---

## 效果层 (Effects)

### FluentAcrylicEffect

**文件**：`src/Effects/FluentAcrylicEffect.h` / `.cpp`

亚克力毛玻璃效果。实现 Fluent Design 的半透明 + 模糊 + 色彩叠加效果。

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `blurRadius` | int | 30 | 模糊半径 |
| `opacity` | qreal | 0.8 | 整体不透明度 |
| `tintColor` | QColor | 白色 | 叠加色调 |
| `tintOpacity` | qreal | 0.4 | 色调叠加不透明度 |
| `noiseOpacity` | qreal | 0.03 | 噪点层不透明度 |

**类结构**：
```
FluentAcrylicEffect : QObject
├── Q_OBJECT + 5 个 Q_PROPERTY
├── attachTo(QWidget* target)      # 挂载到目标 Widget
├── detachFrom(QWidget* target)    # 卸载
├── paint(QPainter*, const QRect&) # 在目标 paintEvent 中调用
├── signal: effectChanged()
└── private: m_blurRadius, m_opacity, m_tintColor, m_tintOpacity, m_noiseOpacity, m_target
```

**绘制流程**：
1. `QWidget::grab()` 捕获 Widget 当前画面 → `QImage`
2. `QImage::scaled()` 先缩小再放大，实现近似高斯模糊
3. `QPainter::drawImage()` 绘制模糊背景
4. `QPainter::fillRect()` 叠加半透明色彩层

**Qt 知识点**：
- `QWidget::grab()`：将 Widget 渲染为 `QPixmap`，用于 offscreen 渲染
- `QImage::scaled()` + `Qt::SmoothTransformation`：图像缩放，双线性插值
- `QPainter::fillRect()` + `QColor::setAlphaF()`：半透明色彩叠加
- `QGraphicsBlurEffect`：Qt 内置模糊效果（当前未直接使用，保留扩展）
- **性能提示**：当前 `grab()` + 软件缩放的模糊是简化实现，生产环境应使用 Windows DWM API 或 GPU Shader

---

### FluentRevealEffect

**文件**：`src/Effects/FluentRevealEffect.h` / `.cpp`

揭示高亮效果。鼠标靠近组件时，以鼠标为中心产生径向渐变发光，模拟 Fluent Design 的 Reveal Highlight。

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `revealColor` | QColor | 白色 | 高光颜色 |
| `revealRadius` | qreal | 80.0 | 高光半径 |
| `revealOpacity` | qreal | 0.15 | 高光不透明度 |

**类结构**：
```
FluentRevealEffect : QObject
├── Q_OBJECT + 3 个 Q_PROPERTY
├── attachTo(QWidget* target)       # 挂载并启用 mouseTracking
├── detachFrom(QWidget* target)
├── setMousePosition(const QPoint&) # 在 mouseMoveEvent 中更新
├── paint(QPainter*, const QRect&)  # 在 paintEvent 中调用
├── signal: effectChanged()
└── private: m_revealColor, m_revealRadius, m_revealOpacity, m_mousePos, m_target
```

**绘制流程**：
1. 以 `m_mousePos` 为中心创建 `QRadialGradient`
2. 中心颜色 = `revealColor` + `revealOpacity` 透明度
3. 边缘颜色 = `revealColor` + 0.0 透明度（完全透明）
4. `QPainter::setClipRect()` 裁剪到组件区域
5. `QPainter::drawRect()` 绘制渐变矩形

**Qt 知识点**：
- `QRadialGradient`：径向渐变，指定中心和半径，用 `setColorAt()` 定义 stops
- `QPainter::save()` / `restore()`：保存/恢复绘制状态（画笔、画刷、裁剪等）
- `QPainter::setClipRect()`：裁剪绘制区域，防止渐变溢出组件边界
- `QWidget::setMouseTracking(true)`：启用鼠标追踪，无需按下即可接收 `mouseMoveEvent`

---

## 组件层 (Widgets)

### FluentWidget（组件基类）

**文件**：`src/Widgets/Base/FluentWidget.h` / `.cpp`

所有 Fluent 自绘组件的公共基类，提供统一的主题接入、状态追踪和绘制框架。

| 属性 | 类型 | 说明 |
|------|------|------|
| `theme` | FluentTheme* | 当前主题（自动从 FluentThemeManager 获取） |

**类结构**：
```
FluentWidget : QWidget
├── Q_OBJECT + Q_PROPERTY(theme)
├── 构造: 自动从 FluentThemeManager 获取 theme 并 connect
├── theme() / setTheme()           # 读写主题，切换时自动刷新
├── paintFluent(QPainter*) = 0     # ★ 纯虚函数，子类必须实现
│
├── protected:
│   ├── paintEvent()               # 创建 QPainter → 调用 paintFluent()
│   ├── enterEvent(QEvent*)        # m_hovered = true → update()
│   ├── leaveEvent(QEvent*)        # m_hovered = false → update()
│   ├── mousePressEvent()          # m_pressed = true → update()
│   ├── mouseReleaseEvent()        # m_pressed = false → update()
│   ├── isHovered() / isPressed()  # 状态查询
│
├── signals:
│   └── themeChanged()
│
└── private: m_theme, m_hovered, m_pressed
```

**Qt 知识点**：
- **纯虚函数 `paintFluent()`**：模板方法模式，`paintEvent()` 负责创建 QPainter 并设置抗锯齿，子类只需关注绘制逻辑
- `QPainter::setRenderHint(QPainter::Antialiasing)`：启用抗锯齿，圆角/曲线必须
- `enterEvent(QEvent*)`：Qt5 中参数类型是 `QEvent*`（Qt6 改为 `QEnterEvent*`）
- `setMouseTracking(true)`：启用 hover 追踪
- `QWidget::update()`：请求重绘，Qt 会在下一次事件循环中调用 `paintEvent()`
- **信号-槽连接**：构造时 `connect(theme, &FluentTheme::themeChanged, this, [this](){ update(); emit themeChanged(); })`，主题变更自动刷新

---

### FluentButton（标准按钮）

**文件**：`src/Widgets/Buttons/FluentButton.h` / `.cpp`

支持 8 种变体的自绘按钮，完全通过 `QPainter` 绘制所有视觉状态。

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `text` | QString | 空 | 按钮文字 |
| `variant` | FluentButtonVariant | Secondary | 按钮变体 |
| `size` | FluentSize | Medium | 组件尺寸 |
| `accentColor` | QColor | theme.primaryColor | 强调色 |

#### 8 种按钮变体

| 变体 | 背景色 | 文字色 | 边框 | 典型用途 |
|------|--------|--------|------|---------|
| `Primary` | accentColor | 白色 | 无 | 主要操作 |
| `Secondary` | surfaceColor | textPrimary | 有 | 次要操作 |
| `Subtle` | 透明 | textPrimary | 无 | 轻量操作 |
| `Accent` | accentColor | 白色 | 无 | 强调操作 |
| `Transparent` | 透明 | textPrimary | 无 | 最低优先级 |
| `Danger` | errorColor | 白色 | 无 | 危险操作 |
| `Toggle` | surfaceColor | textPrimary | 无 | 切换 |
| `Split` | surfaceColor | textPrimary | 无 | 分体操作 |

**类结构**：
```
FluentButton : FluentWidget
├── Q_OBJECT + 4 个 Q_PROPERTY
├── 构造: FluentButton(text, variant, parent)
├── text() / setText()
├── variant() / setVariant()
├── size() / setSize()
├── accentColor() / setAccentColor()
├── sizeHint() / minimumSizeHint()    # 根据文字宽度计算建议尺寸
├── paintFluent(QPainter*) override   # ★ 核心绘制逻辑
│
├── signals:
│   ├── clicked()                     # 左键释放且在按钮内
│   ├── textChanged() / variantChanged() / sizeChanged() / accentColorChanged()
│
├── protected:
│   ├── mousePressEvent()             # 仅响应左键
│   ├── mouseReleaseEvent()           # 左键释放 → emit clicked()
│   ├── keyPressEvent()               # Enter/Return → emit clicked()
│   ├── focusInEvent() / focusOutEvent() # 焦点变化 → update()
│
├── private:
│   ├── setupSize()                   # 根据 FluentSize 设置固定高度
│   ├── initAnimations()              # 预留动画初始化
│   ├── backgroundColorForState()    # 根据 variant 返回背景色
│   ├── textColorForState()          # 根据 variant + enabled 返回文字色
│   ├── borderColorForState()        # 返回边框色
│   ├── m_text, m_variant, m_size, m_accentColor
│   ├── m_hoverAnim, m_hoverProgress  # 预留 hover 动画
```

#### paintFluent() 绘制流程

1. **背景**：`QPainterPath::addRoundedRect()` + `drawPath()` 绘制圆角矩形背景
2. **边框**：Secondary 变体且无焦点时绘制 1px 边框
3. **焦点边框**：`hasFocus()` 时绘制 2px primaryColor 内缩边框
4. **Hover 叠加**：`isHovered() && !isPressed()` 时叠加 6% 透明度的 textPrimary
5. **Pressed 叠加**：`isPressed()` 时叠加 10% 透明度的 textPrimary
6. **文字**：`drawText()` 居中绘制

**Qt 知识点**：
- `QPainterPath::addRoundedRect()`：创建圆角矩形路径，用于绘制和裁剪
- `QPainter::setPen()` / `setBrush()`：设置描边和填充
- `Qt::NoPen`：无描边，用于纯填充绘制
- `QColor::setAlphaF(qreal)`：设置 0.0~1.0 的浮点透明度
- `QFontMetrics::horizontalAdvance()`：计算文字像素宽度，用于 `sizeHint()`
- `setCursor(Qt::PointingHandCursor)`：设置手型光标
- `setFocusPolicy(Qt::StrongFocus)`：支持 Tab 键和点击获取焦点
- `QKeyEvent::key()`：键盘事件，检测 Enter/Return 键
- `QFocusEvent`：焦点进出事件，用于绘制焦点边框

---

### FluentWindow（无边框窗口）

**文件**：`src/Widgets/Window/FluentWindow.h` / `.cpp`

无边框窗口基类，实现自定义标题栏、窗口拖拽、边框缩放和 Mica/Acrylic 效果。

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `micaEnabled` | bool | false | Windows 11 Mica 背景效果 |
| `acrylicEnabled` | bool | false | 亚克力标题栏效果 |

**类结构**：
```
FluentWindow : QWidget
├── Q_OBJECT + 2 个 Q_PROPERTY
├── 构造: 自动创建 FluentTitleBar + QVBoxLayout
├── titleBar()                      # 获取标题栏
├── setCentralWidget(QWidget*)      # 设置中央内容区
├── micaEnabled() / setMicaEnabled()
├── acrylicEnabled() / setAcrylicEnabled()
├── setWindowTitle(const QString&)  # 同步更新标题栏文字
│
├── protected:
│   ├── resizeEvent()               # 窗口大小变化
│   ├── nativeEvent()               # ★ 处理 Windows 原生消息
│   └── hitTestWindows()            # WM_NCHITTEST 命中测试
│
├── signals: micaChanged() / acrylicChanged()
│
├── private:
│   ├── setupUI()                   # FramelessWindowHint + 半透明背景
│   ├── applyMicaEffect()           # DwmSetWindowAttribute(DWMWA_SYSTEMBACKDROP_TYPE)
│   ├── removeMicaEffect()
│   ├── m_titleBar, m_centralWidget, m_micaEnabled, m_acrylicEnabled
```

#### WM_NCHITTEST 处理逻辑

Windows 系统通过 `WM_NCHITTEST` 消息判断鼠标位于窗口的哪个区域：
- 标题栏区域 → `HTCAPTION`（允许拖拽）
- 边框 6px 范围 → `HTLEFT/HTRIGHT/HTTOP/HTBOTTOM/HTTOPLEFT/...`（允许缩放）
- 其他区域 → 不拦截，交给 Qt 默认处理

**Qt 知识点**：
- `Qt::FramelessWindowHint`：移除系统原生标题栏和边框
- `Qt::WA_TranslucentBackground`：允许窗口背景透明，为 Mica/Acrylic 效果准备
- `nativeEvent(const QByteArray&, void* message, long* result)`：**Qt5 签名**，拦截操作系统原生消息（Qt6 改为 `qintptr*`）
- `MSG` 结构体：Windows 消息结构，`msg->message` 判断消息类型，`msg->lParam` 携带参数
- `GET_X_LPARAM` / `GET_Y_LPARAM`：从 `lParam` 中安全提取坐标（`<windowsx.h>`），支持多屏/高 DPI
- `DwmSetWindowAttribute()` + `DWMWA_SYSTEMBACKDROP_TYPE(38)`：Windows 11 22H2+ 的 Mica/Acrylic API
- `reinterpret_cast<HWND>(winId())`：Qt 窗口句柄转换为 Windows HWND
- `#pragma comment(lib, "dwmapi.lib")`：MSVC 链接指令，MinGW 下通过 CMake `target_link_libraries` 链接

---

### FluentTitleBar（自定义标题栏）

**文件**：`src/Widgets/Window/FluentTitleBar.h` / `.cpp`

自定义窗口标题栏，包含标题文字和最小化/最大化/关闭按钮。

**类结构**：
```
FluentTitleBar : QWidget
├── Q_OBJECT
├── 构造: 创建 QHBoxLayout + QLabel + 3 个 QPushButton
├── setTitle(const QString&)
├── setIcon(const QIcon&)           # 预留
├── titleBarHeight()                # 返回 32px
│
├── signals:
│   ├── minimizeClicked()
│   ├── maximizeClicked()
│   └── closeClicked()
│
├── protected:
│   ├── paintEvent()                # 填充 backgroundColor
│   └── mouseDoubleClickEvent()     # 双击 → maximizeClicked()
│
└── private:
    ├── m_titleLabel (QLabel*)
    ├── m_minBtn / m_maxBtn / m_closeBtn (QPushButton*)
    └── TITLE_BAR_HEIGHT = 32
```

**Qt 知识点**：
- `QHBoxLayout`：水平布局，依次排列标题、弹性空间、窗口按钮
- `addStretch()`：添加弹性空间，将按钮推到右侧
- `setObjectName()`：为 QSS 选择器提供标识
- `setAttribute(Qt::WA_StyledBackground, true)`：允许 QWidget 通过 QSS 设置背景色（默认 QWidget 不支持）
- `QPainter::fillRect()`：在 `paintEvent` 中填充背景色

---

## Demo 展示应用

**文件**：`demo/main.cpp` / `demo/MainWindow.h` / `demo/MainWindow.cpp`

展示组件库用法的小应用。继承 `FluentWindow`，在中央区域放置各种 `FluentButton` 变体，点击按钮更新状态标签。

**关键代码**：
```cpp
// 创建各种变体按钮
auto* primaryBtn = new FluentButton(u8"Primary", FluentButtonVariant::Primary, page);
auto* secondaryBtn = new FluentButton(u8"Secondary", FluentButtonVariant::Secondary, page);
auto* subtleBtn = new FluentButton(u8"Subtle", FluentButtonVariant::Subtle, page);
auto* dangerBtn = new FluentButton(u8"Danger", FluentButtonVariant::Danger, page);

// 连接信号
connect(primaryBtn, &FluentButton::clicked, this, [statusLabel]() {
    statusLabel->setText(u8"Primary 按钮被点击");
});
```

**Qt 知识点**：
- `FluentThemeManager::instance().currentTheme()`：在非 FluentWidget 中获取主题
- Lambda 表达式作为槽函数：`connect(sender, &Signal, receiver, [capture]() { ... })`

---

## 构建说明

### 前置条件

- Qt 5.15.2（MSVC 2019 或 MinGW 8.1）
- CMake 3.5+
- C++17 编译器

### 构建步骤

```bash
# 配置（指定 Qt 安装路径）
cmake -B build -DCMAKE_PREFIX_PATH=C:/Qt/5.15.2/msvc2019_64

# 编译
cmake --build build --config Release

# 运行 Demo
./build/demo/FluentWidgetsDemo.exe
```

### CMake 目标

| 目标 | 类型 | 说明 |
|------|------|------|
| `FluentWidgets` | 静态库 | 组件库，链接到 `Qt5::Core Qt5::Gui Qt5::Widgets` + `dwmapi user32`(Win) |
| `FluentWidgetsDemo` | 可执行文件 | Demo 应用，链接到 `FluentWidgets` |

---

## Qt 知识点速查表

| 知识点 | 使用位置 | 说明 |
|--------|---------|------|
| `Q_OBJECT` 宏 | 所有 QObject 子类 | 启用信号槽、Q_PROPERTY、`qobject_cast` |
| `Q_PROPERTY` | FluentTheme(21), FluentWidget(1), FluentButton(4), ... | 属性系统，支持信号通知、QSS 绑定 |
| `QPainter` 自绘 | FluentWidget, FluentButton, FluentTitleBar | 完全自绘组件的核心 |
| `QPainterPath::addRoundedRect()` | FluentButton | 绘制圆角矩形 |
| `QRadialGradient` | FluentRevealEffect | 径向渐变，Reveal 高光 |
| `QPropertyAnimation` | FluentAnimation, FluentButton | 属性动画，驱动 Q_PROPERTY 插值 |
| `QEasingCurve` | FluentAnimation | 缓动曲线，`BezierSpline` 自定义贝塞尔 |
| `QWidget::grab()` | FluentAcrylicEffect | 捕获 Widget 画面为 QPixmap |
| `nativeEvent()` | FluentWindow | 拦截 Windows 原生消息，Qt5 签名 `long*` |
| `Qt::FramelessWindowHint` | FluentWindow | 无边框窗口 |
| `Qt::WA_TranslucentBackground` | FluentWindow | 窗口透明背景 |
| `setMouseTracking(true)` | FluentWidget, FluentRevealEffect | 无需按下即可追踪鼠标 |
| `enterEvent(QEvent*)` / `leaveEvent()` | FluentWidget | Qt5 签名（Qt6 为 QEnterEvent*） |
| `QWidget::update()` | 多处 | 请求异步重绘 |
| `QApplication::allWidgets()` | FluentThemeManager | 遍历所有 Widget 进行主题刷新 |
| `DwmSetWindowAttribute()` | FluentWindow | Windows 11 Mica/Acrylic 效果 API |
| `FLUENT_EXPORT` 宏 | 所有公开类 | 静态库为空；动态库时 `Q_DECL_EXPORT/IMPORT` |
| 单例模式 | FluentConfig, FluentThemeManager | `static T& instance()` 利用静态局部变量 |
| 模板方法模式 | FluentWidget | `paintEvent()` 调用纯虚 `paintFluent()` |
| 观察者模式 | 主题系统 | `FluentTheme::themeChanged` → 各组件 `update()` |

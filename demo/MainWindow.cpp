#include "MainWindow.h"
#include "Core/FluentThemeManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>
#include <QStandardItemModel>

#include <QIcon>
#include <QPixmap>
#include <QPainter>

DemoMainWindow::DemoMainWindow(QWidget* parent)
    : FluentWindow(parent)
{
    setWindowTitle(u8"FluentWidgets Demo");
    updateWindowIcon();
    resize(1000, 650);

    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [this]() {
        updateWindowIcon();
    });

    setupNavigation();
}

void DemoMainWindow::setupNavigation() {
    m_navView = new FluentNavigationView(this);

    // === 添加导航项和页面 ===

    // 首页
    auto* homeItem = m_navView->addItem(u8"首页", QIcon(), "home");
    m_navView->addPage("home", createHomePage());

    // 组件 (有子菜单)
    auto* componentsItem = m_navView->addItem(u8"组件", QIcon(), "components");
    m_navView->addSubItem(componentsItem, u8"按钮 & 开关", QIcon(), "buttons");
    m_navView->addSubItem(componentsItem, u8"输入框 Input", QIcon(), "inputs");
    m_navView->addSubItem(componentsItem, u8"对话框 Dialog", QIcon(), "dialogs");
    m_navView->addSubItem(componentsItem, u8"抽屉 Drawer", QIcon(), "drawers");
    m_navView->addSubItem(componentsItem, u8"表格 Table", QIcon(), "tables");
    m_navView->addSubItem(componentsItem, u8"树 Tree", QIcon(), "trees");
    m_navView->addSubItem(componentsItem, u8"节点图 Graph", QIcon(), "graphs");
    m_navView->addSubItem(componentsItem, u8"导航栏 NavBar", QIcon(), "navbars");
    m_navView->addSubItem(componentsItem, u8"日历 Calendar", QIcon(), "calendars");
    m_navView->addSubItem(componentsItem, u8"轮播图 Carousel", QIcon(), "carousels");
    m_navView->addSubItem(componentsItem, u8"卡片 Card", QIcon(), "cards");
    m_navView->addSubItem(componentsItem, u8"进度条 ProgressBar", QIcon(), "progressbars");
    m_navView->addSubItem(componentsItem, u8"进度环 ProgressRing", QIcon(), "progressrings");
    m_navView->addSubItem(componentsItem, u8"线图 LineChart", QIcon(), "linecharts");
    m_navView->addSubItem(componentsItem, u8"柱状图 BarChart", QIcon(), "barcharts");
    m_navView->addSubItem(componentsItem, u8"历程图 Timeline", QIcon(), "timelines");
    m_navView->addSubItem(componentsItem, u8"饼图 PieChart", QIcon(), "piecharts");
    m_navView->addSubItem(componentsItem, u8"滑块 Slider", QIcon(), "sliders");
    m_navView->addSubItem(componentsItem, u8"日期选择 DatePicker", QIcon(), "datepickers");
    m_navView->addSubItem(componentsItem, u8"时间选择 TimePicker", QIcon(), "timepickers");
    m_navView->addSubItem(componentsItem, u8"颜色选择 ColorPicker", QIcon(), "colorpickers");
    m_navView->addPage("buttons", createButtonPage());
    m_navView->addPage("inputs", createInputPage());
    m_navView->addPage("dialogs", createDialogPage());
    m_navView->addPage("drawers", createDrawerPage());
    m_navView->addPage("tables", createTablePage());
    m_navView->addPage("trees", createTreePage());
    m_navView->addPage("graphs", createGraphPage());
    m_navView->addPage("navbars", createNavBarPage());
    m_navView->addPage("calendars", createCalendarPage());
    m_navView->addPage("carousels", createCarouselPage());
    m_navView->addPage("cards", createCardPage());
    m_navView->addPage("progressbars", createProgressBarPage());
    m_navView->addPage("progressrings", createProgressRingPage());
    m_navView->addPage("linecharts", createLineChartPage());
    m_navView->addPage("barcharts", createBarChartPage());
    m_navView->addPage("timelines", createTimelinePage());
    m_navView->addPage("piecharts", createPieChartPage());
    m_navView->addPage("sliders", createSliderPage());
    m_navView->addPage("datepickers", createDatePickerPage());
    m_navView->addPage("timepickers", createTimePickerPage());
    m_navView->addPage("colorpickers", createColorPickerPage());

    // 设置
    auto* settingsItem = m_navView->addItem(u8"设置", QIcon(), "settings_group");
    m_navView->addSubItem(settingsItem, u8"常规", QIcon(), "settings_general");
    m_navView->addSubItem(settingsItem, u8"关于", QIcon(), "about");
    m_navView->addPage("settings_general", createSettingsPage());
    m_navView->addPage("about", createAboutPage());

    // 设为中心控件
    setCentralWidget(m_navView);

    // 默认选中首页
    m_navView->setCurrentPage("home");
}

QWidget* DemoMainWindow::createHomePage() {
    auto* page = new QWidget();
    auto* scrollArea = new QScrollArea(page);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto* container = new QWidget();
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    auto* theme = FluentThemeManager::instance().currentTheme();

    // === 顶部标题区 ===
    auto* iconLabel = new QLabel(container);
    auto updateIconPixmap = [iconLabel]() {
        bool isDark = FluentThemeManager::instance().currentTheme()->isDark();
        QPixmap pix(isDark ? ":/icons/fluent_dark.svg" : ":/icons/fluent.svg");
        iconLabel->setPixmap(pix.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    };
    updateIconPixmap();
    iconLabel->setFixedSize(48, 48);
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, iconLabel, updateIconPixmap);

    auto* titleLabel = new QLabel(u8"FluentWidgets", container);
    titleLabel->setFont(theme->titleLargeFont());
    titleLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [titleLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        titleLabel->setFont(t->titleLargeFont());
        titleLabel->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });

    auto* verLabel = new QLabel(u8"v1.0", container);
    verLabel->setFont(theme->captionFont());
    verLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [verLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        verLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });

    auto* titleRow = new QHBoxLayout();
    titleRow->setSpacing(12);
    titleRow->addWidget(iconLabel);
    titleRow->addWidget(titleLabel);
    titleRow->addWidget(verLabel);
    titleRow->addStretch();
    layout->addLayout(titleRow);

    auto* descLabel = new QLabel(u8"基于 Qt 的 Fluent Design 风格组件库 — 优雅、现代、开箱即用", container);
    descLabel->setFont(theme->bodyFont());
    descLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [descLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        descLabel->setFont(t->bodyFont());
        descLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(descLabel);

    // === 轮播图特性展示 ===
    auto* carousel = new FluentCarousel(container);
    carousel->setFixedHeight(200);
    carousel->addSlide(QColor("#0078D4"), u8"🎨 丰富的组件库\n按钮、输入框、对话框、表格、树、日历...");
    carousel->addSlide(QColor("#10896C"), u8"🌙 深色 / 浅色主题\n一键切换，圆形扩散动画过渡");
    carousel->addSlide(QColor("#DA3B3B"), u8"📊 数据可视化\n折线图、曲线图、散点图、柱状图");
    carousel->addSlide(QColor("#FF8C00"), u8"🎠 交互组件\n轮播图、进度条、悬浮窗口、导航栏");
    carousel->addSlide(QColor("#744DA9"), u8"🪟 窗口系统\nMica 材质、亚克力模糊、自定义标题栏");
    carousel->setAutoPlay(true);
    carousel->setInterval(4000);
    carousel->setBorderRadius(12);
    layout->addWidget(carousel);

    // === 快捷操作栏 ===
    auto* actionRow = new QHBoxLayout();
    actionRow->setSpacing(10);

    auto* themeBtn = new FluentButton(u8"切换主题", FluentButtonVariant::Accent, container);
    connect(themeBtn, &FluentButton::clicked, this, &DemoMainWindow::toggleTheme);

    m_themeLabel = new QLabel(container);
    m_themeLabel->setFont(theme->captionFont());
    auto updateLabel = [this]() {
        bool isDark = FluentThemeManager::instance().currentTheme()->isDark();
        m_themeLabel->setText(isDark ? u8"Dark" : u8"Light");
        m_themeLabel->setStyleSheet(
            QString("color: %1;").arg(FluentThemeManager::instance().currentTheme()->textColorPrimary().name()));
    };
    updateLabel();
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, updateLabel);

    actionRow->addWidget(themeBtn);
    actionRow->addWidget(m_themeLabel);
    actionRow->addStretch();
    layout->addLayout(actionRow);

    // === 组件快捷入口 ===
    auto* sectionTitle = new QLabel(u8"组件导航", container);
    sectionTitle->setFont(theme->subtitleFont());
    sectionTitle->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [sectionTitle]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        sectionTitle->setFont(t->subtitleFont());
        sectionTitle->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(sectionTitle);

    // 网格布局：4列
    auto* grid = new QGridLayout();
    grid->setSpacing(12);

    // 导航项定义: { 显示名, 页面ID, 颜色, 描述 }
    struct NavItem {
        QString name;
        QString pageId;
        QColor color;
        QString desc;
    };
    QVector<NavItem> navItems = {
        {u8"按钮 & 开关",  "buttons",       QColor("#0078D4"), u8"Primary / Secondary / Toggle"},
        {u8"输入框",       "inputs",        QColor("#10896C"), u8"LineEdit / CheckBox / ComboBox"},
        {u8"对话框",       "dialogs",       QColor("#DA3B3B"), u8"ContentDialog / Popover"},
        {u8"抽屉",         "drawers",       QColor("#8E44AD"), u8"左/右/上/下滑入 / 遮罩"},
        {u8"表格",         "tables",        QColor("#FF8C00"), u8"数据表格展示与编辑"},
        {u8"树",           "trees",         QColor("#744DA9"), u8"多层级树形结构"},
        {u8"节点图",       "graphs",        QColor("#0099BC"), u8"可拖拽节点编辑器"},
        {u8"导航栏",       "navbars",       QColor("#E67E22"), u8"顶部/侧边导航"},
        {u8"日历",         "calendars",     QColor("#8E44AD"), u8"日期选择 / 范围选择"},
        {u8"轮播图",       "carousels",     QColor("#0078D4"), u8"自动播放 / 指示器 / 动画"},
        {u8"卡片",         "cards",         QColor("#16A085"), u8"标题 / 封面 / 悬停提升"},
        {u8"进度条",       "progressbars",  QColor("#10896C"), u8"线形 / 环形 / 不确定"},
        {u8"进度环",       "progressrings", QColor("#1ABC9C"), u8"确定 / 旋转 / 渐变"},
        {u8"线图",         "linecharts",    QColor("#DA3B3B"), u8"折线 / 曲线 / 散点"},
        {u8"柱状图",       "barcharts",     QColor("#FF8C00"), u8"分组 / 堆叠 / 水平"},
        {u8"历程图",       "timelines",     QColor("#744DA9"), u8"框选 / 方向切换 / 多色"},
        {u8"饼图",         "piecharts",     QColor("#0099BC"), u8"饼图 / 环形 / 悬停突出"},
        {u8"滑块",         "sliders",       QColor("#E67E22"), u8"单值 / 范围 / 刻度"},
        {u8"日期选择",     "datepickers",   QColor("#2ECC71"), u8"弹出日历 / 格式化"},
        {u8"时间选择",     "timepickers",   QColor("#0099BC"), u8"滚轮 / 12-24小时制"},
        {u8"颜色选择",     "colorpickers",  QColor("#E74C3C"), u8"预设色板 / 透明度 / HEX"},
    };

    for (int i = 0; i < navItems.size(); ++i) {
        const auto& item = navItems[i];
        int row = i / 4;
        int col = i % 4;

        // 卡片容器
        auto* card = new QWidget(container);
        card->setCursor(Qt::PointingHandCursor);
        card->setFixedHeight(80);

        auto* cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(14, 10, 14, 10);
        cardLayout->setSpacing(4);

        // 色条 + 标题行
        auto* headerRow = new QHBoxLayout();
        headerRow->setSpacing(8);

        auto* dot = new QLabel(card);
        dot->setFixedSize(8, 8);
        dot->setStyleSheet(QString("background-color: %1; border-radius: 4px;").arg(item.color.name()));

        auto* nameLabel = new QLabel(item.name, card);
        nameLabel->setFont(theme->bodyStrongFont());
        nameLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));

        headerRow->addWidget(dot);
        headerRow->addWidget(nameLabel);
        headerRow->addStretch();
        cardLayout->addLayout(headerRow);

        // 描述
        auto* descCardLabel = new QLabel(item.desc, card);
        descCardLabel->setFont(theme->captionFont());
        descCardLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
        cardLayout->addWidget(descCardLabel);

        // 卡片样式（动态更新主题色）
        auto updateCardStyle = [card]() {
            auto* t = FluentThemeManager::instance().currentTheme();
            card->setStyleSheet(QString(
                "QWidget {"
                "  background-color: %1;"
                "  border: 1px solid %2;"
                "  border-radius: 8px;"
                "}"
                "QWidget:hover {"
                "  background-color: %3;"
                "  border-color: %4;"
                "}"
            ).arg(t->cardColor().name(),
                  t->dividerColor().name(),
                  t->surfaceColor().name(),
                  t->primaryColor().name()));
        };
        updateCardStyle();
        connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, card, updateCardStyle);

        // 标题和描述也需要随主题更新
        auto updateLabelStyle = [nameLabel, descCardLabel]() {
            auto* t = FluentThemeManager::instance().currentTheme();
            nameLabel->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
            descCardLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
        };
        connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, card, updateLabelStyle);

        // 点击跳转（通过 eventFilter）
        m_cardPageMap[card] = item.pageId;
        card->installEventFilter(this);

        grid->addWidget(card, row, col);
    }

    layout->addLayout(grid);

    layout->addStretch();

    scrollArea->setWidget(container);

    auto* outerLayout = new QVBoxLayout(page);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scrollArea);

    return page;
}

QWidget* DemoMainWindow::createButtonPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"Button 按钮", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    auto* btnRow = new QHBoxLayout();
    btnRow->setSpacing(12);

    auto* primaryBtn = new FluentButton(u8"Primary", FluentButtonVariant::Primary, page);
    auto* secondaryBtn = new FluentButton(u8"Secondary", FluentButtonVariant::Secondary, page);
    auto* subtleBtn = new FluentButton(u8"Subtle", FluentButtonVariant::Subtle, page);
    auto* dangerBtn = new FluentButton(u8"Danger", FluentButtonVariant::Danger, page);

    btnRow->addWidget(primaryBtn);
    btnRow->addWidget(secondaryBtn);
    btnRow->addWidget(subtleBtn);
    btnRow->addWidget(dangerBtn);
    btnRow->addStretch();

    layout->addLayout(btnRow);

    auto* statusLabel = new QLabel(u8"点击按钮查看效果", page);
    statusLabel->setFont(theme->captionFont());
    statusLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [statusLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        statusLabel->setFont(t->captionFont());
        statusLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(primaryBtn, &FluentButton::clicked, this, [statusLabel]() {
        statusLabel->setText(u8"Primary 按钮被点击");
    });
    connect(secondaryBtn, &FluentButton::clicked, this, [statusLabel]() {
        statusLabel->setText(u8"Secondary 按钮被点击");
    });
    connect(subtleBtn, &FluentButton::clicked, this, [statusLabel]() {
        statusLabel->setText(u8"Subtle 按钮被点击");
    });
    connect(dangerBtn, &FluentButton::clicked, this, [statusLabel]() {
        statusLabel->setText(u8"Danger 按钮被点击");
    });
    layout->addWidget(statusLabel);

    // 复选框
    auto* cbLabel = new QLabel(u8"CheckBox 复选框", page);
    cbLabel->setFont(theme->subtitleFont());
    cbLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [cbLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        cbLabel->setFont(t->subtitleFont());
        cbLabel->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(cbLabel);

    auto* cbRow = new QHBoxLayout();
    cbRow->setSpacing(16);

    auto* cb1 = new FluentCheckBox(u8"选项一", page);
    auto* cb2 = new FluentCheckBox(u8"选项二", page);
    cb2->setChecked(true);
    auto* cb3 = new FluentCheckBox(u8"三态复选框", page);
    cb3->setTristate(true);
    cb3->setCheckState(Qt::PartiallyChecked);

    cbRow->addWidget(cb1);
    cbRow->addWidget(cb2);
    cbRow->addWidget(cb3);
    cbRow->addStretch();
    layout->addLayout(cbRow);

    // 切换开关
    auto* swLabel = new QLabel(u8"Toggle Switch 切换开关", page);
    swLabel->setFont(theme->subtitleFont());
    swLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [swLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        swLabel->setFont(t->subtitleFont());
        swLabel->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(swLabel);

    auto* swRow = new QHBoxLayout();
    swRow->setSpacing(16);

    auto* sw1 = new FluentToggleSwitch(u8"蓝牙", page);
    auto* sw2 = new FluentToggleSwitch(u8"WiFi", page);
    sw2->setChecked(true);
    auto* sw3 = new FluentToggleSwitch(u8"飞行模式", page);

    swRow->addWidget(sw1);
    swRow->addWidget(sw2);
    swRow->addWidget(sw3);
    swRow->addStretch();
    layout->addLayout(swRow);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createInputPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"Input 输入框", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // 标准输入框
    auto* normalLabel = new QLabel(u8"标准输入框", page);
    normalLabel->setFont(theme->captionFont());
    normalLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [normalLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        normalLabel->setFont(t->captionFont());
        normalLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(normalLabel);

    auto* normalInput = new FluentLineEdit(QString(), page);
    normalInput->setPlaceholder(u8"请输入内容...");
    layout->addWidget(normalInput);

    // 密码输入框
    auto* pwdLabel = new QLabel(u8"密码输入框", page);
    pwdLabel->setFont(theme->captionFont());
    pwdLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [pwdLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        pwdLabel->setFont(t->captionFont());
        pwdLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(pwdLabel);

    auto* pwdInput = new FluentLineEdit(QString(), page);
    pwdInput->setPlaceholder(u8"请输入密码...");
    pwdInput->setPasswordMode(true);
    layout->addWidget(pwdInput);

    // 错误状态输入框
    auto* errLabel = new QLabel(u8"错误状态输入框", page);
    errLabel->setFont(theme->captionFont());
    errLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [errLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        errLabel->setFont(t->captionFont());
        errLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(errLabel);

    auto* errInput = new FluentLineEdit(u8"错误内容", page);
    errInput->setShowError(true);
    errInput->setShowError(true);
    errInput->setErrorMessage(u8"此字段不能为空");
    layout->addWidget(errInput);

    // 输入内容回显
    auto* echoLabel = new QLabel(u8"输入内容: ", page);
    echoLabel->setFont(theme->bodyFont());
    echoLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [echoLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        echoLabel->setFont(t->bodyFont());
        echoLabel->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    connect(normalInput, &FluentLineEdit::textChanged, this, [echoLabel](const QString& text) {
        echoLabel->setText(QString(u8"输入内容: %1").arg(text));
    });
    layout->addWidget(echoLabel);

    // SpinBox 数值调节
    auto* spinLabel = new QLabel(u8"SpinBox 数值调节", page);
    spinLabel->setFont(theme->subtitleFont());
    spinLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [spinLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        spinLabel->setFont(t->subtitleFont());
        spinLabel->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(spinLabel);

    auto* spinRow = new QHBoxLayout();
    spinRow->setSpacing(16);

    auto* intSpin = new FluentSpinBox(page);
    intSpin->setRange(0, 100);
    intSpin->setIntValue(42);
    intSpin->setSingleStep(1);

    auto* doubleSpin = new FluentSpinBox(page);
    doubleSpin->setRange(0.0, 10.0);
    doubleSpin->setDoubleValue(3.14);
    doubleSpin->setDecimals(2);
    doubleSpin->setSingleStep(0.1);

    spinRow->addWidget(intSpin);
    spinRow->addWidget(doubleSpin);
    spinRow->addStretch();
    layout->addLayout(spinRow);

    auto* spinStatus = new QLabel(u8"整数: 42 | 浮点: 3.14", page);
    spinStatus->setFont(theme->captionFont());
    spinStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [spinStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        spinStatus->setFont(t->captionFont());
        spinStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(intSpin, QOverload<int>::of(&FluentSpinBox::valueChanged), this, [spinStatus, doubleSpin, intSpin](int val) {
        Q_UNUSED(val);
        spinStatus->setText(QString(u8"整数: %1 | 浮点: %2").arg(intSpin->intValue()).arg(doubleSpin->doubleValue()));
    });
    connect(doubleSpin, QOverload<int>::of(&FluentSpinBox::valueChanged), this, [spinStatus, doubleSpin, intSpin](int val) {
        Q_UNUSED(val);
        spinStatus->setText(QString(u8"整数: %1 | 浮点: %2").arg(intSpin->intValue()).arg(doubleSpin->doubleValue()));
    });
    layout->addWidget(spinStatus);

    // ComboBox 下拉选择
    auto* comboLabel = new QLabel(u8"ComboBox 下拉选择", page);
    comboLabel->setFont(theme->subtitleFont());
    comboLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [comboLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        comboLabel->setFont(t->subtitleFont());
        comboLabel->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(comboLabel);

    auto* comboRow = new QHBoxLayout();
    comboRow->setSpacing(16);

    auto* normalCombo = new FluentComboBox(page);
    normalCombo->addItems({u8"北京", u8"上海", u8"深圳", u8"杭州", u8"成都", u8"广州", u8"南京"});
    normalCombo->setCurrentIndex(0);

    auto* editableCombo = new FluentComboBox(page);
    editableCombo->setEditable(true);
    editableCombo->setPlaceholder(u8"请选择或输入...");
    editableCombo->addItems({u8"工程师", u8"设计师", u8"产品经理", u8"架构师", u8"测试工程师"});

    comboRow->addWidget(normalCombo);
    comboRow->addWidget(editableCombo);
    comboRow->addStretch();
    layout->addLayout(comboRow);

    auto* comboStatus = new QLabel(u8"城市: 北京 | 职位: 未选择", page);
    comboStatus->setFont(theme->captionFont());
    comboStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [comboStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        comboStatus->setFont(t->captionFont());
        comboStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(normalCombo, &FluentComboBox::currentIndexChanged, this, [comboStatus, normalCombo, editableCombo](int idx) {
        Q_UNUSED(idx);
        comboStatus->setText(QString(u8"城市: %1 | 职位: %2")
            .arg(normalCombo->currentText())
            .arg(editableCombo->currentText().isEmpty() ? u8"未选择" : editableCombo->currentText()));
    });
    connect(editableCombo, &FluentComboBox::currentIndexChanged, this, [comboStatus, normalCombo, editableCombo](int idx) {
        Q_UNUSED(idx);
        comboStatus->setText(QString(u8"城市: %1 | 职位: %2")
            .arg(normalCombo->currentText())
            .arg(editableCombo->currentText().isEmpty() ? u8"未选择" : editableCombo->currentText()));
    });
    layout->addWidget(comboStatus);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createDialogPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"Dialog 对话框", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    auto* desc = new QLabel(u8"点击按钮弹出不同类型的对话框", page);
    desc->setFont(theme->bodyFont());
    desc->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [desc]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        desc->setFont(t->bodyFont());
        desc->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(desc);

    auto* btnRow = new QHBoxLayout();
    btnRow->setSpacing(8);

    auto* centerBtn = new FluentButton(u8"居中", FluentButtonVariant::Primary, page);
    connect(centerBtn, &FluentButton::clicked, this, [this]() {
        auto dlg = new FluentContentDialog(this);
        dlg->setTitle(u8"提示");
        dlg->setContent(u8"这是居中弹出的对话框");
        dlg->setDialogType(FluentContentDialog::DialogType::Info);
        dlg->setPopupPosition(FluentContentDialog::PopupPosition::Center);
        dlg->setDialogSize(240, 100);
        dlg->setNegativeButton(u8"取消");
        dlg->exec();
        dlg->deleteLater();
    });

    auto* leftBtn = new FluentButton(u8"左侧居中", FluentButtonVariant::Secondary, page);
    connect(leftBtn, &FluentButton::clicked, this, [this]() {
        auto dlg = new FluentContentDialog(this);
        dlg->setTitle(u8"左侧");
        dlg->setContent(u8"左侧垂直居中弹出");
        dlg->setDialogType(FluentContentDialog::DialogType::Info);
        dlg->setPopupPosition(FluentContentDialog::PopupPosition::LeftCenter);
        dlg->setDialogSize(240, 100);
        dlg->exec();
        dlg->deleteLater();
    });

    auto* rightBtn = new FluentButton(u8"右侧居中", FluentButtonVariant::Secondary, page);
    connect(rightBtn, &FluentButton::clicked, this, [this]() {
        auto dlg = new FluentContentDialog(this);
        dlg->setTitle(u8"右侧");
        dlg->setContent(u8"右侧垂直居中弹出");
        dlg->setDialogType(FluentContentDialog::DialogType::Info);
        dlg->setPopupPosition(FluentContentDialog::PopupPosition::RightCenter);
        dlg->setDialogSize(240, 100);
        dlg->exec();
        dlg->deleteLater();
    });

    auto* topBtn = new FluentButton(u8"顶部居中", FluentButtonVariant::Secondary, page);
    connect(topBtn, &FluentButton::clicked, this, [this]() {
        auto dlg = new FluentContentDialog(this);
        dlg->setTitle(u8"顶部");
        dlg->setContent(u8"顶部居中弹出");
        dlg->setDialogType(FluentContentDialog::DialogType::Warning);
        dlg->setPopupPosition(FluentContentDialog::PopupPosition::TopCenter);
        dlg->setDialogSize(240, 100);
        dlg->exec();
        dlg->deleteLater();
    });

    btnRow->addWidget(centerBtn);
    btnRow->addWidget(leftBtn);
    btnRow->addWidget(rightBtn);
    btnRow->addWidget(topBtn);
    btnRow->addStretch();

    layout->addLayout(btnRow);

    auto* btnRow2 = new QHBoxLayout();
    btnRow2->setSpacing(8);

    auto* errBtn = new FluentButton(u8"错误(右下)", FluentButtonVariant::Danger, page);
    connect(errBtn, &FluentButton::clicked, this, [this]() {
        auto dlg = new FluentContentDialog(this);
        dlg->setTitle(u8"错误");
        dlg->setContent(u8"操作失败，请重试");
        dlg->setDialogType(FluentContentDialog::DialogType::Error);
        dlg->setPopupPosition(FluentContentDialog::PopupPosition::BottomRight);
        dlg->setDialogSize(240, 100);
        dlg->setPositiveButton(u8"重试");
        dlg->setNegativeButton(u8"关闭");
        dlg->exec();
        dlg->deleteLater();
    });

    auto* succBtn = new FluentButton(u8"成功(左上)", FluentButtonVariant::Accent, page);
    connect(succBtn, &FluentButton::clicked, this, [this]() {
        auto dlg = new FluentContentDialog(this);
        dlg->setTitle(u8"成功");
        dlg->setContent(u8"操作已完成");
        dlg->setDialogType(FluentContentDialog::DialogType::Success);
        dlg->setPopupPosition(FluentContentDialog::PopupPosition::TopLeft);
        dlg->setDialogSize(240, 100);
        dlg->exec();
        dlg->deleteLater();
    });

    auto* warnBtn = new FluentButton(u8"警告(左下)", FluentButtonVariant::Secondary, page);
    connect(warnBtn, &FluentButton::clicked, this, [this]() {
        auto dlg = new FluentContentDialog(this);
        dlg->setTitle(u8"警告");
        dlg->setContent(u8"请注意风险");
        dlg->setDialogType(FluentContentDialog::DialogType::Warning);
        dlg->setPopupPosition(FluentContentDialog::PopupPosition::BottomLeft);
        dlg->setDialogSize(240, 100);
        dlg->setNegativeButton(u8"取消");
        dlg->exec();
        dlg->deleteLater();
    });

    auto* trBtn = new FluentButton(u8"右上角", FluentButtonVariant::Secondary, page);
    connect(trBtn, &FluentButton::clicked, this, [this]() {
        auto dlg = new FluentContentDialog(this);
        dlg->setTitle(u8"右上");
        dlg->setContent(u8"右上角弹出通知");
        dlg->setDialogType(FluentContentDialog::DialogType::Info);
        dlg->setPopupPosition(FluentContentDialog::PopupPosition::TopRight);
        dlg->setDialogSize(240, 100);
        dlg->exec();
        dlg->deleteLater();
    });

    btnRow2->addWidget(errBtn);
    btnRow2->addWidget(succBtn);
    btnRow2->addWidget(warnBtn);
    btnRow2->addWidget(trBtn);
    btnRow2->addStretch();

    layout->addLayout(btnRow2);

    // 气泡消息
    auto* infoBarLabel = new QLabel(u8"气泡消息 (InfoBar)", page);
    infoBarLabel->setFont(theme->captionFont());
    infoBarLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [infoBarLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        infoBarLabel->setFont(t->captionFont());
        infoBarLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(infoBarLabel);

    auto* barRow = new QHBoxLayout();
    barRow->setSpacing(8);

    auto* barInfoBtn = new FluentButton(u8"Info", FluentButtonVariant::Secondary, page);
    connect(barInfoBtn, &FluentButton::clicked, this, [this]() {
        FluentInfoBar::info(this, u8"提示", u8"这是一条信息气泡消息");
    });

    auto* barSuccBtn = new FluentButton(u8"Success", FluentButtonVariant::Secondary, page);
    connect(barSuccBtn, &FluentButton::clicked, this, [this]() {
        FluentInfoBar::success(this, u8"成功", u8"操作已成功完成");
    });

    auto* barWarnBtn = new FluentButton(u8"Warning", FluentButtonVariant::Secondary, page);
    connect(barWarnBtn, &FluentButton::clicked, this, [this]() {
        FluentInfoBar::warning(this, u8"警告", u8"请注意潜在风险");
    });

    auto* barErrBtn = new FluentButton(u8"Error", FluentButtonVariant::Secondary, page);
    connect(barErrBtn, &FluentButton::clicked, this, [this]() {
        FluentInfoBar::error(this, u8"错误", u8"操作失败，请重试");
    });

    auto* barBottomBtn = new FluentButton(u8"底部弹出", FluentButtonVariant::Secondary, page);
    connect(barBottomBtn, &FluentButton::clicked, this, [this]() {
        FluentInfoBar::info(this, u8"底部消息", u8"从底部弹出的气泡",
                            3000, FluentInfoBar::Position::Bottom);
    });

    barRow->addWidget(barInfoBtn);
    barRow->addWidget(barSuccBtn);
    barRow->addWidget(barWarnBtn);
    barRow->addWidget(barErrBtn);
    barRow->addWidget(barBottomBtn);
    barRow->addStretch();

    layout->addLayout(barRow);

    // 悬浮式窗口
    auto* popLabel = new QLabel(u8"悬浮式窗口 (Popover)", page);
    popLabel->setFont(theme->captionFont());
    popLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [popLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        popLabel->setFont(t->captionFont());
        popLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(popLabel);

    auto* popRow = new QHBoxLayout();
    popRow->setSpacing(8);

    auto* popTopBtn = new FluentButton(u8"上方弹出", FluentButtonVariant::Secondary, page);
    auto* popBottomBtn = new FluentButton(u8"下方弹出", FluentButtonVariant::Secondary, page);
    auto* popLeftBtn = new FluentButton(u8"左侧弹出", FluentButtonVariant::Secondary, page);
    auto* popRightBtn = new FluentButton(u8"右侧弹出", FluentButtonVariant::Secondary, page);

    connect(popTopBtn, &FluentButton::clicked, this, [popTopBtn]() {
        auto* pop = new FluentPopover();
        pop->setTarget(popTopBtn);
        pop->setPlacement(FluentPopover::Top);
        pop->setTitleAndContent(u8"提示", u8"这是从上方弹出的悬浮窗口");
        pop->showPopover();
    });

    connect(popBottomBtn, &FluentButton::clicked, this, [popBottomBtn]() {
        auto* pop = new FluentPopover();
        pop->setTarget(popBottomBtn);
        pop->setPlacement(FluentPopover::Bottom);
        pop->setTitleAndContent(u8"信息", u8"这是从下方弹出的悬浮窗口，支持自动关闭");
        pop->showPopover();
    });

    connect(popLeftBtn, &FluentButton::clicked, this, [popLeftBtn]() {
        auto* pop = new FluentPopover();
        pop->setTarget(popLeftBtn);
        pop->setPlacement(FluentPopover::Left);
        pop->setTitleAndContent(u8"左侧", u8"左侧弹出");
        pop->showPopover();
    });

    connect(popRightBtn, &FluentButton::clicked, this, [popRightBtn]() {
        auto* pop = new FluentPopover();
        pop->setTarget(popRightBtn);
        pop->setPlacement(FluentPopover::Right);
        pop->setTitleAndContent(u8"右侧", u8"右侧弹出");
        pop->showPopover();
    });

    popRow->addWidget(popTopBtn);
    popRow->addWidget(popBottomBtn);
    popRow->addWidget(popLeftBtn);
    popRow->addWidget(popRightBtn);
    popRow->addStretch();
    layout->addLayout(popRow);

    // 悬停触发
    auto* hoverLabel = new QLabel(u8"悬停触发 (鼠标移入即显示)", page);
    hoverLabel->setFont(theme->captionFont());
    hoverLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [hoverLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        hoverLabel->setFont(t->captionFont());
        hoverLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(hoverLabel);

    auto* hoverRow = new QHBoxLayout();
    hoverRow->setSpacing(16);

    auto* hoverBtn1 = new FluentButton(u8"悬停提示", FluentButtonVariant::Primary, page);
    auto* hoverBtn2 = new FluentButton(u8"悬停详情", FluentButtonVariant::Secondary, page);

    // 悬停触发的 Popover（持久化，不每次 new）
    auto* hoverPop1 = new FluentPopover();
    hoverPop1->setTarget(hoverBtn1);
    hoverPop1->setPlacement(FluentPopover::Top);
    hoverPop1->setTriggerMode(FluentPopover::Hover);
    hoverPop1->setTitleAndContent(u8"快捷提示", u8"鼠标移入即可查看信息，移出自动隐藏");

    auto* hoverPop2 = new FluentPopover();
    hoverPop2->setTarget(hoverBtn2);
    hoverPop2->setPlacement(FluentPopover::Bottom);
    hoverPop2->setTriggerMode(FluentPopover::Hover);
    hoverPop2->setTitleAndContent(u8"详细信息", u8"这是悬停触发的 Popover，\n可以移动到气泡上继续查看内容");

    Q_UNUSED(hoverPop1);
    Q_UNUSED(hoverPop2);

    hoverRow->addWidget(hoverBtn1);
    hoverRow->addWidget(hoverBtn2);
    hoverRow->addStretch();
    layout->addLayout(hoverRow);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createTablePage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"Table 表格", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // 创建表格
    auto* table = new FluentTable(page);
    table->setAlternatingRowColors(true);
    table->setShowGrid(true);
    table->setSortable(true);
    table->setSelectionMode(FluentTable::SelectionMode::Single);

    // 创建数据模型
    auto* model = new QStandardItemModel(page);
    model->setHorizontalHeaderLabels({
        u8"姓名", u8"年龄", u8"城市", u8"职业", u8"邮箱"
    });

    // 添加示例数据
    struct Person {
        QString name, city, job, email;
        int age;
    };
    QList<Person> people = {
        {u8"张三",  u8"北京",  u8"工程师",     u8"zhangsan@example.com",  28},
        {u8"李四",  u8"上海",  u8"设计师",     u8"lisi@example.com",      32},
        {u8"王五",  u8"深圳",  u8"产品经理",   u8"wangwu@example.com",    25},
        {u8"赵六",  u8"杭州",  u8"数据分析师", u8"zhaoliu@example.com",   30},
        {u8"孙七",  u8"成都",  u8"架构师",     u8"sunqi@example.com",     35},
        {u8"周八",  u8"广州",  u8"测试工程师", u8"zhouba@example.com",    27},
        {u8"吴九",  u8"南京",  u8"运维工程师", u8"wujiu@example.com",     29},
        {u8"郑十",  u8"武汉",  u8"前端开发",   u8"zhengshi@example.com",  26},
        {u8"钱一",  u8"西安",  u8"后端开发",   u8"qianyi@example.com",    31},
        {u8"陈二",  u8"重庆",  u8"全栈开发",   u8"chener@example.com",    33},
        {u8"林三",  u8"长沙",  u8"算法工程师", u8"linsan@example.com",    28},
        {u8"黄四",  u8"天津",  u8"安全工程师", u8"huangsi@example.com",   34},
    };

    for (const auto& p : people) {
        QList<QStandardItem*> row;
        row << new QStandardItem(p.name)
            << new QStandardItem(QString::number(p.age))
            << new QStandardItem(p.city)
            << new QStandardItem(p.job)
            << new QStandardItem(p.email);
        model->appendRow(row);
    }

    table->setModel(model);
    table->setColumnWidth(0, 100);
    table->setColumnWidth(1, 70);
    table->setColumnWidth(2, 100);
    table->setColumnWidth(3, 120);
    table->setColumnWidth(4, 200);

    layout->addWidget(table, 1);

    // 状态标签
    auto* statusLabel = new QLabel(u8"点击表格行查看选中信息", page);
    statusLabel->setFont(theme->captionFont());
    statusLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [statusLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        statusLabel->setFont(t->captionFont());
        statusLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });

    connect(table, &FluentTable::clicked, this, [statusLabel, model](const QModelIndex& index) {
        if (!index.isValid()) return;
        QString name = model->index(index.row(), 0).data().toString();
        QString job = model->index(index.row(), 3).data().toString();
        statusLabel->setText(QString(u8"选中: 第 %1 行 — %2 (%3)").arg(index.row() + 1).arg(name).arg(job));
    });

    layout->addWidget(statusLabel);

    return page;
}

QWidget* DemoMainWindow::createTreePage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"Tree 树型控件", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // 创建树控件
    auto* tree = new FluentTree(page);
    tree->setSelectionMode(FluentTree::SelectionMode::Single);
    tree->setShowLines(true);

    // 创建树形数据模型
    auto* model = new QStandardItemModel(page);
    model->setHorizontalHeaderLabels({u8"名称"});

    // 文件系统示例
    auto* rootNode = new QStandardItem(u8"📁 项目根目录");
    auto* srcNode  = new QStandardItem(u8"📁 src");
    auto* coreNode = new QStandardItem(u8"📁 Core");
    coreNode->appendRow(new QStandardItem(u8"📄 FluentTheme.h"));
    coreNode->appendRow(new QStandardItem(u8"📄 FluentTheme.cpp"));
    coreNode->appendRow(new QStandardItem(u8"📄 FluentGlobal.h"));
    auto* widgetsNode = new QStandardItem(u8"📁 Widgets");
    auto* btnNode = new QStandardItem(u8"📁 Buttons");
    btnNode->appendRow(new QStandardItem(u8"📄 FluentButton.h"));
    btnNode->appendRow(new QStandardItem(u8"📄 FluentButton.cpp"));
    auto* inputNode = new QStandardItem(u8"📁 Input");
    inputNode->appendRow(new QStandardItem(u8"📄 FluentLineEdit.h"));
    inputNode->appendRow(new QStandardItem(u8"📄 FluentCheckBox.h"));
    inputNode->appendRow(new QStandardItem(u8"📄 FluentToggleSwitch.h"));
    auto* dataNode = new QStandardItem(u8"📁 Data");
    dataNode->appendRow(new QStandardItem(u8"📄 FluentTable.h"));
    dataNode->appendRow(new QStandardItem(u8"📄 FluentTree.h"));
    widgetsNode->appendRow(btnNode);
    widgetsNode->appendRow(inputNode);
    widgetsNode->appendRow(dataNode);
    srcNode->appendRow(coreNode);
    srcNode->appendRow(widgetsNode);

    auto* demoNode = new QStandardItem(u8"📁 demo");
    demoNode->appendRow(new QStandardItem(u8"📄 main.cpp"));
    demoNode->appendRow(new QStandardItem(u8"📄 MainWindow.h"));
    demoNode->appendRow(new QStandardItem(u8"📄 MainWindow.cpp"));

    auto* docNode = new QStandardItem(u8"📁 docs");
    docNode->appendRow(new QStandardItem(u8"📄 README.md"));
    docNode->appendRow(new QStandardItem(u8"📄 CHANGELOG.md"));

    rootNode->appendRow(srcNode);
    rootNode->appendRow(demoNode);
    rootNode->appendRow(docNode);
    rootNode->appendRow(new QStandardItem(u8"📄 CMakeLists.txt"));
    model->appendRow(rootNode);

    tree->setModel(model);

    // 默认展开根节点和 src
    tree->expand(rootNode->index());
    tree->expand(srcNode->index());

    layout->addWidget(tree, 1);

    // 状态标签
    auto* statusLabel = new QLabel(u8"点击节点查看信息，双击展开/折叠", page);
    statusLabel->setFont(theme->captionFont());
    statusLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [statusLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        statusLabel->setFont(t->captionFont());
        statusLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });

    connect(tree, &FluentTree::clicked, this, [statusLabel, model](const QModelIndex& index) {
        if (!index.isValid()) return;
        QString name = model->data(index, Qt::DisplayRole).toString();
        int depth = 0;
        QModelIndex parent = index.parent();
        while (parent.isValid()) { depth++; parent = parent.parent(); }
        statusLabel->setText(QString(u8"选中: %1 (层级: %2)").arg(name).arg(depth));
    });

    connect(tree, &FluentTree::expanded, this, [statusLabel](const QModelIndex& index) {
        QString name = index.data(Qt::DisplayRole).toString();
        statusLabel->setText(QString(u8"展开: %1").arg(name));
    });

    connect(tree, &FluentTree::collapsed, this, [statusLabel](const QModelIndex& index) {
        QString name = index.data(Qt::DisplayRole).toString();
        statusLabel->setText(QString(u8"折叠: %1").arg(name));
    });

    layout->addWidget(statusLabel);

    return page;
}

QWidget* DemoMainWindow::createGraphPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"Graph 节点连接图", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // 创建图控件
    auto* graph = new FluentGraph(page);

    // 构建数据处理流水线节点
    auto* inputNode = graph->addNode("input", u8"数据源");
    inputNode->addPort("output", FluentGraphPort::Direction::Out);
    inputNode->setColorTag(QColor("#4CAF50"));  // 绿色

    auto* filterNode = graph->addNode("filter", u8"数据过滤");
    filterNode->addPort("input", FluentGraphPort::Direction::In);
    filterNode->addPort("rejected", FluentGraphPort::Direction::Out);
    filterNode->addPort("output", FluentGraphPort::Direction::Out);
    filterNode->setColorTag(QColor("#FF9800"));  // 橙色

    auto* transformNode = graph->addNode("transform", u8"数据转换");
    transformNode->addPort("input", FluentGraphPort::Direction::In);
    transformNode->addPort("output", FluentGraphPort::Direction::Out);
    transformNode->setColorTag(QColor("#2196F3"));  // 蓝色

    auto* aggregateNode = graph->addNode("aggregate", u8"数据聚合");
    aggregateNode->addPort("input", FluentGraphPort::Direction::In);
    aggregateNode->addPort("output", FluentGraphPort::Direction::Out);
    aggregateNode->setColorTag(QColor("#9C27B0"));  // 紫色

    auto* outputNode = graph->addNode("output", u8"输出");
    outputNode->addPort("input", FluentGraphPort::Direction::In);
    outputNode->setColorTag(QColor("#F44336"));  // 红色

    auto* logNode = graph->addNode("log", u8"日志记录");
    logNode->addPort("input", FluentGraphPort::Direction::In);
    logNode->setColorTag(QColor("#607D8B"));  // 灰色

    // 创建连接
    graph->addEdge("input", "output", "filter", "input");
    graph->addEdge("filter", "output", "transform", "input");
    graph->addEdge("transform", "output", "aggregate", "input");
    graph->addEdge("aggregate", "output", "output", "input");
    graph->addEdge("filter", "rejected", "log", "input");

    // 自动水平布局
    graph->layoutHorizontal(280, 140);

    layout->addWidget(graph, 1);

    // 状态标签
    auto* statusLabel = new QLabel(u8"拖拽节点移动 | 拖拽输出端口连接 | 滚轮缩放 | 空白区域拖拽平移", page);
    statusLabel->setFont(theme->captionFont());
    statusLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [statusLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        statusLabel->setFont(t->captionFont());
        statusLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });

    connect(graph, &FluentGraph::nodeClicked, this, [statusLabel, graph](const QString& id) {
        auto* n = graph->node(id);
        if (n) statusLabel->setText(QString(u8"点击节点: %1 (%2个端口)").arg(n->title()).arg(n->ports().size()));
    });

    connect(graph, &FluentGraph::nodeMoved, this, [statusLabel](const QString& id, const QPointF& pos) {
        statusLabel->setText(QString(u8"移动节点: %1 → (%2, %3)").arg(id).arg(int(pos.x())).arg(int(pos.y())));
    });

    connect(graph, &FluentGraph::edgeCreated, this, [statusLabel](const QString& from, const QString& fromPort,
                                                                    const QString& to, const QString& toPort) {
        statusLabel->setText(QString(u8"新连接: %1.%2 → %3.%4").arg(from).arg(fromPort).arg(to).arg(toPort));
    });

    layout->addWidget(statusLabel);

    return page;
}

QWidget* DemoMainWindow::createNavBarPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(20);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"NavBar 导航栏", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // --- Tab 样式 ---
    auto* tabLabel = new QLabel(u8"Tab 选项卡导航", page);
    tabLabel->setFont(theme->captionFont());
    tabLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [tabLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        tabLabel->setFont(t->captionFont());
        tabLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(tabLabel);

    auto* tabBar = new FluentNavBar(page);
    tabBar->setNavStyle(FluentNavBar::NavStyle::Tab);
    tabBar->addItem("home", u8"首页");
    tabBar->addItem("docs", u8"文档");
    tabBar->addItem("api", u8"API");
    tabBar->addItem("blog", u8"博客");
    tabBar->addItem("about", u8"关于");
    layout->addWidget(tabBar);

    auto* tabStatus = new QLabel(u8"当前选中: 首页", page);
    tabStatus->setFont(theme->captionFont());
    tabStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [tabStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        tabStatus->setFont(t->captionFont());
        tabStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(tabBar, &FluentNavBar::currentChanged, this, [tabStatus](const QString& key) {
        tabStatus->setText(QString(u8"当前选中: %1").arg(key));
    });
    layout->addWidget(tabStatus);

    // --- Pill 样式 ---
    auto* pillLabel = new QLabel(u8"Pill 胶囊导航", page);
    pillLabel->setFont(theme->captionFont());
    pillLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [pillLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        pillLabel->setFont(t->captionFont());
        pillLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(pillLabel);

    auto* pillBar = new FluentNavBar(page);
    pillBar->setNavStyle(FluentNavBar::NavStyle::Pill);
    pillBar->addItem("all", u8"全部");
    pillBar->addItem("active", u8"进行中");
    pillBar->addItem("completed", u8"已完成");
    pillBar->addItem("archived", u8"已归档");
    layout->addWidget(pillBar);

    auto* pillStatus = new QLabel(u8"当前选中: 全部", page);
    pillStatus->setFont(theme->captionFont());
    pillStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [pillStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        pillStatus->setFont(t->captionFont());
        pillStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(pillBar, &FluentNavBar::currentChanged, this, [pillStatus](const QString& key) {
        pillStatus->setText(QString(u8"当前选中: %1").arg(key));
    });
    layout->addWidget(pillStatus);

    // --- Breadcrumb 样式 ---
    auto* bcLabel = new QLabel(u8"Breadcrumb 面包屑导航", page);
    bcLabel->setFont(theme->captionFont());
    bcLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [bcLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        bcLabel->setFont(t->captionFont());
        bcLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(bcLabel);

    auto* bcBar = new FluentNavBar(page);
    bcBar->setNavStyle(FluentNavBar::NavStyle::Breadcrumb);
    bcBar->addItem("root", u8"根目录");
    bcBar->addItem("project", u8"项目");
    bcBar->addItem("src", u8"src");
    bcBar->addItem("widgets", u8"Widgets");
    bcBar->setCurrentKey("widgets");
    layout->addWidget(bcBar);

    QStringList bcTexts = { u8"根目录", u8"项目", u8"src", u8"Widgets" };
    auto* bcStatus = new QLabel(u8"当前路径: 根目录 › 项目 › src › Widgets", page);
    bcStatus->setFont(theme->captionFont());
    bcStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [bcStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        bcStatus->setFont(t->captionFont());
        bcStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(bcBar, &FluentNavBar::currentChanged, this, [bcStatus, bcBar, bcTexts](const QString& key) {
        Q_UNUSED(key);
        QString path;
        int curIdx = bcBar->currentIndex();
        for (int i = 0; i <= curIdx && i < bcTexts.size(); ++i) {
            if (i > 0) path += u8" › ";
            path += bcTexts[i];
        }
        bcStatus->setText(QString(u8"当前路径: %1").arg(path));
    });
    layout->addWidget(bcStatus);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createSettingsPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"常规设置", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    auto* themeBtn = new FluentButton(u8"切换深色/浅色主题", FluentButtonVariant::Secondary, page);
    connect(themeBtn, &FluentButton::clicked, this, &DemoMainWindow::toggleTheme);
    layout->addWidget(themeBtn);
    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createAboutPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"关于", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    auto* desc = new QLabel(u8"FluentWidgets\n基于 Qt 5.15.2 的 Fluent Design 风格组件库\n版本 1.0.0", page);
    desc->setFont(theme->bodyFont());
    desc->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [desc]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        desc->setFont(t->bodyFont());
        desc->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(desc);
    layout->addStretch();

    return page;
}

void DemoMainWindow::toggleTheme() {
    auto& mgr = FluentThemeManager::instance();
    if (mgr.themeMode() == FluentThemeMode::Light) {
        mgr.setThemeMode(FluentThemeMode::Dark);
    } else {
        mgr.setThemeMode(FluentThemeMode::Light);
    }
}

void DemoMainWindow::updateWindowIcon() {
    bool isDark = FluentThemeManager::instance().currentTheme()->isDark();
    setWindowIcon(QIcon(isDark ? ":/icons/fluent_dark.svg" : ":/icons/fluent.svg"));
}

bool DemoMainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonRelease) {
        auto* widget = qobject_cast<QWidget*>(obj);
        if (widget && m_cardPageMap.contains(widget)) {
            m_navView->setCurrentPage(m_cardPageMap[widget]);
            return true;
        }
    }
    return FluentWindow::eventFilter(obj, event);
}

QWidget* DemoMainWindow::createCalendarPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"Calendar 日历", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // --- 单日期选择 ---
    auto* singleLabel = new QLabel(u8"单日期选择", page);
    singleLabel->setFont(theme->captionFont());
    singleLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [singleLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        singleLabel->setFont(t->captionFont());
        singleLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(singleLabel);

    auto* singleCal = new FluentCalendar(page);
    singleCal->setSelectionMode(FluentCalendar::SingleDate);

    // 标记一些日期
    QSet<QDate> marks;
    marks << QDate::currentDate().addDays(2)
          << QDate::currentDate().addDays(5)
          << QDate::currentDate().addDays(8);
    singleCal->setMarkedDates(marks);

    layout->addWidget(singleCal);

    auto* singleStatus = new QLabel(
        QString(u8"选中日期: %1").arg(QDate::currentDate().toString("yyyy-MM-dd")), page);
    singleStatus->setFont(theme->captionFont());
    singleStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [singleStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        singleStatus->setFont(t->captionFont());
        singleStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(singleCal, &FluentCalendar::selectedDateChanged, this, [singleStatus](const QDate& date) {
        singleStatus->setText(QString(u8"选中日期: %1").arg(date.toString("yyyy-MM-dd")));
    });
    layout->addWidget(singleStatus);

    // --- 日期范围选择 ---
    auto* rangeLabel = new QLabel(u8"日期范围选择", page);
    rangeLabel->setFont(theme->captionFont());
    rangeLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [rangeLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        rangeLabel->setFont(t->captionFont());
        rangeLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(rangeLabel);

    auto* rangeCal = new FluentCalendar(page);
    rangeCal->setSelectionMode(FluentCalendar::DateRange);

    layout->addWidget(rangeCal);

    auto* rangeStatus = new QLabel(u8"点击选择起始日期和结束日期", page);
    rangeStatus->setFont(theme->captionFont());
    rangeStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [rangeStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        rangeStatus->setFont(t->captionFont());
        rangeStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(rangeCal, &FluentCalendar::dateRangeChanged, this, [rangeStatus](const QDate& start, const QDate& end) {
        rangeStatus->setText(QString(u8"范围: %1 → %2").arg(start.toString("yyyy-MM-dd")).arg(end.toString("yyyy-MM-dd")));
    });
    layout->addWidget(rangeStatus);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createCarouselPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"Carousel 轮播图", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // 基础轮播图
    auto* carousel = new FluentCarousel(page);
    carousel->setFixedHeight(240);
    carousel->addSlide(QColor("#4A90D9"), u8"欢迎使用 FluentWidgets");
    carousel->addSlide(QColor("#50C878"), u8"丰富的组件库");
    carousel->addSlide(QColor("#FF6B6B"), u8"Fluent Design 风格");
    carousel->addSlide(QColor("#FFB347"), u8"支持深色/浅色主题");
    carousel->addSlide(QColor("#9B59B6"), u8"持续更新中...");
    carousel->setAutoPlay(true);
    carousel->setInterval(3000);
    layout->addWidget(carousel);

    auto* carouselStatus = new QLabel(u8"当前: 第 1 张 / 共 5 张", page);
    carouselStatus->setFont(theme->captionFont());
    carouselStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [carouselStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        carouselStatus->setFont(t->captionFont());
        carouselStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(carousel, &FluentCarousel::currentChanged, this, [carouselStatus, carousel](int index) {
        carouselStatus->setText(QString(u8"当前: 第 %1 张 / 共 %2 张").arg(index + 1).arg(carousel->slideCount()));
    });
    layout->addWidget(carouselStatus);

    // 控制按钮
    auto* ctrlRow = new QHBoxLayout();
    ctrlRow->setSpacing(8);

    auto* prevBtn = new FluentButton(u8"上一张", FluentButtonVariant::Secondary, page);
    auto* nextBtn = new FluentButton(u8"下一张", FluentButtonVariant::Secondary, page);
    auto* toggleAutoBtn = new FluentButton(u8"暂停自动播放", FluentButtonVariant::Secondary, page);

    connect(prevBtn, &FluentButton::clicked, carousel, &FluentCarousel::goToPrev);
    connect(nextBtn, &FluentButton::clicked, carousel, &FluentCarousel::goToNext);
    connect(toggleAutoBtn, &FluentButton::clicked, this, [carousel, toggleAutoBtn]() {
        carousel->setAutoPlay(!carousel->autoPlay());
        toggleAutoBtn->setText(carousel->autoPlay() ? u8"暂停自动播放" : u8"开启自动播放");
    });

    ctrlRow->addWidget(prevBtn);
    ctrlRow->addWidget(nextBtn);
    ctrlRow->addWidget(toggleAutoBtn);
    ctrlRow->addStretch();
    layout->addLayout(ctrlRow);

    // 无箭头轮播图
    auto* noArrowLabel = new QLabel(u8"无箭头 + 顶部指示器", page);
    noArrowLabel->setFont(theme->captionFont());
    noArrowLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [noArrowLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        noArrowLabel->setFont(t->captionFont());
        noArrowLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(noArrowLabel);

    auto* carousel2 = new FluentCarousel(page);
    carousel2->setFixedHeight(160);
    carousel2->addSlide(QColor("#2C3E50"), u8"Slide 1");
    carousel2->addSlide(QColor("#8E44AD"), u8"Slide 2");
    carousel2->addSlide(QColor("#16A085"), u8"Slide 3");
    carousel2->setArrowVisibility(FluentCarousel::Never);
    carousel2->setIndicatorPosition(FluentCarousel::Top);
    carousel2->setInterval(2000);
    layout->addWidget(carousel2);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createProgressBarPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"ProgressBar 进度条", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // --- 线形进度条 ---
    auto* lineLabel = new QLabel(u8"线形进度条", page);
    lineLabel->setFont(theme->captionFont());
    lineLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [lineLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        lineLabel->setFont(t->captionFont());
        lineLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(lineLabel);

    // 基础
    auto* bar1 = new FluentProgressBar(page);
    bar1->setValue(65);
    bar1->setLabel(u8"下载进度");
    bar1->setShowValue(true);
    layout->addWidget(bar1);

    // 条纹
    auto* bar2 = new FluentProgressBar(page);
    bar2->setValue(40);
    bar2->setStriped(true);
    bar2->setStripedAnimated(true);
    bar2->setLabel(u8"安装中");
    bar2->setShowValue(true);
    layout->addWidget(bar2);

    // 自定义颜色
    auto* bar3 = new FluentProgressBar(page);
    bar3->setValue(80);
    bar3->setCustomColor(QColor("#50C878"));
    bar3->setLabel(u8"成功");
    bar3->setShowValue(true);
    layout->addWidget(bar3);

    auto* bar4 = new FluentProgressBar(page);
    bar4->setValue(25);
    bar4->setCustomColor(QColor("#FF6B6B"));
    bar4->setLabel(u8"错误");
    bar4->setShowValue(true);
    layout->addWidget(bar4);

    // 不确定模式
    auto* bar5 = new FluentProgressBar(page);
    bar5->setIndeterminate(true);
    bar5->setLabel(u8"加载中...");
    layout->addWidget(bar5);

    // --- 环形进度条 ---
    auto* ringLabel = new QLabel(u8"环形进度条", page);
    ringLabel->setFont(theme->captionFont());
    ringLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [ringLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        ringLabel->setFont(t->captionFont());
        ringLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(ringLabel);

    auto* ringRow = new QHBoxLayout();
    ringRow->setSpacing(24);

    auto* ring1 = new FluentProgressBar(page);
    ring1->setBarStyle(FluentProgressBar::RingBar);
    ring1->setValue(72);
    ring1->setShowValue(true);
    ring1->setFixedSize(100, 100);

    auto* ring2 = new FluentProgressBar(page);
    ring2->setBarStyle(FluentProgressBar::RingBar);
    ring2->setValue(45);
    ring2->setShowValue(true);
    ring2->setCustomColor(QColor("#FFB347"));
    ring2->setFixedSize(100, 100);

    auto* ring3 = new FluentProgressBar(page);
    ring3->setBarStyle(FluentProgressBar::RingBar);
    ring3->setIndeterminate(true);
    ring3->setFixedSize(100, 100);

    ringRow->addWidget(ring1);
    ringRow->addWidget(ring2);
    ringRow->addWidget(ring3);
    ringRow->addStretch();
    layout->addLayout(ringRow);

    // --- 控制按钮 ---
    auto* ctrlRow = new QHBoxLayout();
    ctrlRow->setSpacing(8);

    auto* lessBtn = new FluentButton(u8"-10", FluentButtonVariant::Secondary, page);
    auto* moreBtn = new FluentButton(u8"+10", FluentButtonVariant::Secondary, page);
    auto* resetBtn = new FluentButton(u8"重置", FluentButtonVariant::Secondary, page);

    connect(lessBtn, &FluentButton::clicked, this, [bar1, ring1]() {
        bar1->setValue(bar1->value() - 10);
        ring1->setValue(ring1->value() - 10);
    });
    connect(moreBtn, &FluentButton::clicked, this, [bar1, ring1]() {
        bar1->setValue(bar1->value() + 10);
        ring1->setValue(ring1->value() + 10);
    });
    connect(resetBtn, &FluentButton::clicked, this, [bar1, ring1]() {
        bar1->setValue(0);
        ring1->setValue(0);
    });

    ctrlRow->addWidget(lessBtn);
    ctrlRow->addWidget(moreBtn);
    ctrlRow->addWidget(resetBtn);
    ctrlRow->addStretch();
    layout->addLayout(ctrlRow);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createLineChartPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"LineChart 线图", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // === 折线图 ===
    auto* lineLabel = new QLabel(u8"折线图（多系列）", page);
    lineLabel->setFont(theme->captionFont());
    lineLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [lineLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        lineLabel->setFont(t->captionFont());
        lineLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(lineLabel);

    auto* lineChart = new FluentLineChart(page);
    lineChart->setChartType(FluentLineChart::LineChart);
    lineChart->setSmooth(false);
    lineChart->setXAxisTitle(u8"月份");
    lineChart->setYAxisTitle(u8"销售额 (万)");
    lineChart->setMinimumHeight(260);

    FluentLineChart::Series s1;
    s1.name = u8"产品 A";
    s1.points = {{1, 23}, {2, 35}, {3, 28}, {4, 45}, {5, 42}, {6, 58}, {7, 52}, {8, 65}, {9, 70}, {10, 68}, {11, 82}, {12, 90}};
    lineChart->addSeries(s1);

    FluentLineChart::Series s2;
    s2.name = u8"产品 B";
    s2.points = {{1, 12}, {2, 18}, {3, 25}, {4, 30}, {5, 35}, {6, 32}, {7, 40}, {8, 38}, {9, 48}, {10, 55}, {11, 60}, {12, 65}};
    lineChart->addSeries(s2);

    FluentLineChart::Series s3;
    s3.name = u8"产品 C";
    s3.color = QColor(255, 140, 0);
    s3.points = {{1, 5}, {2, 8}, {3, 15}, {4, 12}, {5, 20}, {6, 22}, {7, 28}, {8, 35}, {9, 30}, {10, 40}, {11, 38}, {12, 45}};
    s3.lineStyle = Qt::DashLine;
    lineChart->addSeries(s3);

    layout->addWidget(lineChart);

    // === 曲线面积图 ===
    auto* splineLabel = new QLabel(u8"曲线面积图", page);
    splineLabel->setFont(theme->captionFont());
    splineLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [splineLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        splineLabel->setFont(t->captionFont());
        splineLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(splineLabel);

    auto* splineChart = new FluentLineChart(page);
    splineChart->setChartType(FluentLineChart::SplineChart);
    splineChart->setSmooth(true);
    splineChart->setAreaFill(true);
    splineChart->setXAxisTitle(u8"时间");
    splineChart->setYAxisTitle(u8"温度 (°C)");
    splineChart->setMinimumHeight(240);

    FluentLineChart::Series tempSeries;
    tempSeries.name = u8"温度变化";
    tempSeries.color = QColor(218, 59, 59);
    tempSeries.lineWidth = 2.5;
    tempSeries.dotSize = 5;
    tempSeries.points = {{0, 8}, {2, 10}, {4, 15}, {6, 20}, {8, 26}, {10, 30}, {12, 33}, {14, 35}, {16, 33}, {18, 28}, {20, 22}, {22, 15}, {24, 10}};
    splineChart->addSeries(tempSeries);

    layout->addWidget(splineChart);

    // === 散点图 ===
    auto* scatterLabel = new QLabel(u8"散点图", page);
    scatterLabel->setFont(theme->captionFont());
    scatterLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [scatterLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        scatterLabel->setFont(t->captionFont());
        scatterLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(scatterLabel);

    auto* scatterChart = new FluentLineChart(page);
    scatterChart->setChartType(FluentLineChart::ScatterChart);
    scatterChart->setShowDots(true);
    scatterChart->setXAxisTitle(u8"身高 (cm)");
    scatterChart->setYAxisTitle(u8"体重 (kg)");
    scatterChart->setMinimumHeight(240);

    FluentLineChart::Series male;
    male.name = u8"男性";
    male.color = QColor(0, 120, 212);
    male.dotSize = 8;
    male.points = {
        {165, 60}, {170, 65}, {172, 68}, {175, 70}, {178, 72}, {180, 75},
        {182, 78}, {168, 62}, {176, 71}, {185, 82}, {173, 67}, {179, 74},
        {183, 80}, {169, 63}, {177, 73}, {181, 77}, {174, 69}, {186, 85},
        {171, 66}, {184, 79}
    };
    scatterChart->addSeries(male);

    FluentLineChart::Series female;
    female.name = u8"女性";
    female.color = QColor(218, 59, 59);
    female.dotSize = 8;
    female.points = {
        {155, 48}, {158, 50}, {160, 52}, {162, 55}, {163, 53}, {165, 56},
        {157, 49}, {161, 51}, {164, 54}, {159, 50}, {166, 58}, {156, 47},
        {168, 60}, {163, 54}, {160, 53}, {167, 59}, {162, 55}, {170, 62},
        {158, 51}, {165, 57}
    };
    scatterChart->addSeries(female);

    layout->addWidget(scatterChart);

    // === 控制按钮 ===
    auto* ctrlRow = new QHBoxLayout();
    ctrlRow->setSpacing(8);

    auto* reanimateBtn = new FluentButton(u8"重播动画", FluentButtonVariant::Accent, page);
    connect(reanimateBtn, &FluentButton::clicked, this, [lineChart, splineChart, scatterChart]() {
        lineChart->setAnimate(true);
        splineChart->setAnimate(true);
        scatterChart->setAnimate(true);
        auto lineSeries1 = lineChart->series(0);
        auto lineSeries2 = lineChart->series(1);
        auto lineSeries3 = lineChart->series(2);
        lineChart->clearSeries();
        lineChart->addSeries(lineSeries1);
        lineChart->addSeries(lineSeries2);
        lineChart->addSeries(lineSeries3);

        auto splineSeries = splineChart->series(0);
        splineChart->clearSeries();
        splineChart->addSeries(splineSeries);

        auto scatterSeries1 = scatterChart->series(0);
        auto scatterSeries2 = scatterChart->series(1);
        scatterChart->clearSeries();
        scatterChart->addSeries(scatterSeries1);
        scatterChart->addSeries(scatterSeries2);
    });

    auto* toggleGridBtn = new FluentButton(u8"切换网格", FluentButtonVariant::Secondary, page);
    connect(toggleGridBtn, &FluentButton::clicked, this, [lineChart, splineChart, scatterChart]() {
        lineChart->setShowGrid(!lineChart->showGrid());
        splineChart->setShowGrid(!splineChart->showGrid());
        scatterChart->setShowGrid(!scatterChart->showGrid());
    });

    auto* toggleDotsBtn = new FluentButton(u8"切换数据点", FluentButtonVariant::Secondary, page);
    connect(toggleDotsBtn, &FluentButton::clicked, this, [lineChart, splineChart]() {
        lineChart->setShowDots(!lineChart->showDots());
        splineChart->setShowDots(!splineChart->showDots());
    });

    ctrlRow->addWidget(reanimateBtn);
    ctrlRow->addWidget(toggleGridBtn);
    ctrlRow->addWidget(toggleDotsBtn);
    ctrlRow->addStretch();
    layout->addLayout(ctrlRow);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createBarChartPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"BarChart 柱状图", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // === 分组柱状图 ===
    auto* groupedLabel = new QLabel(u8"分组柱状图", page);
    groupedLabel->setFont(theme->captionFont());
    groupedLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [groupedLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        groupedLabel->setFont(t->captionFont());
        groupedLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(groupedLabel);

    auto* groupedChart = new FluentBarChart(page);
    groupedChart->setBarMode(FluentBarChart::Grouped);
    groupedChart->setOrientation(FluentBarChart::Vertical);
    groupedChart->setValueAxisTitle(u8"销售额 (万)");
    groupedChart->setShowValues(true);
    groupedChart->setMinimumHeight(260);

    QStringList cats = {u8"1月", u8"2月", u8"3月", u8"4月", u8"5月", u8"6月"};

    FluentBarChart::Series bs1;
    bs1.name = u8"产品 A";
    bs1.categories = cats;
    bs1.values = {23, 35, 28, 45, 42, 58};
    groupedChart->addSeries(bs1);

    FluentBarChart::Series bs2;
    bs2.name = u8"产品 B";
    bs2.categories = cats;
    bs2.values = {12, 18, 25, 30, 35, 32};
    groupedChart->addSeries(bs2);

    FluentBarChart::Series bs3;
    bs3.name = u8"产品 C";
    bs3.color = QColor(255, 140, 0);
    bs3.categories = cats;
    bs3.values = {5, 8, 15, 12, 20, 22};
    groupedChart->addSeries(bs3);

    layout->addWidget(groupedChart);

    // === 堆叠柱状图 ===
    auto* stackedLabel = new QLabel(u8"堆叠柱状图", page);
    stackedLabel->setFont(theme->captionFont());
    stackedLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [stackedLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        stackedLabel->setFont(t->captionFont());
        stackedLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(stackedLabel);

    auto* stackedChart = new FluentBarChart(page);
    stackedChart->setBarMode(FluentBarChart::Stacked);
    stackedChart->setOrientation(FluentBarChart::Vertical);
    stackedChart->setValueAxisTitle(u8"总销售额 (万)");
    stackedChart->setMinimumHeight(240);

    FluentBarChart::Series ss1;
    ss1.name = u8"线上";
    ss1.categories = cats;
    ss1.values = {30, 40, 35, 50, 45, 60};
    stackedChart->addSeries(ss1);

    FluentBarChart::Series ss2;
    ss2.name = u8"线下";
    ss2.categories = cats;
    ss2.values = {20, 25, 30, 28, 35, 30};
    stackedChart->addSeries(ss2);

    layout->addWidget(stackedChart);

    // === 水平柱状图 ===
    auto* horizLabel = new QLabel(u8"水平柱状图", page);
    horizLabel->setFont(theme->captionFont());
    horizLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [horizLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        horizLabel->setFont(t->captionFont());
        horizLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(horizLabel);

    auto* horizChart = new FluentBarChart(page);
    horizChart->setOrientation(FluentBarChart::Horizontal);
    horizChart->setValueAxisTitle(u8"评分");
    horizChart->setShowValues(true);
    horizChart->setMinimumHeight(220);

    FluentBarChart::Series hs1;
    hs1.name = u8"评分";
    hs1.color = QColor(16, 137, 108);
    hs1.categories = QStringList() << u8"功能" << u8"性能" << u8"易用性" << u8"稳定性" << u8"外观";
    hs1.values = {85, 72, 90, 78, 95};
    horizChart->addSeries(hs1);

    layout->addWidget(horizChart);

    // === 控制按钮 ===
    auto* ctrlRow = new QHBoxLayout();
    ctrlRow->setSpacing(8);

    auto* reanimateBtn = new FluentButton(u8"重播动画", FluentButtonVariant::Accent, page);
    connect(reanimateBtn, &FluentButton::clicked, this, [groupedChart, stackedChart, horizChart]() {
        groupedChart->setAnimate(true);
        stackedChart->setAnimate(true);
        horizChart->setAnimate(true);

        auto g1 = groupedChart->series(0), g2 = groupedChart->series(1), g3 = groupedChart->series(2);
        groupedChart->clearSeries();
        groupedChart->addSeries(g1); groupedChart->addSeries(g2); groupedChart->addSeries(g3);

        auto s1 = stackedChart->series(0), s2 = stackedChart->series(1);
        stackedChart->clearSeries();
        stackedChart->addSeries(s1); stackedChart->addSeries(s2);

        auto h1 = horizChart->series(0);
        horizChart->clearSeries();
        horizChart->addSeries(h1);
    });

    auto* toggleValuesBtn = new FluentButton(u8"切换数值标签", FluentButtonVariant::Secondary, page);
    connect(toggleValuesBtn, &FluentButton::clicked, this, [groupedChart, horizChart]() {
        groupedChart->setShowValues(!groupedChart->showValues());
        horizChart->setShowValues(!horizChart->showValues());
    });

    auto* toggleGridBtn = new FluentButton(u8"切换网格", FluentButtonVariant::Secondary, page);
    connect(toggleGridBtn, &FluentButton::clicked, this, [groupedChart, stackedChart, horizChart]() {
        groupedChart->setShowGrid(!groupedChart->showGrid());
        stackedChart->setShowGrid(!stackedChart->showGrid());
        horizChart->setShowGrid(!horizChart->showGrid());
    });

    ctrlRow->addWidget(reanimateBtn);
    ctrlRow->addWidget(toggleValuesBtn);
    ctrlRow->addWidget(toggleGridBtn);
    ctrlRow->addStretch();
    layout->addLayout(ctrlRow);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createTimelinePage() {
    auto* page = new QWidget();
    auto* mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"Timeline 历程图", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    mainLayout->addWidget(title);

    // === 垂直历程图（多色节点） ===
    auto* vLabel = new QLabel(u8"垂直历程图（不同节点不同颜色，点击选中）", page);
    vLabel->setFont(theme->captionFont());
    vLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [vLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        vLabel->setFont(t->captionFont());
        vLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    mainLayout->addWidget(vLabel);

    auto* scrollArea = new QScrollArea(page);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto* vTimeline = new FluentTimeline();
    vTimeline->setOrientation(FluentTimeline::Vertical);
    vTimeline->setSelectionEnabled(true);
    vTimeline->addItem(u8"项目启动", u8"完成项目立项和团队组建，确定技术选型方案", u8"2024-01", QColor("#0078D4"));
    vTimeline->addItem(u8"需求分析", u8"用户调研完成，输出需求规格说明书", u8"2024-02", QColor("#10896C"));
    vTimeline->addItem(u8"架构设计", u8"完成系统架构设计和技术评审", u8"2024-03", QColor("#744DA9"), FluentTimeline::Diamond);
    vTimeline->addItem(u8"开发阶段", u8"核心模块开发完成，进入集成测试", u8"2024-05", QColor("#FF8C00"));
    vTimeline->addItem(u8"测试验收", u8"修复全部 P0/P1 缺陷，通过验收测试", u8"2024-07", QColor("#DA3B3B"), FluentTimeline::Ring);
    vTimeline->addItem(u8"正式发布", u8"v1.0 版本上线，用户反馈良好", u8"2024-08", QColor("#0078D4"), FluentTimeline::Icon);

    scrollArea->setWidget(vTimeline);
    mainLayout->addWidget(scrollArea, 1);

    // 状态标签
    auto* vStatus = new QLabel(u8"点击节点查看详情 | 拖拽框选多项 | Ctrl+点击多选", page);
    vStatus->setFont(theme->captionFont());
    vStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [vStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        vStatus->setFont(t->captionFont());
        vStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(vTimeline, &FluentTimeline::itemClicked, this, [vStatus, vTimeline](int index) {
        auto it = vTimeline->item(index);
        vStatus->setText(QString(u8"点击: %1 — %2").arg(it.title).arg(it.timestamp));
    });
    connect(vTimeline, &FluentTimeline::selectionChanged, this, [vStatus](const QVector<int>& indexes) {
        if (indexes.isEmpty()) {
            vStatus->setText(u8"无选中项");
        } else {
            QStringList strs;
            for (int i : indexes) strs << QString::number(i + 1);
            vStatus->setText(QString(u8"选中 %1 项: #%2").arg(indexes.size()).arg(strs.join(", #")));
        }
    });
    mainLayout->addWidget(vStatus);

    // === 控制按钮 ===
    auto* ctrlRow = new QHBoxLayout();
    ctrlRow->setSpacing(8);

    auto* toggleOrientBtn = new FluentButton(u8"切换方向", FluentButtonVariant::Accent, page);
    connect(toggleOrientBtn, &FluentButton::clicked, this, [vTimeline]() {
        vTimeline->setOrientation(
            vTimeline->orientation() == FluentTimeline::Vertical
                ? FluentTimeline::Horizontal : FluentTimeline::Vertical);
    });

    auto* clearSelBtn = new FluentButton(u8"清除选中", FluentButtonVariant::Secondary, page);
    connect(clearSelBtn, &FluentButton::clicked, this, [vTimeline]() {
        vTimeline->clearSelection();
    });

    auto* addBtn = new FluentButton(u8"添加节点", FluentButtonVariant::Secondary, page);
    connect(addBtn, &FluentButton::clicked, this, [vTimeline]() {
        static int counter = 1;
        QStringList colors = QStringList() << "#0078D4" << "#10896C" << "#744DA9" << "#FF8C00" << "#DA3B3B" << "#0099BC";
        QColor c(colors[counter % colors.size()]);
        vTimeline->addItem(QString(u8"新节点 %1").arg(counter),
                           u8"动态添加的历程节点",
                           QString(u8"2024-%1").arg(9 + counter, 2, 10, QChar('0')),
                           c, FluentTimeline::Dot);
        counter++;
    });

    auto* removeBtn = new FluentButton(u8"删除末尾", FluentButtonVariant::Secondary, page);
    connect(removeBtn, &FluentButton::clicked, this, [vTimeline]() {
        if (vTimeline->itemCount() > 0) {
            vTimeline->removeItem(vTimeline->itemCount() - 1);
        }
    });

    ctrlRow->addWidget(toggleOrientBtn);
    ctrlRow->addWidget(clearSelBtn);
    ctrlRow->addWidget(addBtn);
    ctrlRow->addWidget(removeBtn);
    ctrlRow->addStretch();
    mainLayout->addLayout(ctrlRow);

    // === 水平历程图 ===
    auto* hLabel = new QLabel(u8"水平历程图", page);
    hLabel->setFont(theme->captionFont());
    hLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [hLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        hLabel->setFont(t->captionFont());
        hLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    mainLayout->addWidget(hLabel);

    auto* hTimeline = new FluentTimeline();
    hTimeline->setOrientation(FluentTimeline::Horizontal);
    hTimeline->setFixedHeight(100);
    hTimeline->setItemSpacing(120);
    hTimeline->setSelectionEnabled(true);
    hTimeline->addItem(u8"规划", u8"", u8"Q1", QColor("#0078D4"));
    hTimeline->addItem(u8"设计", u8"", u8"Q2", QColor("#10896C"), FluentTimeline::Ring);
    hTimeline->addItem(u8"开发", u8"", u8"Q3", QColor("#FF8C00"), FluentTimeline::Diamond);
    hTimeline->addItem(u8"测试", u8"", u8"Q4", QColor("#DA3B3B"));
    hTimeline->addItem(u8"发布", u8"", u8"Q5", QColor("#744DA9"), FluentTimeline::Icon);

    mainLayout->addWidget(hTimeline);

    return page;
}

QWidget* DemoMainWindow::createPieChartPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"PieChart 饼图", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // === 实心饼图 ===
    auto* pieLabel = new QLabel(u8"实心饼图", page);
    pieLabel->setFont(theme->captionFont());
    pieLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [pieLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        pieLabel->setFont(t->captionFont());
        pieLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(pieLabel);

    auto* pieChart = new FluentPieChart(page);
    pieChart->setPieStyle(FluentPieChart::Pie);
    pieChart->setMinimumHeight(300);
    pieChart->setTitle(u8"市场份额");
    pieChart->addSlice(u8"产品 A", 35, QColor("#0078D4"));
    pieChart->addSlice(u8"产品 B", 25, QColor("#10896C"));
    pieChart->addSlice(u8"产品 C", 18, QColor("#DA3B3B"));
    pieChart->addSlice(u8"产品 D", 12, QColor("#FF8C00"));
    pieChart->addSlice(u8"其他", 10, QColor("#744DA9"));
    layout->addWidget(pieChart);

    // === 环形图 ===
    auto* donutLabel = new QLabel(u8"环形图（Donut）", page);
    donutLabel->setFont(theme->captionFont());
    donutLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [donutLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        donutLabel->setFont(t->captionFont());
        donutLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(donutLabel);

    auto* donutChart = new FluentPieChart(page);
    donutChart->setPieStyle(FluentPieChart::Donut);
    donutChart->setHoleRatio(0.6);
    donutChart->setMinimumHeight(300);
    donutChart->setTitle(u8"支出分布");
    donutChart->addSlice(u8"研发", 40, QColor("#0078D4"));
    donutChart->addSlice(u8"市场", 22, QColor("#10896C"));
    donutChart->addSlice(u8"运营", 15, QColor("#FF8C00"));
    donutChart->addSlice(u8"人力", 13, QColor("#744DA9"));
    donutChart->addSlice(u8"行政", 10, QColor("#DA3B3B"));
    layout->addWidget(donutChart);

    // 状态标签
    auto* statusLabel = new QLabel(u8"悬停切片查看详情 | 点击选中切片", page);
    statusLabel->setFont(theme->captionFont());
    statusLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [statusLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        statusLabel->setFont(t->captionFont());
        statusLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });

    connect(pieChart, &FluentPieChart::sliceClicked, this, [statusLabel, pieChart](int index) {
        auto s = pieChart->slice(index);
        statusLabel->setText(QString(u8"饼图选中: %1 — 值: %2").arg(s.name).arg(s.value));
    });
    connect(donutChart, &FluentPieChart::sliceClicked, this, [statusLabel, donutChart](int index) {
        auto s = donutChart->slice(index);
        statusLabel->setText(QString(u8"环形图选中: %1 — 值: %2").arg(s.name).arg(s.value));
    });
    layout->addWidget(statusLabel);

    // === 控制按钮 ===
    auto* ctrlRow = new QHBoxLayout();
    ctrlRow->setSpacing(8);

    auto* toggleStyleBtn = new FluentButton(u8"切换饼图/环形", FluentButtonVariant::Accent, page);
    connect(toggleStyleBtn, &FluentButton::clicked, this, [pieChart]() {
        pieChart->setPieStyle(pieChart->pieStyle() == FluentPieChart::Pie
                                  ? FluentPieChart::Donut : FluentPieChart::Pie);
    });

    auto* toggleLabelBtn = new FluentButton(u8"切换标签", FluentButtonVariant::Secondary, page);
    connect(toggleLabelBtn, &FluentButton::clicked, this, [pieChart, donutChart]() {
        pieChart->setShowLabels(!pieChart->showLabels());
        donutChart->setShowLabels(!donutChart->showLabels());
    });

    auto* togglePercentBtn = new FluentButton(u8"切换百分比", FluentButtonVariant::Secondary, page);
    connect(togglePercentBtn, &FluentButton::clicked, this, [pieChart, donutChart]() {
        pieChart->setShowPercent(!pieChart->showPercent());
        donutChart->setShowPercent(!donutChart->showPercent());
    });

    auto* reanimateBtn = new FluentButton(u8"重播动画", FluentButtonVariant::Secondary, page);
    connect(reanimateBtn, &FluentButton::clicked, this, [pieChart, donutChart]() {
        pieChart->setAnimate(true);
        donutChart->setAnimate(true);
        // 重新添加数据触发动画
        auto p1 = pieChart->slice(0), p2 = pieChart->slice(1), p3 = pieChart->slice(2),
             p4 = pieChart->slice(3), p5 = pieChart->slice(4);
        pieChart->clearSlices();
        pieChart->addSlice(p1); pieChart->addSlice(p2); pieChart->addSlice(p3);
        pieChart->addSlice(p4); pieChart->addSlice(p5);

        auto d1 = donutChart->slice(0), d2 = donutChart->slice(1), d3 = donutChart->slice(2),
             d4 = donutChart->slice(3), d5 = donutChart->slice(4);
        donutChart->clearSlices();
        donutChart->addSlice(d1); donutChart->addSlice(d2); donutChart->addSlice(d3);
        donutChart->addSlice(d4); donutChart->addSlice(d5);
    });

    ctrlRow->addWidget(toggleStyleBtn);
    ctrlRow->addWidget(toggleLabelBtn);
    ctrlRow->addWidget(togglePercentBtn);
    ctrlRow->addWidget(reanimateBtn);
    ctrlRow->addStretch();
    layout->addLayout(ctrlRow);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createSliderPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(20);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"Slider 滑块", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // === 基础水平滑块 ===
    auto* basicLabel = new QLabel(u8"基础水平滑块", page);
    basicLabel->setFont(theme->captionFont());
    basicLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [basicLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        basicLabel->setFont(t->captionFont());
        basicLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(basicLabel);

    auto* basicSlider = new FluentSlider(Qt::Horizontal, page);
    basicSlider->setRange(0, 100);
    basicSlider->setValue(42);
    basicSlider->setSingleStep(1);
    basicSlider->setShowValueLabel(true);
    layout->addWidget(basicSlider);

    auto* basicStatus = new QLabel(u8"当前值: 42", page);
    basicStatus->setFont(theme->captionFont());
    basicStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [basicStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        basicStatus->setFont(t->captionFont());
        basicStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(basicSlider, &FluentSlider::valueChanged, this, [basicStatus](qreal val) {
        basicStatus->setText(QString(u8"当前值: %1").arg(qRound(val)));
    });
    layout->addWidget(basicStatus);

    // === 带刻度滑块 ===
    auto* tickLabel = new QLabel(u8"带刻度滑块", page);
    tickLabel->setFont(theme->captionFont());
    tickLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [tickLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        tickLabel->setFont(t->captionFont());
        tickLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(tickLabel);

    auto* tickSlider = new FluentSlider(Qt::Horizontal, page);
    tickSlider->setRange(0, 100);
    tickSlider->setValue(60);
    tickSlider->setSingleStep(10);
    tickSlider->setShowTicks(true);
    tickSlider->setTickPosition(FluentSlider::TicksBelow);
    tickSlider->setTickInterval(10);
    layout->addWidget(tickSlider);

    auto* tickStatus = new QLabel(u8"当前值: 60", page);
    tickStatus->setFont(theme->captionFont());
    tickStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [tickStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        tickStatus->setFont(t->captionFont());
        tickStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(tickSlider, &FluentSlider::valueChanged, this, [tickStatus](qreal val) {
        tickStatus->setText(QString(u8"当前值: %1").arg(qRound(val)));
    });
    layout->addWidget(tickStatus);

    // === 范围滑块（双滑块） ===
    auto* rangeLabel = new QLabel(u8"范围滑块（双滑块）", page);
    rangeLabel->setFont(theme->captionFont());
    rangeLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [rangeLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        rangeLabel->setFont(t->captionFont());
        rangeLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(rangeLabel);

    auto* rangeSlider = new FluentSlider(Qt::Horizontal, page);
    rangeSlider->setRange(0, 100);
    rangeSlider->setValue(25);
    rangeSlider->setSecondValue(75);
    rangeSlider->setSingleStep(1);
    rangeSlider->setRangeMode(true);
    rangeSlider->setShowValueLabel(true);
    layout->addWidget(rangeSlider);

    auto* rangeStatus = new QLabel(u8"范围: 25 ~ 75", page);
    rangeStatus->setFont(theme->captionFont());
    rangeStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [rangeStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        rangeStatus->setFont(t->captionFont());
        rangeStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(rangeSlider, &FluentSlider::rangeChanged, this, [rangeStatus](qreal first, qreal second) {
        rangeStatus->setText(QString(u8"范围: %1 ~ %2").arg(qRound(first)).arg(qRound(second)));
    });
    layout->addWidget(rangeStatus);

    // === 浮点滑块 ===
    auto* floatLabel = new QLabel(u8"浮点滑块（音量调节）", page);
    floatLabel->setFont(theme->captionFont());
    floatLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [floatLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        floatLabel->setFont(t->captionFont());
        floatLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(floatLabel);

    auto* floatSlider = new FluentSlider(Qt::Horizontal, page);
    floatSlider->setRange(0.0, 1.0);
    floatSlider->setValue(0.65);
    floatSlider->setSingleStep(0.01);
    floatSlider->setShowValueLabel(true);
    layout->addWidget(floatSlider);

    auto* floatStatus = new QLabel(u8"音量: 65%", page);
    floatStatus->setFont(theme->captionFont());
    floatStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [floatStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        floatStatus->setFont(t->captionFont());
        floatStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(floatSlider, &FluentSlider::valueChanged, this, [floatStatus](qreal val) {
        floatStatus->setText(QString(u8"音量: %1%").arg(qRound(val * 100)));
    });
    layout->addWidget(floatStatus);

    // === 垂直滑块 ===
    auto* vertLabel = new QLabel(u8"垂直滑块", page);
    vertLabel->setFont(theme->captionFont());
    vertLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [vertLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        vertLabel->setFont(t->captionFont());
        vertLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(vertLabel);

    auto* vertRow = new QHBoxLayout();
    vertRow->setSpacing(24);

    auto* vSlider1 = new FluentSlider(Qt::Vertical, page);
    vSlider1->setRange(0, 100);
    vSlider1->setValue(30);
    vSlider1->setFixedHeight(150);
    vSlider1->setShowValueLabel(true);

    auto* vSlider2 = new FluentSlider(Qt::Vertical, page);
    vSlider2->setRange(0, 100);
    vSlider2->setValue(70);
    vSlider2->setFixedHeight(150);
    vSlider2->setShowValueLabel(true);
    vSlider2->setShowTicks(true);
    vSlider2->setTickPosition(FluentSlider::TicksBelow);
    vSlider2->setTickInterval(20);

    auto* vSlider3 = new FluentSlider(Qt::Vertical, page);
    vSlider3->setRange(0, 100);
    vSlider3->setValue(50);
    vSlider3->setSecondValue(90);
    vSlider3->setFixedHeight(150);
    vSlider3->setRangeMode(true);
    vSlider3->setShowValueLabel(true);

    vertRow->addWidget(vSlider1);
    vertRow->addWidget(vSlider2);
    vertRow->addWidget(vSlider3);
    vertRow->addStretch();
    layout->addLayout(vertRow);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createDatePickerPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(20);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"DatePicker 日期选择器", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // === 基础日期选择 ===
    auto* basicLabel = new QLabel(u8"基础日期选择", page);
    basicLabel->setFont(theme->captionFont());
    basicLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [basicLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        basicLabel->setFont(t->captionFont());
        basicLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(basicLabel);

    auto* basicPicker = new FluentDatePicker(page);
    basicPicker->setPlaceholder(u8"请选择日期...");
    basicPicker->setDate(QDate::currentDate());
    layout->addWidget(basicPicker);

    auto* basicStatus = new QLabel(QString(u8"选中日期: %1").arg(QDate::currentDate().toString("yyyy-MM-dd")), page);
    basicStatus->setFont(theme->captionFont());
    basicStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [basicStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        basicStatus->setFont(t->captionFont());
        basicStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(basicPicker, &FluentDatePicker::dateChanged, this, [basicStatus](const QDate& date) {
        basicStatus->setText(QString(u8"选中日期: %1").arg(date.toString("yyyy-MM-dd")));
    });
    layout->addWidget(basicStatus);

    // === 自定义格式 ===
    auto* formatLabel = new QLabel(u8"自定义日期格式", page);
    formatLabel->setFont(theme->captionFont());
    formatLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [formatLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        formatLabel->setFont(t->captionFont());
        formatLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(formatLabel);

    auto* formatRow = new QHBoxLayout();
    formatRow->setSpacing(16);

    auto* picker1 = new FluentDatePicker(page);
    picker1->setFormat(u8"yyyy年MM月dd日");
    picker1->setDate(QDate::currentDate());

    auto* picker2 = new FluentDatePicker(page);
    picker2->setFormat("dd/MM/yyyy");
    picker2->setDate(QDate::currentDate());

    auto* picker3 = new FluentDatePicker(page);
    picker3->setFormat("MM.dd.yyyy");
    picker3->setPlaceholder(u8"选择日期...");
    picker3->setDate(QDate::currentDate());

    formatRow->addWidget(picker1);
    formatRow->addWidget(picker2);
    formatRow->addWidget(picker3);
    formatRow->addStretch();
    layout->addLayout(formatRow);

    // === 带限制的日期选择 ===
    auto* limitLabel = new QLabel(u8"限制日期范围（仅可选 2024 年）", page);
    limitLabel->setFont(theme->captionFont());
    limitLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [limitLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        limitLabel->setFont(t->captionFont());
        limitLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(limitLabel);

    auto* limitedPicker = new FluentDatePicker(page);
    limitedPicker->setMinimumDate(QDate(2024, 1, 1));
    limitedPicker->setMaximumDate(QDate(2024, 12, 31));
    limitedPicker->setDate(QDate(2024, 6, 15));
    limitedPicker->setPlaceholder(u8"仅限2024年");
    layout->addWidget(limitedPicker);

    auto* limitedStatus = new QLabel(u8"选中日期: 2024-06-15", page);
    limitedStatus->setFont(theme->captionFont());
    limitedStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [limitedStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        limitedStatus->setFont(t->captionFont());
        limitedStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(limitedPicker, &FluentDatePicker::dateChanged, this, [limitedStatus](const QDate& date) {
        limitedStatus->setText(QString(u8"选中日期: %1").arg(date.toString("yyyy-MM-dd")));
    });
    layout->addWidget(limitedStatus);

    // === 操作按钮 ===
    auto* ctrlRow = new QHBoxLayout();
    ctrlRow->setSpacing(8);

    auto* todayBtn = new FluentButton(u8"设为今天", FluentButtonVariant::Accent, page);
    connect(todayBtn, &FluentButton::clicked, this, [basicPicker]() {
        basicPicker->setDate(QDate::currentDate());
    });

    auto* clearBtn = new FluentButton(u8"清空", FluentButtonVariant::Secondary, page);
    connect(clearBtn, &FluentButton::clicked, this, [basicPicker]() {
        basicPicker->clear();
    });

    auto* formatBtn = new FluentButton(u8"切换格式", FluentButtonVariant::Secondary, page);
    connect(formatBtn, &FluentButton::clicked, this, [basicPicker]() {
        QString fmt = basicPicker->format();
        if (fmt == u8"yyyy-MM-dd") basicPicker->setFormat(u8"yyyy年MM月dd日");
        else if (fmt == u8"yyyy年MM月dd日") basicPicker->setFormat("dd/MM/yyyy");
        else basicPicker->setFormat(u8"yyyy-MM-dd");
    });

    ctrlRow->addWidget(todayBtn);
    ctrlRow->addWidget(clearBtn);
    ctrlRow->addWidget(formatBtn);
    ctrlRow->addStretch();
    layout->addLayout(ctrlRow);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createTimePickerPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(20);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"TimePicker 时间选择器", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // === 24小时制 ===
    auto* h24Label = new QLabel(u8"24小时制", page);
    h24Label->setFont(theme->captionFont());
    h24Label->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [h24Label]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        h24Label->setFont(t->captionFont());
        h24Label->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(h24Label);

    auto* picker24 = new FluentTimePicker(page);
    picker24->setIs24Hour(true);
    picker24->setTime(QTime(14, 30));
    layout->addWidget(picker24);

    auto* status24 = new QLabel(u8"当前时间: 14:30", page);
    status24->setFont(theme->captionFont());
    status24->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [status24]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        status24->setFont(t->captionFont());
        status24->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(picker24, &FluentTimePicker::timeChanged, this, [status24](const QTime& time) {
        status24->setText(QString(u8"当前时间: %1").arg(time.toString(u8"HH:mm")));
    });
    layout->addWidget(status24);

    // === 12小时制 ===
    auto* h12Label = new QLabel(u8"12小时制", page);
    h12Label->setFont(theme->captionFont());
    h12Label->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [h12Label]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        h12Label->setFont(t->captionFont());
        h12Label->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(h12Label);

    auto* picker12 = new FluentTimePicker(page);
    picker12->setIs24Hour(false);
    picker12->setTime(QTime(9, 15));
    layout->addWidget(picker12);

    auto* status12 = new QLabel(u8"当前时间: 09:15 AM", page);
    status12->setFont(theme->captionFont());
    status12->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [status12]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        status12->setFont(t->captionFont());
        status12->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(picker12, &FluentTimePicker::timeChanged, this, [status12](const QTime& time) {
        status12->setText(QString(u8"当前时间: %1").arg(time.toString(u8"hh:mm AP")));
    });
    layout->addWidget(status12);

    // === 带秒 ===
    auto* secLabel = new QLabel(u8"带秒选择", page);
    secLabel->setFont(theme->captionFont());
    secLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [secLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        secLabel->setFont(t->captionFont());
        secLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(secLabel);

    auto* pickerSec = new FluentTimePicker(page);
    pickerSec->setIs24Hour(true);
    pickerSec->setShowSeconds(true);
    pickerSec->setTime(QTime(18, 45, 30));
    layout->addWidget(pickerSec);

    auto* statusSec = new QLabel(u8"当前时间: 18:45:30", page);
    statusSec->setFont(theme->captionFont());
    statusSec->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [statusSec]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        statusSec->setFont(t->captionFont());
        statusSec->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(pickerSec, &FluentTimePicker::timeChanged, this, [statusSec](const QTime& time) {
        statusSec->setText(QString(u8"当前时间: %1").arg(time.toString(u8"HH:mm:ss")));
    });
    layout->addWidget(statusSec);

    // === 控制按钮 ===
    auto* ctrlRow = new QHBoxLayout();
    ctrlRow->setSpacing(8);

    auto* nowBtn = new FluentButton(u8"设为当前时间", FluentButtonVariant::Accent, page);
    connect(nowBtn, &FluentButton::clicked, this, [picker24, picker12, pickerSec]() {
        QTime now = QTime::currentTime();
        picker24->setTime(now);
        picker12->setTime(now);
        pickerSec->setTime(now);
    });

    auto* clearBtn = new FluentButton(u8"清空", FluentButtonVariant::Secondary, page);
    connect(clearBtn, &FluentButton::clicked, this, [picker24, picker12, pickerSec]() {
        picker24->clear();
        picker12->clear();
        pickerSec->clear();
    });

    ctrlRow->addWidget(nowBtn);
    ctrlRow->addWidget(clearBtn);
    ctrlRow->addStretch();
    layout->addLayout(ctrlRow);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createColorPickerPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(32, 28, 32, 28);
    layout->setSpacing(20);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"ColorPicker 颜色选择器", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    layout->addWidget(title);

    auto* desc = new QLabel(u8"点击输入框弹出预设色板，支持透明度滑块和 HEX 输入。", page);
    desc->setWordWrap(true);
    desc->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    layout->addWidget(desc);

    // === 基础选择 ===
    auto* basicLabel = new QLabel(u8"基础颜色选择", page);
    basicLabel->setFont(theme->bodyStrongFont());
    basicLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    layout->addWidget(basicLabel);

    auto* basicPicker = new FluentColorPicker(page);
    basicPicker->setPlaceholder(u8"请选择颜色...");

    auto* basicStatus = new QLabel(u8"未选择颜色", page);
    basicStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [basicStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        basicStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(basicPicker, &FluentColorPicker::colorChanged, this, [basicStatus](const QColor& color) {
        basicStatus->setText(QString(u8"选中颜色: %1 (R:%2 G:%3 B:%4)")
            .arg(color.name(QColor::HexArgb))
            .arg(color.red()).arg(color.green()).arg(color.blue()));
    });

    layout->addWidget(basicPicker);
    layout->addWidget(basicStatus);

    // === 带透明度 ===
    auto* alphaLabel = new QLabel(u8"带透明度选择", page);
    alphaLabel->setFont(theme->bodyStrongFont());
    alphaLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    layout->addWidget(alphaLabel);

    auto* alphaPicker = new FluentColorPicker(page);
    alphaPicker->setShowAlpha(true);
    alphaPicker->setPlaceholder(u8"选择颜色（含透明度）...");

    auto* alphaStatus = new QLabel(u8"未选择颜色", page);
    alphaStatus->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [alphaStatus]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        alphaStatus->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    connect(alphaPicker, &FluentColorPicker::colorChanged, this, [alphaStatus](const QColor& color) {
        alphaStatus->setText(QString(u8"选中颜色: %1 (Alpha: %2)")
            .arg(color.name(QColor::HexArgb)).arg(color.alpha()));
    });

    layout->addWidget(alphaPicker);
    layout->addWidget(alphaStatus);

    // === 预设初始颜色 ===
    auto* initLabel = new QLabel(u8"预设初始颜色", page);
    initLabel->setFont(theme->bodyStrongFont());
    initLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    layout->addWidget(initLabel);

    auto* initRow = new QHBoxLayout();
    initRow->setSpacing(16);

    auto* picker1 = new FluentColorPicker(page);
    picker1->setColor(QColor("#0078D4"));

    auto* picker2 = new FluentColorPicker(page);
    picker2->setColor(QColor("#E74C3C"));

    auto* picker3 = new FluentColorPicker(page);
    picker3->setColor(QColor("#2ECC71"));

    initRow->addWidget(picker1);
    initRow->addWidget(picker2);
    initRow->addWidget(picker3);
    initRow->addStretch();
    layout->addLayout(initRow);

    // === 控制按钮 ===
    auto* ctrlRow = new QHBoxLayout();
    ctrlRow->setSpacing(8);

    auto* randomBtn = new FluentButton(u8"随机颜色", FluentButtonVariant::Accent, page);
    connect(randomBtn, &FluentButton::clicked, this, [basicPicker, alphaPicker]() {
        QColor c(QRandomGenerator::global()->bounded(256),
                 QRandomGenerator::global()->bounded(256),
                 QRandomGenerator::global()->bounded(256));
        basicPicker->setColor(c);
        alphaPicker->setColor(c);
    });

    auto* clearBtn = new FluentButton(u8"清空", FluentButtonVariant::Secondary, page);
    connect(clearBtn, &FluentButton::clicked, this, [basicPicker, alphaPicker]() {
        basicPicker->clear();
        alphaPicker->clear();
    });

    ctrlRow->addWidget(randomBtn);
    ctrlRow->addWidget(clearBtn);
    ctrlRow->addStretch();
    layout->addLayout(ctrlRow);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createProgressRingPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"ProgressRing 进度环", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // --- 基础进度环 ---
    auto* basicLabel = new QLabel(u8"基础进度环", page);
    basicLabel->setFont(theme->captionFont());
    basicLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [basicLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        basicLabel->setFont(t->captionFont());
        basicLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(basicLabel);

    auto* basicRow = new QHBoxLayout();
    basicRow->setSpacing(24);

    auto* ring1 = new FluentProgressRing(page);
    ring1->setValue(25);
    ring1->setShowValue(true);
    ring1->setFixedSize(80, 80);

    auto* ring2 = new FluentProgressRing(page);
    ring2->setValue(50);
    ring2->setShowValue(true);
    ring2->setFixedSize(80, 80);

    auto* ring3 = new FluentProgressRing(page);
    ring3->setValue(75);
    ring3->setShowValue(true);
    ring3->setFixedSize(80, 80);

    auto* ring4 = new FluentProgressRing(page);
    ring4->setValue(100);
    ring4->setShowValue(true);
    ring4->setFixedSize(80, 80);

    basicRow->addWidget(ring1);
    basicRow->addWidget(ring2);
    basicRow->addWidget(ring3);
    basicRow->addWidget(ring4);
    basicRow->addStretch();
    layout->addLayout(basicRow);

    // --- 不确定模式 ---
    auto* indetLabel = new QLabel(u8"不确定模式（加载中）", page);
    indetLabel->setFont(theme->captionFont());
    indetLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [indetLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        indetLabel->setFont(t->captionFont());
        indetLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(indetLabel);

    auto* indetRow = new QHBoxLayout();
    indetRow->setSpacing(24);

    auto* indet1 = new FluentProgressRing(page);
    indet1->setIndeterminate(true);
    indet1->setFixedSize(48, 48);
    indet1->setStrokeWidth(3);

    auto* indet2 = new FluentProgressRing(page);
    indet2->setIndeterminate(true);
    indet2->setFixedSize(64, 64);

    auto* indet3 = new FluentProgressRing(page);
    indet3->setIndeterminate(true);
    indet3->setFixedSize(80, 80);

    auto* indet4 = new FluentProgressRing(page);
    indet4->setIndeterminate(true);
    indet4->setFixedSize(96, 96);
    indet4->setStrokeWidth(8);

    indetRow->addWidget(indet1);
    indetRow->addWidget(indet2);
    indetRow->addWidget(indet3);
    indetRow->addWidget(indet4);
    indetRow->addStretch();
    layout->addLayout(indetRow);

    // --- 自定义颜色 ---
    auto* colorLabel = new QLabel(u8"自定义颜色", page);
    colorLabel->setFont(theme->captionFont());
    colorLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [colorLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        colorLabel->setFont(t->captionFont());
        colorLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(colorLabel);

    auto* colorRow = new QHBoxLayout();
    colorRow->setSpacing(24);

    auto* cRing1 = new FluentProgressRing(page);
    cRing1->setValue(60);
    cRing1->setShowValue(true);
    cRing1->setCustomColor(QColor("#E74C3C"));
    cRing1->setFixedSize(80, 80);

    auto* cRing2 = new FluentProgressRing(page);
    cRing2->setValue(60);
    cRing2->setShowValue(true);
    cRing2->setCustomColor(QColor("#2ECC71"));
    cRing2->setFixedSize(80, 80);

    auto* cRing3 = new FluentProgressRing(page);
    cRing3->setValue(60);
    cRing3->setShowValue(true);
    cRing3->setCustomColor(QColor("#F39C12"));
    cRing3->setFixedSize(80, 80);

    auto* cRing4 = new FluentProgressRing(page);
    cRing4->setValue(60);
    cRing4->setShowValue(true);
    cRing4->setCustomColor(QColor("#9B59B6"));
    cRing4->setFixedSize(80, 80);

    colorRow->addWidget(cRing1);
    colorRow->addWidget(cRing2);
    colorRow->addWidget(cRing3);
    colorRow->addWidget(cRing4);
    colorRow->addStretch();
    layout->addLayout(colorRow);

    // --- 渐变色 ---
    auto* gradLabel = new QLabel(u8"渐变色进度环", page);
    gradLabel->setFont(theme->captionFont());
    gradLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [gradLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        gradLabel->setFont(t->captionFont());
        gradLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(gradLabel);

    auto* gradRow = new QHBoxLayout();
    gradRow->setSpacing(24);

    auto* gRing1 = new FluentProgressRing(page);
    gRing1->setValue(45);
    gRing1->setShowValue(true);
    gRing1->setUseGradient(true);
    gRing1->setFixedSize(90, 90);

    auto* gRing2 = new FluentProgressRing(page);
    gRing2->setValue(70);
    gRing2->setShowValue(true);
    gRing2->setUseGradient(true);
    gRing2->setCustomColor(QColor("#E74C3C"));
    gRing2->setFixedSize(90, 90);

    auto* gRing3 = new FluentProgressRing(page);
    gRing3->setValue(90);
    gRing3->setShowValue(true);
    gRing3->setUseGradient(true);
    gRing3->setCustomColor(QColor("#2ECC71"));
    gRing3->setFixedSize(90, 90);

    gradRow->addWidget(gRing1);
    gradRow->addWidget(gRing2);
    gradRow->addWidget(gRing3);
    gradRow->addStretch();
    layout->addLayout(gradRow);

    // --- 不同线宽 ---
    auto* strokeLabel = new QLabel(u8"不同线宽", page);
    strokeLabel->setFont(theme->captionFont());
    strokeLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [strokeLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        strokeLabel->setFont(t->captionFont());
        strokeLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(strokeLabel);

    auto* strokeRow = new QHBoxLayout();
    strokeRow->setSpacing(24);

    auto* sRing1 = new FluentProgressRing(page);
    sRing1->setValue(60);
    sRing1->setShowValue(true);
    sRing1->setStrokeWidth(3);
    sRing1->setFixedSize(80, 80);

    auto* sRing2 = new FluentProgressRing(page);
    sRing2->setValue(60);
    sRing2->setShowValue(true);
    sRing2->setStrokeWidth(6);
    sRing2->setFixedSize(80, 80);

    auto* sRing3 = new FluentProgressRing(page);
    sRing3->setValue(60);
    sRing3->setShowValue(true);
    sRing3->setStrokeWidth(10);
    sRing3->setFixedSize(80, 80);

    auto* sRing4 = new FluentProgressRing(page);
    sRing4->setValue(60);
    sRing4->setShowValue(true);
    sRing4->setStrokeWidth(16);
    sRing4->setFixedSize(80, 80);

    strokeRow->addWidget(sRing1);
    strokeRow->addWidget(sRing2);
    strokeRow->addWidget(sRing3);
    strokeRow->addWidget(sRing4);
    strokeRow->addStretch();
    layout->addLayout(strokeRow);

    // --- 带标签 ---
    auto* labelSection = new QLabel(u8"带标签文字", page);
    labelSection->setFont(theme->captionFont());
    labelSection->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [labelSection]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        labelSection->setFont(t->captionFont());
        labelSection->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(labelSection);

    auto* labelRow = new QHBoxLayout();
    labelRow->setSpacing(24);

    auto* lRing1 = new FluentProgressRing(page);
    lRing1->setValue(85);
    lRing1->setShowValue(true);
    lRing1->setLabel(u8"CPU");
    lRing1->setFixedSize(100, 100);

    auto* lRing2 = new FluentProgressRing(page);
    lRing2->setValue(52);
    lRing2->setShowValue(true);
    lRing2->setLabel(u8"内存");
    lRing2->setCustomColor(QColor("#3498DB"));
    lRing2->setFixedSize(100, 100);

    auto* lRing3 = new FluentProgressRing(page);
    lRing3->setValue(30);
    lRing3->setShowValue(true);
    lRing3->setLabel(u8"磁盘");
    lRing3->setCustomColor(QColor("#E67E22"));
    lRing3->setFixedSize(100, 100);

    labelRow->addWidget(lRing1);
    labelRow->addWidget(lRing2);
    labelRow->addWidget(lRing3);
    labelRow->addStretch();
    layout->addLayout(labelRow);

    // --- 控制按钮 ---
    auto* ctrlRow = new QHBoxLayout();
    ctrlRow->setSpacing(8);

    auto* lessBtn = new FluentButton(u8"-10", FluentButtonVariant::Secondary, page);
    auto* moreBtn = new FluentButton(u8"+10", FluentButtonVariant::Secondary, page);
    auto* resetBtn = new FluentButton(u8"重置", FluentButtonVariant::Secondary, page);
    auto* toggleIndetBtn = new FluentButton(u8"切换不确定", FluentButtonVariant::Secondary, page);

    connect(lessBtn, &FluentButton::clicked, this, [ring1, ring2, ring3, ring4]() {
        ring1->setValue(ring1->value() - 10);
        ring2->setValue(ring2->value() - 10);
        ring3->setValue(ring3->value() - 10);
        ring4->setValue(ring4->value() - 10);
    });
    connect(moreBtn, &FluentButton::clicked, this, [ring1, ring2, ring3, ring4]() {
        ring1->setValue(ring1->value() + 10);
        ring2->setValue(ring2->value() + 10);
        ring3->setValue(ring3->value() + 10);
        ring4->setValue(ring4->value() + 10);
    });
    connect(resetBtn, &FluentButton::clicked, this, [ring1, ring2, ring3, ring4]() {
        ring1->setValue(25);
        ring2->setValue(50);
        ring3->setValue(75);
        ring4->setValue(100);
    });
    connect(toggleIndetBtn, &FluentButton::clicked, this, [indet2]() {
        indet2->setIndeterminate(!indet2->indeterminate());
        if (!indet2->indeterminate()) {
            indet2->setValue(42);
            indet2->setShowValue(true);
        } else {
            indet2->setShowValue(false);
        }
    });

    ctrlRow->addWidget(lessBtn);
    ctrlRow->addWidget(moreBtn);
    ctrlRow->addWidget(resetBtn);
    ctrlRow->addWidget(toggleIndetBtn);
    ctrlRow->addStretch();
    layout->addLayout(ctrlRow);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createDrawerPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"Drawer 抽屉", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // --- 四个方向 ---
    auto* dirLabel = new QLabel(u8"四个方向", page);
    dirLabel->setFont(theme->captionFont());
    dirLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [dirLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        dirLabel->setFont(t->captionFont());
        dirLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(dirLabel);

    auto* dirRow = new QHBoxLayout();
    dirRow->setSpacing(8);

    // 获取主窗口用于 Drawer 的父控件
    auto* mainWindow = this;

    auto* leftBtn = new FluentButton(u8"左侧抽屉", FluentButtonVariant::Primary, page);
    auto* rightBtn = new FluentButton(u8"右侧抽屉", FluentButtonVariant::Primary, page);
    auto* topBtn = new FluentButton(u8"顶部抽屉", FluentButtonVariant::Primary, page);
    auto* bottomBtn = new FluentButton(u8"底部抽屉", FluentButtonVariant::Primary, page);

    connect(leftBtn, &FluentButton::clicked, this, [mainWindow]() {
        auto* drawer = new FluentDrawer(mainWindow);
        drawer->setEdge(FluentDrawer::Left);
        drawer->setDrawerWidth(280);
        drawer->setTitle(u8"左侧导航");
        auto* content = new QWidget();
        auto* cLayout = new QVBoxLayout(content);
        cLayout->setContentsMargins(20, 16, 20, 16);
        for (int i = 1; i <= 5; ++i) {
            cLayout->addWidget(new QLabel(QString(u8"菜单项 %1").arg(i)));
        }
        cLayout->addStretch();
        drawer->setContent(content);
        drawer->resize(mainWindow->size());
        drawer->open();
        connect(drawer, &FluentDrawer::closed, drawer, &QObject::deleteLater);
    });

    connect(rightBtn, &FluentButton::clicked, this, [mainWindow]() {
        auto* drawer = new FluentDrawer(mainWindow);
        drawer->setEdge(FluentDrawer::Right);
        drawer->setDrawerWidth(320);
        drawer->setTitle(u8"属性面板");
        auto* content = new QWidget();
        auto* cLayout = new QVBoxLayout(content);
        cLayout->setContentsMargins(20, 16, 20, 16);
        cLayout->addWidget(new QLabel(u8"名称: FluentDrawer"));
        cLayout->addWidget(new QLabel(u8"版本: 1.0.0"));
        cLayout->addWidget(new QLabel(u8"方向: 右侧"));
        cLayout->addStretch();
        drawer->setContent(content);
        drawer->resize(mainWindow->size());
        drawer->open();
        connect(drawer, &FluentDrawer::closed, drawer, &QObject::deleteLater);
    });

    connect(topBtn, &FluentButton::clicked, this, [mainWindow]() {
        auto* drawer = new FluentDrawer(mainWindow);
        drawer->setEdge(FluentDrawer::Top);
        drawer->setDrawerHeight(200);
        drawer->setTitle(u8"顶部工具栏");
        auto* content = new QWidget();
        auto* cLayout = new QHBoxLayout(content);
        cLayout->setContentsMargins(20, 8, 20, 8);
        cLayout->addWidget(new QLabel(u8"🔧 工具1"));
        cLayout->addWidget(new QLabel(u8"🎨 工具2"));
        cLayout->addWidget(new QLabel(u8"📝 工具3"));
        cLayout->addStretch();
        drawer->setContent(content);
        drawer->resize(mainWindow->size());
        drawer->open();
        connect(drawer, &FluentDrawer::closed, drawer, &QObject::deleteLater);
    });

    connect(bottomBtn, &FluentButton::clicked, this, [mainWindow]() {
        auto* drawer = new FluentDrawer(mainWindow);
        drawer->setEdge(FluentDrawer::Bottom);
        drawer->setDrawerHeight(180);
        drawer->setTitle(u8"底部控制台");
        auto* content = new QWidget();
        auto* cLayout = new QVBoxLayout(content);
        cLayout->setContentsMargins(20, 8, 20, 8);
        auto* logLabel = new QLabel(u8"> 组件初始化完成\n> 主题已加载\n> 就绪");
        logLabel->setStyleSheet("font-family: Consolas, monospace;");
        cLayout->addWidget(logLabel);
        cLayout->addStretch();
        drawer->setContent(content);
        drawer->resize(mainWindow->size());
        drawer->open();
        connect(drawer, &FluentDrawer::closed, drawer, &QObject::deleteLater);
    });

    dirRow->addWidget(leftBtn);
    dirRow->addWidget(rightBtn);
    dirRow->addWidget(topBtn);
    dirRow->addWidget(bottomBtn);
    dirRow->addStretch();
    layout->addLayout(dirRow);

    // --- 自定义宽度 ---
    auto* sizeLabel = new QLabel(u8"自定义尺寸", page);
    sizeLabel->setFont(theme->captionFont());
    sizeLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [sizeLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        sizeLabel->setFont(t->captionFont());
        sizeLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(sizeLabel);

    auto* sizeRow = new QHBoxLayout();
    sizeRow->setSpacing(8);

    auto* narrowBtn = new FluentButton(u8"窄 (200px)", FluentButtonVariant::Secondary, page);
    auto* mediumBtn = new FluentButton(u8"中 (320px)", FluentButtonVariant::Secondary, page);
    auto* wideBtn = new FluentButton(u8"宽 (480px)", FluentButtonVariant::Secondary, page);

    connect(narrowBtn, &FluentButton::clicked, this, [mainWindow]() {
        auto* drawer = new FluentDrawer(mainWindow);
        drawer->setEdge(FluentDrawer::Right);
        drawer->setDrawerWidth(200);
        drawer->setTitle(u8"窄抽屉");
        auto* content = new QWidget();
        auto* cLayout = new QVBoxLayout(content);
        cLayout->setContentsMargins(16, 12, 16, 12);
        cLayout->addWidget(new QLabel(u8"紧凑内容区域"));
        cLayout->addStretch();
        drawer->setContent(content);
        drawer->resize(mainWindow->size());
        drawer->open();
        connect(drawer, &FluentDrawer::closed, drawer, &QObject::deleteLater);
    });

    connect(mediumBtn, &FluentButton::clicked, this, [mainWindow]() {
        auto* drawer = new FluentDrawer(mainWindow);
        drawer->setEdge(FluentDrawer::Right);
        drawer->setDrawerWidth(320);
        drawer->setTitle(u8"中等抽屉");
        auto* content = new QWidget();
        auto* cLayout = new QVBoxLayout(content);
        cLayout->setContentsMargins(20, 16, 20, 16);
        cLayout->addWidget(new QLabel(u8"标准内容区域"));
        cLayout->addStretch();
        drawer->setContent(content);
        drawer->resize(mainWindow->size());
        drawer->open();
        connect(drawer, &FluentDrawer::closed, drawer, &QObject::deleteLater);
    });

    connect(wideBtn, &FluentButton::clicked, this, [mainWindow]() {
        auto* drawer = new FluentDrawer(mainWindow);
        drawer->setEdge(FluentDrawer::Right);
        drawer->setDrawerWidth(480);
        drawer->setTitle(u8"宽抽屉");
        auto* content = new QWidget();
        auto* cLayout = new QVBoxLayout(content);
        cLayout->setContentsMargins(24, 16, 24, 16);
        cLayout->addWidget(new QLabel(u8"宽敞内容区域，适合表单等复杂内容"));
        cLayout->addStretch();
        drawer->setContent(content);
        drawer->resize(mainWindow->size());
        drawer->open();
        connect(drawer, &FluentDrawer::closed, drawer, &QObject::deleteLater);
    });

    sizeRow->addWidget(narrowBtn);
    sizeRow->addWidget(mediumBtn);
    sizeRow->addWidget(wideBtn);
    sizeRow->addStretch();
    layout->addLayout(sizeRow);

    // --- 遮罩控制 ---
    auto* maskLabel = new QLabel(u8"遮罩控制", page);
    maskLabel->setFont(theme->captionFont());
    maskLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [maskLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        maskLabel->setFont(t->captionFont());
        maskLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(maskLabel);

    auto* maskRow = new QHBoxLayout();
    maskRow->setSpacing(8);

    auto* withMaskBtn = new FluentButton(u8"有遮罩（点击关闭）", FluentButtonVariant::Secondary, page);
    auto* noMaskBtn = new FluentButton(u8"无遮罩", FluentButtonVariant::Secondary, page);
    auto* noCloseBtn = new FluentButton(u8"有遮罩（不可关闭）", FluentButtonVariant::Secondary, page);

    connect(withMaskBtn, &FluentButton::clicked, this, [mainWindow]() {
        auto* drawer = new FluentDrawer(mainWindow);
        drawer->setEdge(FluentDrawer::Right);
        drawer->setDrawerWidth(300);
        drawer->setTitle(u8"点击遮罩可关闭");
        drawer->setOverlay(true);
        drawer->setClosable(true);
        auto* content = new QWidget();
        auto* cLayout = new QVBoxLayout(content);
        cLayout->setContentsMargins(20, 16, 20, 16);
        cLayout->addWidget(new QLabel(u8"点击遮罩区域即可关闭此抽屉"));
        cLayout->addStretch();
        drawer->setContent(content);
        drawer->resize(mainWindow->size());
        drawer->open();
        connect(drawer, &FluentDrawer::closed, drawer, &QObject::deleteLater);
    });

    connect(noMaskBtn, &FluentButton::clicked, this, [mainWindow]() {
        auto* drawer = new FluentDrawer(mainWindow);
        drawer->setEdge(FluentDrawer::Right);
        drawer->setDrawerWidth(300);
        drawer->setTitle(u8"无遮罩");
        drawer->setOverlay(false);
        auto* content = new QWidget();
        auto* cLayout = new QVBoxLayout(content);
        cLayout->setContentsMargins(20, 16, 20, 16);
        cLayout->addWidget(new QLabel(u8"没有遮罩层，通过 ✕ 按钮关闭"));
        cLayout->addStretch();
        drawer->setContent(content);
        drawer->resize(mainWindow->size());
        drawer->open();
        connect(drawer, &FluentDrawer::closed, drawer, &QObject::deleteLater);
    });

    connect(noCloseBtn, &FluentButton::clicked, this, [mainWindow]() {
        auto* drawer = new FluentDrawer(mainWindow);
        drawer->setEdge(FluentDrawer::Right);
        drawer->setDrawerWidth(300);
        drawer->setTitle(u8"不可通过遮罩关闭");
        drawer->setOverlay(true);
        drawer->setClosable(false);
        auto* content = new QWidget();
        auto* cLayout = new QVBoxLayout(content);
        cLayout->setContentsMargins(20, 16, 20, 16);
        cLayout->addWidget(new QLabel(u8"点击遮罩不会关闭\n只能通过标题栏 ✕ 按钮或 Esc 键关闭"));
        cLayout->addStretch();
        drawer->setContent(content);
        drawer->resize(mainWindow->size());
        drawer->open();
        connect(drawer, &FluentDrawer::closed, drawer, &QObject::deleteLater);
    });

    maskRow->addWidget(withMaskBtn);
    maskRow->addWidget(noMaskBtn);
    maskRow->addWidget(noCloseBtn);
    maskRow->addStretch();
    layout->addLayout(maskRow);

    // --- 带丰富内容 ---
    auto* richLabel = new QLabel(u8"丰富内容示例", page);
    richLabel->setFont(theme->captionFont());
    richLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [richLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        richLabel->setFont(t->captionFont());
        richLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(richLabel);

    auto* richRow = new QHBoxLayout();
    richRow->setSpacing(8);

    auto* formBtn = new FluentButton(u8"表单抽屉", FluentButtonVariant::Primary, page);
    auto* listBtn = new FluentButton(u8"列表抽屉", FluentButtonVariant::Primary, page);

    connect(formBtn, &FluentButton::clicked, this, [mainWindow]() {
        auto* drawer = new FluentDrawer(mainWindow);
        drawer->setEdge(FluentDrawer::Right);
        drawer->setDrawerWidth(360);
        drawer->setTitle(u8"新建项目");
        auto* content = new QWidget();
        auto* cLayout = new QVBoxLayout(content);
        cLayout->setContentsMargins(20, 16, 20, 16);
        cLayout->setSpacing(12);
        cLayout->addWidget(new QLabel(u8"项目名称:"));
        auto* nameEdit = new FluentLineEdit(QString(), content);
        nameEdit->setPlaceholder(u8"请输入项目名称");
        cLayout->addWidget(nameEdit);
        cLayout->addWidget(new QLabel(u8"项目描述:"));
        auto* descEdit = new FluentLineEdit(QString(), content);
        descEdit->setPlaceholder(u8"请输入项目描述");
        cLayout->addWidget(descEdit);
        cLayout->addSpacing(16);
        auto* submitBtn = new FluentButton(u8"创建", FluentButtonVariant::Primary, content);
        connect(submitBtn, &FluentButton::clicked, drawer, &FluentDrawer::close);
        cLayout->addWidget(submitBtn);
        cLayout->addStretch();
        drawer->setContent(content);
        drawer->resize(mainWindow->size());
        drawer->open();
        connect(drawer, &FluentDrawer::closed, drawer, &QObject::deleteLater);
    });

    connect(listBtn, &FluentButton::clicked, this, [mainWindow]() {
        auto* drawer = new FluentDrawer(mainWindow);
        drawer->setEdge(FluentDrawer::Left);
        drawer->setDrawerWidth(260);
        drawer->setTitle(u8"文件列表");
        auto* content = new QWidget();
        auto* cLayout = new QVBoxLayout(content);
        cLayout->setContentsMargins(12, 8, 12, 8);
        cLayout->setSpacing(4);
        QStringList files = {u8"📄 main.cpp", u8"📄 widget.h", u8"📄 widget.cpp",
                            u8"📋 CMakeLists.txt", u8"📄 README.md", u8"🖼️ icon.png",
                            u8"📄 theme.h", u8"📄 theme.cpp"};
        for (const auto& f : files) {
            auto* item = new FluentButton(f, FluentButtonVariant::Subtle, content);
            item->setFixedHeight(36);
            cLayout->addWidget(item);
        }
        cLayout->addStretch();
        drawer->setContent(content);
        drawer->resize(mainWindow->size());
        drawer->open();
        connect(drawer, &FluentDrawer::closed, drawer, &QObject::deleteLater);
    });

    richRow->addWidget(formBtn);
    richRow->addWidget(listBtn);
    richRow->addStretch();
    layout->addLayout(richRow);

    layout->addStretch();

    return page;
}

QWidget* DemoMainWindow::createCardPage() {
    auto* page = new QWidget();
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* theme = FluentThemeManager::instance().currentTheme();
    auto* title = new QLabel(u8"Card 卡片", page);
    title->setFont(theme->subtitleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [title]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        title->setFont(t->subtitleFont());
        title->setStyleSheet(QString("color: %1;").arg(t->textColorPrimary().name()));
    });
    layout->addWidget(title);

    // --- 基础卡片 ---
    auto* basicLabel = new QLabel(u8"基础卡片", page);
    basicLabel->setFont(theme->captionFont());
    basicLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [basicLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        basicLabel->setFont(t->captionFont());
        basicLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(basicLabel);

    auto* basicRow = new QHBoxLayout();
    basicRow->setSpacing(16);

    // 基础卡片1
    auto* card1 = new FluentCard(page);
    card1->setTitle(u8"项目概览");
    card1->setSubtitle(u8"查看所有项目的运行状态与统计信息");
    card1->setFixedSize(260, 140);

    // 基础卡片2
    auto* card2 = new FluentCard(page);
    card2->setTitle(u8"系统通知");
    card2->setSubtitle(u8"3 条未读消息");
    card2->setFixedSize(260, 140);

    // 基础卡片3
    auto* card3 = new FluentCard(page);
    card3->setTitle(u8"快速操作");
    card3->setSubtitle(u8"创建新项目或导入现有资源");
    card3->setFixedSize(260, 140);

    basicRow->addWidget(card1);
    basicRow->addWidget(card2);
    basicRow->addWidget(card3);
    basicRow->addStretch();
    layout->addLayout(basicRow);

    // --- 可点击卡片 ---
    auto* clickLabel = new QLabel(u8"可点击卡片（悬停提升 + 强调线）", page);
    clickLabel->setFont(theme->captionFont());
    clickLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [clickLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        clickLabel->setFont(t->captionFont());
        clickLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(clickLabel);

    auto* clickRow = new QHBoxLayout();
    clickRow->setSpacing(16);

    auto* clickCard1 = new FluentCard(page);
    clickCard1->setTitle(u8"文档中心");
    clickCard1->setSubtitle(u8"浏览技术文档与教程");
    clickCard1->setClickable(true);
    clickCard1->setFixedSize(260, 140);
    connect(clickCard1, &FluentCard::clicked, this, []() {
        FluentInfoBar::info(nullptr, u8"卡片点击", u8"你点击了「文档中心」卡片");
    });

    auto* clickCard2 = new FluentCard(page);
    clickCard2->setTitle(u8"数据面板");
    clickCard2->setSubtitle(u8"查看实时数据分析");
    clickCard2->setClickable(true);
    clickCard2->setFixedSize(260, 140);
    connect(clickCard2, &FluentCard::clicked, this, []() {
        FluentInfoBar::info(nullptr, u8"卡片点击", u8"你点击了「数据面板」卡片");
    });

    auto* clickCard3 = new FluentCard(page);
    clickCard3->setTitle(u8"设置");
    clickCard3->setSubtitle(u8"自定义你的偏好");
    clickCard3->setClickable(true);
    clickCard3->setFixedSize(260, 140);
    connect(clickCard3, &FluentCard::clicked, this, []() {
        FluentInfoBar::info(nullptr, u8"卡片点击", u8"你点击了「设置」卡片");
    });

    clickRow->addWidget(clickCard1);
    clickRow->addWidget(clickCard2);
    clickRow->addWidget(clickCard3);
    clickRow->addStretch();
    layout->addLayout(clickRow);

    // --- 带封面图片 ---
    auto* coverLabel = new QLabel(u8"带封面图片", page);
    coverLabel->setFont(theme->captionFont());
    coverLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [coverLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        coverLabel->setFont(t->captionFont());
        coverLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(coverLabel);

    auto* coverRow = new QHBoxLayout();
    coverRow->setSpacing(16);

    // 生成纯色封面图
    auto makeColorCover = [](const QColor& color, const QString& text) -> QPixmap {
        QPixmap pix(400, 200);
        pix.fill(color);
        QPainter p(&pix);
        p.setPen(Qt::white);
        p.setFont(QFont("Segoe UI", 20, QFont::Bold));
        p.drawText(pix.rect(), Qt::AlignCenter, text);
        return pix;
    };

    auto* coverCard1 = new FluentCard(page);
    coverCard1->setCoverPixmap(makeColorCover(QColor("#0078D4"), u8"🌊 海洋"));
    coverCard1->setCoverHeight(120);
    coverCard1->setTitle(u8"海洋主题");
    coverCard1->setSubtitle(u8"蓝色调配色方案");
    coverCard1->setFixedSize(260, 260);

    auto* coverCard2 = new FluentCard(page);
    coverCard2->setCoverPixmap(makeColorCover(QColor("#2ECC71"), u8"🌿 自然"));
    coverCard2->setCoverHeight(120);
    coverCard2->setTitle(u8"自然主题");
    coverCard2->setSubtitle(u8"绿色调配色方案");
    coverCard2->setFixedSize(260, 260);

    auto* coverCard3 = new FluentCard(page);
    coverCard3->setCoverPixmap(makeColorCover(QColor("#E74C3C"), u8"🔥 热情"));
    coverCard3->setCoverHeight(120);
    coverCard3->setTitle(u8"热情主题");
    coverCard3->setSubtitle(u8"红色调配色方案");
    coverCard3->setFixedSize(260, 260);

    coverRow->addWidget(coverCard1);
    coverRow->addWidget(coverCard2);
    coverRow->addWidget(coverCard3);
    coverRow->addStretch();
    layout->addLayout(coverRow);

    // --- 带自定义内容和底部操作栏 ---
    auto* customLabel = new QLabel(u8"带自定义内容 + 底部操作栏", page);
    customLabel->setFont(theme->captionFont());
    customLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [customLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        customLabel->setFont(t->captionFont());
        customLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(customLabel);

    auto* customRow = new QHBoxLayout();
    customRow->setSpacing(16);

    // 统计卡片
    auto* statCard = new FluentCard(page);
    statCard->setTitle(u8"用户统计");
    auto* statContent = new QWidget();
    auto* statLayout = new QVBoxLayout(statContent);
    statLayout->setContentsMargins(0, 4, 0, 4);
    statLayout->setSpacing(4);
    auto* stat1 = new QLabel(u8"👤 活跃用户: 1,234");
    auto* stat2 = new QLabel(u8"📊 日增长: +5.6%");
    auto* stat3 = new QLabel(u8"🌍 地区: 28 个");
    statLayout->addWidget(stat1);
    statLayout->addWidget(stat2);
    statLayout->addWidget(stat3);
    statCard->setContentWidget(statContent);
    auto* statFooter = new QWidget();
    auto* statFooterLayout = new QHBoxLayout(statFooter);
    statFooterLayout->setContentsMargins(0, 0, 0, 0);
    statFooterLayout->addStretch();
    auto* detailBtn = new FluentButton(u8"查看详情", FluentButtonVariant::Primary, statFooter);
    statFooterLayout->addWidget(detailBtn);
    statCard->setFooterWidget(statFooter);
    statCard->setFixedSize(280, 220);

    // 任务卡片
    auto* taskCard = new FluentCard(page);
    taskCard->setTitle(u8"待办任务");
    auto* taskContent = new QWidget();
    auto* taskLayout = new QVBoxLayout(taskContent);
    taskLayout->setContentsMargins(0, 4, 0, 4);
    taskLayout->setSpacing(4);
    auto* t1 = new QLabel(u8"☑ 完成首页设计");
    auto* t2 = new QLabel(u8"☑ 代码审查");
    auto* t3 = new QLabel(u8"☐ 发布 v2.0");
    taskLayout->addWidget(t1);
    taskLayout->addWidget(t2);
    taskLayout->addWidget(t3);
    taskCard->setContentWidget(taskContent);
    auto* taskFooter = new QWidget();
    auto* taskFooterLayout = new QHBoxLayout(taskFooter);
    taskFooterLayout->setContentsMargins(0, 0, 0, 0);
    taskFooterLayout->addStretch();
    auto* addBtn = new FluentButton(u8"添加任务", FluentButtonVariant::Secondary, taskFooter);
    taskFooterLayout->addWidget(addBtn);
    taskCard->setFooterWidget(taskFooter);
    taskCard->setFixedSize(280, 220);

    customRow->addWidget(statCard);
    customRow->addWidget(taskCard);
    customRow->addStretch();
    layout->addLayout(customRow);

    // --- 无边框 + 自定义圆角 ---
    auto* styleLabel = new QLabel(u8"样式变体", page);
    styleLabel->setFont(theme->captionFont());
    styleLabel->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [styleLabel]() {
        auto* t = FluentThemeManager::instance().currentTheme();
        styleLabel->setFont(t->captionFont());
        styleLabel->setStyleSheet(QString("color: %1;").arg(t->textColorSecondary().name()));
    });
    layout->addWidget(styleLabel);

    auto* styleRow = new QHBoxLayout();
    styleRow->setSpacing(16);

    auto* noBorderCard = new FluentCard(page);
    noBorderCard->setTitle(u8"无边框");
    noBorderCard->setSubtitle(u8"setShowBorder(false)");
    noBorderCard->setShowBorder(false);
    noBorderCard->setFixedSize(200, 120);

    auto* roundCard = new FluentCard(page);
    roundCard->setTitle(u8"大圆角");
    roundCard->setSubtitle(u8"borderRadius = 16");
    roundCard->setBorderRadius(16);
    roundCard->setFixedSize(200, 120);

    auto* noElevCard = new FluentCard(page);
    noElevCard->setTitle(u8"无悬停提升");
    noElevCard->setSubtitle(u8"hoverElevation = false");
    noElevCard->setHoverElevation(false);
    noElevCard->setFixedSize(200, 120);

    styleRow->addWidget(noBorderCard);
    styleRow->addWidget(roundCard);
    styleRow->addWidget(noElevCard);
    styleRow->addStretch();
    layout->addLayout(styleRow);

    layout->addStretch();

    return page;
}

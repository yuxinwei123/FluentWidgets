#include "MainWindow.h"
#include "Core/FluentThemeManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>

DemoMainWindow::DemoMainWindow(QWidget* parent)
    : FluentWindow(parent)
{
    setWindowTitle(u8"FluentWidgets Demo");
    resize(1000, 650);

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
    m_navView->addPage("buttons", createButtonPage());
    m_navView->addPage("inputs", createInputPage());
    m_navView->addPage("dialogs", createDialogPage());

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
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto* title = new QLabel(u8"欢迎使用 FluentWidgets", page);
    auto* theme = FluentThemeManager::instance().currentTheme();
    title->setFont(theme->titleFont());
    title->setStyleSheet(QString("color: %1;").arg(theme->textColorPrimary().name()));

    auto* desc = new QLabel(u8"这是一个基于 Qt 的 Fluent Design 风格组件库\n左侧导航栏可展开多级菜单并切换页面", page);
    desc->setFont(theme->bodyFont());
    desc->setStyleSheet(QString("color: %1;").arg(theme->textColorSecondary().name()));

    // 主题切换
    auto* btnRow = new QHBoxLayout();
    btnRow->setSpacing(12);

    auto* themeBtn = new FluentButton(u8"切换主题", FluentButtonVariant::Accent, page);
    connect(themeBtn, &FluentButton::clicked, this, &DemoMainWindow::toggleTheme);

    m_themeLabel = new QLabel(page);
    m_themeLabel->setFont(theme->captionFont());
    auto updateLabel = [this]() {
        bool isDark = FluentThemeManager::instance().currentTheme()->isDark();
        m_themeLabel->setText(isDark ? u8"当前主题: Dark" : u8"当前主题: Light");
        m_themeLabel->setStyleSheet(
            QString("color: %1;").arg(FluentThemeManager::instance().currentTheme()->textColorPrimary().name()));
    };
    updateLabel();
    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, updateLabel);

    btnRow->addWidget(themeBtn);
    btnRow->addWidget(m_themeLabel);
    btnRow->addStretch();

    layout->addWidget(title);
    layout->addWidget(desc);
    layout->addLayout(btnRow);
    layout->addStretch();

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

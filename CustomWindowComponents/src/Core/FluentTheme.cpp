#include "FluentTheme.h"
#include "FluentConfig.h"
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QVariantAnimation>

// === QColor 自定义插值函数 ===
static QVariant colorInterpolator(const QColor& from, const QColor& to, qreal progress) {
    int r = from.red()   + (to.red()   - from.red())   * progress;
    int g = from.green() + (to.green() - from.green()) * progress;
    int b = from.blue()  + (to.blue()  - from.blue())  * progress;
    int a = from.alpha() + (to.alpha() - from.alpha()) * progress;
    return QColor(r, g, b, a);
}

void FluentTheme::registerColorInterpolator() {
    static bool registered = false;
    if (!registered) {
        qRegisterAnimationInterpolator<QColor>(colorInterpolator);
        registered = true;
    }
}

FluentTheme::FluentTheme(FluentThemeMode mode, QObject* parent)
    : QObject(parent)
    , m_mode(mode)
    , m_primaryColor("#0078D4")
{
    if (mode == FluentThemeMode::Light) {
        applyLightPalette();
    } else {
        applyDarkPalette();
    }
    m_oldColors = m_colors;
    m_oldPrimaryColor = m_primaryColor;
}

// --- 颜色 getter + setter ---
// getter 根据 m_paintOldPalette 返回新色或旧色

QColor FluentTheme::primaryColor() const {
    return m_paintOldPalette ? m_oldPrimaryColor : m_primaryColor;
}
void FluentTheme::setPrimaryColor(const QColor& color) {
    if (m_primaryColor != color) { m_primaryColor = color; emit themeChanged(); }
}

QColor FluentTheme::backgroundColor() const {
    return m_paintOldPalette ? m_oldColors.backgroundColor : m_colors.backgroundColor;
}
void FluentTheme::setBackgroundColor(const QColor& color) {
    if (m_colors.backgroundColor != color) { m_colors.backgroundColor = color; emit themeChanged(); }
}

QColor FluentTheme::surfaceColor() const {
    return m_paintOldPalette ? m_oldColors.surfaceColor : m_colors.surfaceColor;
}
void FluentTheme::setSurfaceColor(const QColor& color) {
    if (m_colors.surfaceColor != color) { m_colors.surfaceColor = color; emit themeChanged(); }
}

QColor FluentTheme::cardColor() const {
    return m_paintOldPalette ? m_oldColors.cardColor : m_colors.cardColor;
}
void FluentTheme::setCardColor(const QColor& color) {
    if (m_colors.cardColor != color) { m_colors.cardColor = color; emit themeCardColorChanged(); }
}

QColor FluentTheme::textColorPrimary() const {
    return m_paintOldPalette ? m_oldColors.textColorPrimary : m_colors.textColorPrimary;
}
void FluentTheme::setTextColorPrimary(const QColor& color) {
    if (m_colors.textColorPrimary != color) { m_colors.textColorPrimary = color; emit themeChanged(); }
}

QColor FluentTheme::textColorSecondary() const {
    return m_paintOldPalette ? m_oldColors.textColorSecondary : m_colors.textColorSecondary;
}
void FluentTheme::setTextColorSecondary(const QColor& color) {
    if (m_colors.textColorSecondary != color) { m_colors.textColorSecondary = color; emit themeChanged(); }
}

QColor FluentTheme::textColorDisabled() const {
    return m_paintOldPalette ? m_oldColors.textColorDisabled : m_colors.textColorDisabled;
}
void FluentTheme::setTextColorDisabled(const QColor& color) {
    if (m_colors.textColorDisabled != color) { m_colors.textColorDisabled = color; emit themeChanged(); }
}

QColor FluentTheme::borderColor() const {
    return m_paintOldPalette ? m_oldColors.borderColor : m_colors.borderColor;
}
void FluentTheme::setBorderColor(const QColor& color) {
    if (m_colors.borderColor != color) { m_colors.borderColor = color; emit themeChanged(); }
}

QColor FluentTheme::dividerColor() const {
    return m_paintOldPalette ? m_oldColors.dividerColor : m_colors.dividerColor;
}
void FluentTheme::setDividerColor(const QColor& color) {
    if (m_colors.dividerColor != color) { m_colors.dividerColor = color; emit themeChanged(); }
}

QColor FluentTheme::errorColor() const {
    return m_paintOldPalette ? m_oldColors.errorColor : m_colors.errorColor;
}
void FluentTheme::setErrorColor(const QColor& color) {
    if (m_colors.errorColor != color) { m_colors.errorColor = color; emit themeChanged(); }
}

QColor FluentTheme::successColor() const {
    return m_paintOldPalette ? m_oldColors.successColor : m_colors.successColor;
}
void FluentTheme::setSuccessColor(const QColor& color) {
    if (m_colors.successColor != color) { m_colors.successColor = color; emit themeChanged(); }
}

QColor FluentTheme::warningColor() const {
    return m_paintOldPalette ? m_oldColors.warningColor : m_colors.warningColor;
}
void FluentTheme::setWarningColor(const QColor& color) {
    if (m_colors.warningColor != color) { m_colors.warningColor = color; emit themeChanged(); }
}

QColor FluentTheme::infoColor() const {
    return m_paintOldPalette ? m_oldColors.infoColor : m_colors.infoColor;
}
void FluentTheme::setInfoColor(const QColor& color) {
    if (m_colors.infoColor != color) { m_colors.infoColor = color; emit themeChanged(); }
}

// --- 排版 ---

QFont FluentTheme::captionFont() const { QFont f; f.setFamily("Segoe UI"); f.setPixelSize(12); return f; }
QFont FluentTheme::bodyFont() const { QFont f; f.setFamily("Segoe UI"); f.setPixelSize(14); return f; }
QFont FluentTheme::bodyStrongFont() const { QFont f; f.setFamily("Segoe UI"); f.setPixelSize(14); f.setBold(true); return f; }
QFont FluentTheme::subtitleFont() const { QFont f; f.setFamily("Segoe UI"); f.setPixelSize(20); return f; }
QFont FluentTheme::titleFont() const { QFont f; f.setFamily("Segoe UI"); f.setPixelSize(28); return f; }
QFont FluentTheme::titleLargeFont() const { QFont f; f.setFamily("Segoe UI"); f.setPixelSize(40); return f; }
QFont FluentTheme::displayFont() const { QFont f; f.setFamily("Segoe UI"); f.setPixelSize(68); return f; }

// --- 间距 ---

int FluentTheme::controlHeight() const { return 32; }
int FluentTheme::controlHeightSmall() const { return 24; }
int FluentTheme::controlHeightLarge() const { return 40; }
int FluentTheme::cornerRadius() const { return 4; }
int FluentTheme::cornerRadiusSmall() const { return 2; }
int FluentTheme::borderWidth() const { return 1; }
int FluentTheme::focusBorderWidth() const { return 2; }

// --- 模式 ---

FluentThemeMode FluentTheme::themeMode() const { return m_mode; }

void FluentTheme::setThemeMode(FluentThemeMode mode) {
    if (m_mode == mode) return;

    ThemeColors target = targetPalette(mode);
    m_mode = mode;

    if (FluentConfig::instance().animationEnabled() && m_animationDuration > 0) {
        startRevealAnimation(target);
    } else {
        applyPaletteImmediate(target);
        emit themeChanged();
    }
}

int FluentTheme::animationDuration() const { return m_animationDuration; }
void FluentTheme::setAnimationDuration(int ms) { m_animationDuration = ms; }

bool FluentTheme::isDark() const { return m_mode == FluentThemeMode::Dark; }

// --- 填充式动画 ---

qreal FluentTheme::revealProgress() const { return m_revealProgress; }

void FluentTheme::setRevealProgress(qreal progress) {
    if (qFuzzyCompare(m_revealProgress, progress)) return;
    m_revealProgress = progress;
    emit revealProgressChanged(progress);
    emit themeChanged();  // 触发所有组件 update()
}

bool FluentTheme::isRevealing() const { return m_revealing; }

void FluentTheme::setPaintOldPalette(bool old) { m_paintOldPalette = old; }
bool FluentTheme::paintOldPalette() const { return m_paintOldPalette; }

void FluentTheme::startRevealAnimation(const ThemeColors& target) {
    // 保存旧调色板
    m_oldColors = m_colors;
    m_oldPrimaryColor = m_primaryColor;

    // 立即应用新颜色到 m_colors（绘制时由裁剪路径控制可见性）
    m_primaryColor = target.primaryColor;
    m_colors = target;

    m_revealing = true;
    m_revealProgress = 0.0;

    auto* anim = new QPropertyAnimation(this, "revealProgress");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(m_animationDuration);
    anim->setEasingCurve(QEasingCurve::OutCubic);

    connect(anim, &QAbstractAnimation::finished, this, [this]() {
        m_revealing = false;
        m_revealProgress = 1.0;
        m_oldColors = m_colors;
        m_oldPrimaryColor = m_primaryColor;
        emit revealProgressChanged(1.0);
    });
    connect(anim, &QAbstractAnimation::finished, anim, &QObject::deleteLater);

    anim->start();
}

void FluentTheme::applyPaletteImmediate(const ThemeColors& palette) {
    m_primaryColor = palette.primaryColor;
    m_colors = palette;
    m_oldColors = m_colors;
    m_oldPrimaryColor = m_primaryColor;
    m_revealProgress = 1.0;
    m_revealing = false;
}

// --- 私有 ---

ThemeColors FluentTheme::targetPalette(FluentThemeMode mode) const {
    if (mode == FluentThemeMode::Light) {
        return {
            "#0078D4",  // primaryColor
            "#FFFFFF",  // backgroundColor
            "#F3F3F3",  // surfaceColor
            "#FFFFFF",  // cardColor
            "#1A1A1A",  // textColorPrimary
            "#616161",  // textColorSecondary
            "#A0A0A0",  // textColorDisabled
            "#E0E0E0",  // borderColor
            "#EDEDED",  // dividerColor
            "#D13438",  // errorColor
            "#0F7B0F",  // successColor
            "#9D5D00",  // warningColor
            "#0078D4"   // infoColor
        };
    } else {
        return {
            "#0078D4",  // primaryColor
            "#202020",  // backgroundColor
            "#2D2D2D",  // surfaceColor
            "#2D2D2D",  // cardColor
            "#FFFFFF",  // textColorPrimary
            "#C5C5C5",  // textColorSecondary
            "#5C5C5C",  // textColorDisabled
            "#3D3D3D",  // borderColor
            "#3D3D3D",  // dividerColor
            "#F47067",  // errorColor
            "#6CCB5F",  // successColor
            "#FFC930",  // warningColor
            "#4CC2FF"   // infoColor
        };
    }
}

void FluentTheme::applyLightPalette() {
    m_colors = targetPalette(FluentThemeMode::Light);
}

void FluentTheme::applyDarkPalette() {
    m_colors = targetPalette(FluentThemeMode::Dark);
}

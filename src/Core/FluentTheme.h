#ifndef FLUENTTHEME_H
#define FLUENTTHEME_H

#include "FluentGlobal.h"
#include <QObject>
#include <QColor>
#include <QFont>

// 所有颜色 Token 的集合，用于主题切换动画的目标调色板
struct ThemeColors {
    QColor primaryColor;
    QColor backgroundColor;
    QColor surfaceColor;
    QColor cardColor;
    QColor textColorPrimary;
    QColor textColorSecondary;
    QColor textColorDisabled;
    QColor borderColor;
    QColor dividerColor;
    QColor errorColor;
    QColor successColor;
    QColor warningColor;
    QColor infoColor;
};

class FLUENT_EXPORT FluentTheme : public QObject {
    Q_OBJECT

    // === 颜色 Token (全部带 WRITE，支持动画驱动) ===
    Q_PROPERTY(QColor primaryColor READ primaryColor WRITE setPrimaryColor NOTIFY themeChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY themeChanged)
    Q_PROPERTY(QColor surfaceColor READ surfaceColor WRITE setSurfaceColor NOTIFY themeChanged)
    Q_PROPERTY(QColor cardColor READ cardColor WRITE setCardColor NOTIFY themeCardColorChanged)
    Q_PROPERTY(QColor textColorPrimary READ textColorPrimary WRITE setTextColorPrimary NOTIFY themeChanged)
    Q_PROPERTY(QColor textColorSecondary READ textColorSecondary WRITE setTextColorSecondary NOTIFY themeChanged)
    Q_PROPERTY(QColor textColorDisabled READ textColorDisabled WRITE setTextColorDisabled NOTIFY themeChanged)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY themeChanged)
    Q_PROPERTY(QColor dividerColor READ dividerColor WRITE setDividerColor NOTIFY themeChanged)
    Q_PROPERTY(QColor errorColor READ errorColor WRITE setErrorColor NOTIFY themeChanged)
    Q_PROPERTY(QColor successColor READ successColor WRITE setSuccessColor NOTIFY themeChanged)
    Q_PROPERTY(QColor warningColor READ warningColor WRITE setWarningColor NOTIFY themeChanged)
    Q_PROPERTY(QColor infoColor READ infoColor WRITE setInfoColor NOTIFY themeChanged)

    // === 排版 Token ===
    Q_PROPERTY(QFont captionFont READ captionFont NOTIFY themeChanged)
    Q_PROPERTY(QFont bodyFont READ bodyFont NOTIFY themeChanged)
    Q_PROPERTY(QFont bodyStrongFont READ bodyStrongFont NOTIFY themeChanged)
    Q_PROPERTY(QFont subtitleFont READ subtitleFont NOTIFY themeChanged)
    Q_PROPERTY(QFont titleFont READ titleFont NOTIFY themeChanged)
    Q_PROPERTY(QFont titleLargeFont READ titleLargeFont NOTIFY themeChanged)
    Q_PROPERTY(QFont displayFont READ displayFont NOTIFY themeChanged)

    // === 间距 Token ===
    Q_PROPERTY(int controlHeight READ controlHeight NOTIFY themeChanged)
    Q_PROPERTY(int controlHeightSmall READ controlHeightSmall NOTIFY themeChanged)
    Q_PROPERTY(int controlHeightLarge READ controlHeightLarge NOTIFY themeChanged)
    Q_PROPERTY(int cornerRadius READ cornerRadius NOTIFY themeChanged)
    Q_PROPERTY(int cornerRadiusSmall READ cornerRadiusSmall NOTIFY themeChanged)
    Q_PROPERTY(int borderWidth READ borderWidth NOTIFY themeChanged)
    Q_PROPERTY(int focusBorderWidth READ focusBorderWidth NOTIFY themeChanged)

    // === 主题模式 ===
    Q_PROPERTY(FluentThemeMode themeMode READ themeMode WRITE setThemeMode NOTIFY themeChanged)

    // === 填充式动画进度 (0.0 ~ 1.0) ===
    Q_PROPERTY(qreal revealProgress READ revealProgress WRITE setRevealProgress NOTIFY revealProgressChanged)

public:
    explicit FluentTheme(FluentThemeMode mode = FluentThemeMode::Light, QObject* parent = nullptr);

    // 颜色 getter + setter
    // 动画期间 getter 会根据 m_paintOldPalette 标志返回新色或旧色
    QColor primaryColor() const;
    void setPrimaryColor(const QColor& color);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor& color);

    QColor surfaceColor() const;
    void setSurfaceColor(const QColor& color);

    QColor cardColor() const;
    void setCardColor(const QColor& color);

    QColor textColorPrimary() const;
    void setTextColorPrimary(const QColor& color);

    QColor textColorSecondary() const;
    void setTextColorSecondary(const QColor& color);

    QColor textColorDisabled() const;
    void setTextColorDisabled(const QColor& color);

    QColor borderColor() const;
    void setBorderColor(const QColor& color);

    QColor dividerColor() const;
    void setDividerColor(const QColor& color);

    QColor errorColor() const;
    void setErrorColor(const QColor& color);

    QColor successColor() const;
    void setSuccessColor(const QColor& color);

    QColor warningColor() const;
    void setWarningColor(const QColor& color);

    QColor infoColor() const;
    void setInfoColor(const QColor& color);

    // 排版
    QFont captionFont() const;
    QFont bodyFont() const;
    QFont bodyStrongFont() const;
    QFont subtitleFont() const;
    QFont titleFont() const;
    QFont titleLargeFont() const;
    QFont displayFont() const;

    // 间距
    int controlHeight() const;
    int controlHeightSmall() const;
    int controlHeightLarge() const;
    int cornerRadius() const;
    int cornerRadiusSmall() const;
    int borderWidth() const;
    int focusBorderWidth() const;

    // 模式
    FluentThemeMode themeMode() const;
    void setThemeMode(FluentThemeMode mode);

    // 动画过渡时长 (ms)，0 表示无动画瞬间切换
    int animationDuration() const;
    void setAnimationDuration(int ms);

    bool isDark() const;

    // 填充式动画：当前扩散进度
    qreal revealProgress() const;
    void setRevealProgress(qreal progress);

    // 填充式动画：是否正在过渡中
    bool isRevealing() const;

    // 临时切换调色板：绘制旧色/新色
    // 用于 paintEvent 中分两遍绘制（旧色全画面 → 新色圆内）
    void setPaintOldPalette(bool old);
    bool paintOldPalette() const;

    // 注册 QColor 插值函数 (在应用启动时调用一次)
    static void registerColorInterpolator();

signals:
    void themeChanged();
    void revealProgressChanged(qreal progress);
    void themeCardColorChanged();

private:
    void applyLightPalette();
    void applyDarkPalette();
    ThemeColors targetPalette(FluentThemeMode mode) const;
    void applyPaletteImmediate(const ThemeColors& palette);
    void startRevealAnimation(const ThemeColors& target);

    FluentThemeMode m_mode;
    QColor m_primaryColor;
    ThemeColors m_colors;           // 当前（新）调色板
    QColor m_oldPrimaryColor;       // 旧 primary
    ThemeColors m_oldColors;        // 动画开始前的旧调色板
    qreal m_revealProgress = 1.0;   // 0=未开始, 1=完成
    bool m_revealing = false;
    bool m_paintOldPalette = false; // 绘制时临时切换到旧调色板
    int m_animationDuration = 500;  // 默认 500ms
};

#endif // FLUENTTHEME_H

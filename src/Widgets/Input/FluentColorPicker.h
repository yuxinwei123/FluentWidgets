#ifndef FLUENTCOLORPICKER_H
#define FLUENTCOLORPICKER_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QColor>
#include <QTimer>
#include <QRandomGenerator>
// Fluent UI 风格颜色选择器
// 支持: 预设色板、自定义颜色、透明度、HEX 输入、主题适配

class FLUENT_EXPORT FluentColorPicker : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(bool showAlpha READ showAlpha WRITE setShowAlpha NOTIFY showAlphaChanged)
    Q_PROPERTY(QString placeholder READ placeholder WRITE setPlaceholder NOTIFY placeholderChanged)

public:
    explicit FluentColorPicker(QWidget* parent = nullptr);
    ~FluentColorPicker() override;

    // 颜色
    QColor color() const;
    void setColor(const QColor& color);
    void clear();

    // 透明度
    bool showAlpha() const;
    void setShowAlpha(bool on);

    // 占位文本
    QString placeholder() const;
    void setPlaceholder(const QString& text);

    // 预设颜色
    void setPresetColors(const QVector<QColor>& colors);
    QVector<QColor> presetColors() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void colorChanged(const QColor& color);
    void showAlphaChanged();
    void placeholderChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void showPopup();
    void hidePopup();
    void updatePopupTheme();
    QString colorText() const;

    QColor m_color;
    bool m_hasColor = false;
    bool m_showAlpha = false;
    QString m_placeholder;
    QVector<QColor> m_presetColors;

    // 弹出面板
    QWidget* m_popup = nullptr;
    class ColorGrid* m_colorGrid = nullptr;
    class AlphaSlider* m_alphaSlider = nullptr;
    class HexInput* m_hexInput = nullptr;
    bool m_popupVisible = false;

    // 布局
    QRect previewRect() const;
    QRect textRect() const;

    enum { HEIGHT = 32, PREVIEW_SIZE = 20, LEFT_PADDING = 6, RIGHT_PADDING = 10, PREVIEW_MARGIN = 6 };
};

// 内部：色块网格
class ColorGrid : public QWidget {
    Q_OBJECT
public:
    explicit ColorGrid(QWidget* parent = nullptr);

    void setColors(const QVector<QColor>& colors);
    void setSelectedColor(const QColor& color);

    QSize sizeHint() const override;

signals:
    void colorClicked(const QColor& color);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QVector<QColor> m_colors;
    QColor m_selectedColor;
    int m_hoverIndex = -1;
    int m_columns = 10;
    int m_cellSize = 24;
    int m_spacing = 4;
};

// 内部：透明度滑块
class AlphaSlider : public QWidget {
    Q_OBJECT
public:
    explicit AlphaSlider(QWidget* parent = nullptr);

    void setColor(const QColor& color);
    int alpha() const;
    void setAlpha(int alpha);

    QSize sizeHint() const override;

signals:
    void alphaChanged(int alpha);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QColor m_color;
    int m_alpha = 255;
    bool m_dragging = false;
};

// 内部：HEX 输入框
class HexInput : public QWidget {
    Q_OBJECT
public:
    explicit HexInput(QWidget* parent = nullptr);

    void setColor(const QColor& color);
    QColor color() const;

    QSize sizeHint() const override;

signals:
    void colorSubmitted(const QColor& color);

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    QString m_text;
    bool m_hasFocus = false;
};

#endif // FLUENTCOLORPICKER_H

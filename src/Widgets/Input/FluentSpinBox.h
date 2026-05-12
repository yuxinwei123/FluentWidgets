#ifndef FLUENTSPINBOX_H
#define FLUENTSPINBOX_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"

// Fluent UI 风格数值调节控件
// 支持: 整数/浮点数、上下按钮、键盘编辑、步进值、前后缀、主题适配

class FLUENT_EXPORT FluentSpinBox : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(int value READ intValue WRITE setIntValue NOTIFY valueChanged)
    Q_PROPERTY(double doubleValue READ doubleValue WRITE setDoubleValue NOTIFY valueChanged)
    Q_PROPERTY(double minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(double singleStep READ singleStep WRITE setSingleStep NOTIFY singleStepChanged)
    Q_PROPERTY(int decimals READ decimals WRITE setDecimals NOTIFY decimalsChanged)
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix NOTIFY prefixChanged)
    Q_PROPERTY(QString suffix READ suffix WRITE setSuffix NOTIFY suffixChanged)
    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly NOTIFY readOnlyChanged)

public:
    explicit FluentSpinBox(QWidget* parent = nullptr);
    ~FluentSpinBox() override;

    // 值
    int intValue() const;
    void setIntValue(int val);
    double doubleValue() const;
    void setDoubleValue(double val);

    // 范围
    double minimum() const;
    void setMinimum(double min);
    double maximum() const;
    void setMaximum(double max);
    void setRange(double min, double max);

    // 步进
    double singleStep() const;
    void setSingleStep(double step);

    // 小数位数
    int decimals() const;
    void setDecimals(int n);

    // 前后缀
    QString prefix() const;
    void setPrefix(const QString& p);
    QString suffix() const;
    void setSuffix(const QString& s);

    // 只读
    bool readOnly() const;
    void setReadOnly(bool ro);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void valueChanged(double val);
    void valueChanged(int val);
    void minimumChanged();
    void maximumChanged();
    void singleStepChanged();
    void decimalsChanged();
    void prefixChanged();
    void suffixChanged();
    void readOnlyChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void timerEvent(QTimerEvent* event) override;
    void inputMethodEvent(QInputMethodEvent* event) override;

private:
    // 布局
    QRect upButtonRect() const;
    QRect downButtonRect() const;
    QRect textRect() const;
    int buttonWidth() const;

    // 操作
    void stepUp();
    void stepDown();
    void stepBy(int steps);
    void commitText();
    QString formatValue(double val) const;
    double boundValue(double val) const;

    // 数据
    double m_value = 0.0;
    double m_minimum = 0.0;
    double m_maximum = 99.99;
    double m_singleStep = 1.0;
    int m_decimals = 0;
    QString m_prefix;
    QString m_suffix;
    bool m_readOnly = false;

    // 编辑状态
    QString m_editText;
    int m_cursorPos = 0;
    int m_scrollOffset = 0;
    int m_selectionStart = 0;
    int m_selectionEnd = 0;
    bool m_selecting = false;
    bool m_cursorBlink = true;
    int m_blinkTimerId = 0;

    // 交互状态
    enum class HoverArea { None, Up, Down };
    HoverArea m_hoveredArea = HoverArea::None;
    HoverArea m_pressedArea = HoverArea::None;
    int m_repeatTimerId = 0;
    bool m_repeatStep = false;  // true=up, false=down

    // 常量
    enum { HEIGHT = 32, BTN_WIDTH = 32, BORDER_RADIUS = 4,
           LEFT_PADDING = 10, RIGHT_PADDING = 4 };
};

#endif // FLUENTSPINBOX_H

#ifndef FLUENTLINEEDIT_H
#define FLUENTLINEEDIT_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"

class FLUENT_EXPORT FluentLineEdit : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString placeholder READ placeholder WRITE setPlaceholder NOTIFY placeholderChanged)
    Q_PROPERTY(FluentSize size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(bool passwordMode READ passwordMode WRITE setPasswordMode NOTIFY passwordModeChanged)
    Q_PROPERTY(bool showError READ showError WRITE setShowError NOTIFY showErrorChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage WRITE setErrorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(bool clearButtonEnabled READ clearButtonEnabled WRITE setClearButtonEnabled NOTIFY clearButtonEnabledChanged)

public:
    explicit FluentLineEdit(const QString& text = QString(), QWidget* parent = nullptr);

    QString text() const;
    void setText(const QString& text);

    QString placeholder() const;
    void setPlaceholder(const QString& placeholder);

    FluentSize size() const;
    void setSize(FluentSize size);

    bool passwordMode() const;
    void setPasswordMode(bool enabled);

    bool showError() const;
    void setShowError(bool show);

    QString errorMessage() const;
    void setErrorMessage(const QString& msg);

    bool clearButtonEnabled() const;
    void setClearButtonEnabled(bool enabled);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void textChanged(const QString& text);
    void placeholderChanged();
    void sizeChanged();
    void passwordModeChanged();
    void showErrorChanged();
    void errorMessageChanged();
    void clearButtonEnabledChanged();
    void returnPressed();
    void editingFinished();

protected:
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void inputMethodEvent(QInputMethodEvent* event) override;
    void timerEvent(QTimerEvent* event) override;

private:
    void setupSize();
    void ensureCursorVisible();
    int textOffset() const;
    int textWidth() const;
    QRect textRect() const;
    QRect clearButtonRect() const;
    bool isOverClearButton(const QPoint& pos) const;

    QString m_text;
    QString m_placeholder;
    FluentSize m_size = FluentSize::Medium;
    bool m_passwordMode = false;
    bool m_showError = false;
    QString m_errorMessage;
    bool m_clearButtonEnabled = true;

    // 光标
    int m_cursorPos = 0;
    int m_scrollOffset = 0;
    bool m_cursorBlink = true;
    int m_blinkTimerId = 0;

    // 选中
    int m_selectionStart = 0;
    int m_selectionEnd = 0;
    bool m_selecting = false;

    // 内部边距
    static const int LEFT_PADDING = 10;
    static const int RIGHT_PADDING = 10;
    static const int CLEAR_BTN_SIZE = 16;
};

#endif // FLUENTLINEEDIT_H

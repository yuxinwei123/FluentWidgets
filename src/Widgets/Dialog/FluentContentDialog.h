#ifndef FLUENTCONTENTDIALOG_H
#define FLUENTCONTENTDIALOG_H

#include "Core/FluentGlobal.h"
#include <QWidget>
#include <QDialog>

class FluentTheme;
class FluentButton;
class QVBoxLayout;
class QLabel;
class QPropertyAnimation;

// Fluent UI 风格对话框
// 支持 Info/Warning/Error/Success 四种类型，带遮罩层和弹出动画
class FLUENT_EXPORT FluentContentDialog : public QDialog {
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)

public:
    enum class DialogType {
        Info,
        Warning,
        Error,
        Success
    };
    Q_ENUM(DialogType)

    // 对话框弹出位置
    enum class PopupPosition {
        Center,             // 正中心
        TopCenter,          // 顶部居中
        BottomCenter,       // 底部居中
        LeftCenter,         // 左侧垂直居中
        RightCenter,        // 右侧垂直居中
        TopLeft,            // 左上角
        TopRight,           // 右上角
        BottomLeft,         // 左下角
        BottomRight         // 右下角
    };
    Q_ENUM(PopupPosition)

    explicit FluentContentDialog(QWidget* parent = nullptr);

    // 标题和内容
    QString title() const;
    void setTitle(const QString& title);

    QString content() const;
    void setContent(const QString& content);

    DialogType dialogType() const;
    void setDialogType(DialogType type);

    // 弹出位置
    PopupPosition popupPosition() const;
    void setPopupPosition(PopupPosition position);

    // 对话框尺寸
    QSize dialogSize() const;
    void setDialogSize(const QSize& size);
    void setDialogSize(int width, int height);

    // 边距（用于 Left/Right/Top/Bottom 等边缘位置时的间距）
    int margin() const;
    void setMargin(int margin);

    // 按钮
    void setPositiveButton(const QString& text);
    void setNegativeButton(const QString& text);
    void setCloseButtonVisible(bool visible);

    // 显示
    int exec() override;
    void open() override;

    // 透明度（动画用）
    qreal opacity() const;
    void setOpacity(qreal opacity);

signals:
    void positiveClicked();
    void negativeClicked();
    void opacityChanged();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    void setupUI();
    void applyTheme();
    QColor accentColorForType() const;
    QString iconCharForType() const;
    void showWithAnimation();
    void hideWithAnimation();
    QRect calcPanelGeometry() const;

    QString m_title;
    QString m_content;
    DialogType m_type = DialogType::Info;
    PopupPosition m_popupPosition = PopupPosition::Center;
    QSize m_dialogSize = QSize(200, 80);
    int m_margin = 16;

    QWidget* m_backgroundWidget = nullptr;  // 遮罩层
    QWidget* m_dialogPanel = nullptr;        // 对话框面板
    QVBoxLayout* m_panelLayout = nullptr;

    QLabel* m_titleLabel = nullptr;
    QLabel* m_contentLabel = nullptr;

    FluentButton* m_positiveBtn = nullptr;
    FluentButton* m_negativeBtn = nullptr;
    FluentButton* m_closeBtn = nullptr;

    qreal m_opacity = 0.0;
    int m_resultCode = 0;
};

#endif // FLUENTCONTENTDIALOG_H

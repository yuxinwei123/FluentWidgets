#ifndef FLUENTBUTTON_H
#define FLUENTBUTTON_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QPropertyAnimation>

class FluentTheme;

class FLUENT_EXPORT FluentButton : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(FluentButtonVariant variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(FluentSize size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged)

public:
    explicit FluentButton(const QString& text = QString(),
                           FluentButtonVariant variant = FluentButtonVariant::Secondary,
                           QWidget* parent = nullptr);

    QString text() const;
    void setText(const QString& text);

    FluentButtonVariant variant() const;
    void setVariant(FluentButtonVariant variant);

    FluentSize size() const;
    void setSize(FluentSize size);

    QColor accentColor() const;
    void setAccentColor(const QColor& color);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void clicked();
    void textChanged();
    void variantChanged();
    void sizeChanged();
    void accentColorChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    void setupSize();
    void initAnimations();

    QString m_text;
    FluentButtonVariant m_variant = FluentButtonVariant::Secondary;
    FluentSize m_size = FluentSize::Medium;
    QColor m_accentColor;

    // 动画属性
    QPropertyAnimation* m_hoverAnim = nullptr;
    qreal m_hoverProgress = 0.0;

    // 绘制辅助
    QColor backgroundColorForState() const;
    QColor textColorForState() const;
    QColor borderColorForState() const;
};

#endif // FLUENTBUTTON_H

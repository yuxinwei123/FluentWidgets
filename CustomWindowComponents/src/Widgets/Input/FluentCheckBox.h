#ifndef FLUENTCHECKBOX_H
#define FLUENTCHECKBOX_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"

class QPropertyAnimation;

// Fluent UI 风格复选框
class FLUENT_EXPORT FluentCheckBox : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(qreal checkProgress READ checkProgress WRITE setCheckProgress NOTIFY checkProgressChanged)

public:
    explicit FluentCheckBox(const QString& text = QString(), QWidget* parent = nullptr);

    QString text() const;
    void setText(const QString& text);

    bool checked() const;
    void setChecked(bool checked);

    // 三态支持
    bool tristate() const;
    void setTristate(bool enabled);

    Qt::CheckState checkState() const;
    void setCheckState(Qt::CheckState state);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

    qreal checkProgress() const;
    void setCheckProgress(qreal progress);

signals:
    void textChanged();
    void checkedChanged(bool checked);
    void checkStateChanged(Qt::CheckState state);
    void checkProgressChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void toggle();
    QRect boxRect() const;

    QString m_text;
    Qt::CheckState m_state = Qt::Unchecked;
    bool m_tristate = false;
    qreal m_checkProgress = 0.0;  // 0=unchecked, 1=checked, 0.5=partial

    enum { BOX_SIZE = 18, BOX_MARGIN = 2, TEXT_SPACING = 8 };
};

#endif // FLUENTCHECKBOX_H

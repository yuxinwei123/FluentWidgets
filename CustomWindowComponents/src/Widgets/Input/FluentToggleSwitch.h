#ifndef FLUENTTOGGLESWITCH_H
#define FLUENTTOGGLESWITCH_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"

class QPropertyAnimation;

// Fluent UI 风格切换开关 (Toggle Switch)
class FLUENT_EXPORT FluentToggleSwitch : public FluentWidget {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(qreal toggleProgress READ toggleProgress WRITE setToggleProgress NOTIFY toggleProgressChanged)

public:
    explicit FluentToggleSwitch(const QString& text = QString(), QWidget* parent = nullptr);

    QString text() const;
    void setText(const QString& text);

    bool checked() const;
    void setChecked(bool checked);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

    qreal toggleProgress() const;
    void setToggleProgress(qreal progress);

signals:
    void textChanged();
    void checkedChanged(bool checked);
    void toggleProgressChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    QRect trackRect() const;
    qreal thumbX() const;

    QString m_text;
    bool m_checked = false;
    qreal m_toggleProgress = 0.0; // 0=off, 1=on

    enum { TRACK_WIDTH = 40, TRACK_HEIGHT = 20, THUMB_SIZE = 12, THUMB_MARGIN = 4, TEXT_SPACING = 8 };
};

#endif // FLUENTTOGGLESWITCH_H

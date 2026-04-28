#ifndef FLUENTREVEALEFFECT_H
#define FLUENTREVEALEFFECT_H

#include "Core/FluentGlobal.h"
#include <QObject>
#include <QColor>
#include <QPoint>
#include <QRect>

class QWidget;
class QPainter;

class FLUENT_EXPORT FluentRevealEffect : public QObject {
    Q_OBJECT
    Q_PROPERTY(QColor revealColor READ revealColor WRITE setRevealColor NOTIFY effectChanged)
    Q_PROPERTY(qreal revealRadius READ revealRadius WRITE setRevealRadius NOTIFY effectChanged)
    Q_PROPERTY(qreal revealOpacity READ revealOpacity WRITE setRevealOpacity NOTIFY effectChanged)

public:
    explicit FluentRevealEffect(QObject* parent = nullptr);

    QColor revealColor() const;
    void setRevealColor(const QColor& color);

    qreal revealRadius() const;
    void setRevealRadius(qreal radius);

    qreal revealOpacity() const;
    void setRevealOpacity(qreal opacity);

    void attachTo(QWidget* target);
    void detachFrom(QWidget* target);

    void setMousePosition(const QPoint& pos);
    void paint(QPainter* painter, const QRect& rect);

signals:
    void effectChanged();

private:
    QColor m_revealColor;
    qreal m_revealRadius = 80.0;
    qreal m_revealOpacity = 0.15;
    QPoint m_mousePos;
    QWidget* m_target = nullptr;
};

#endif // FLUENTREVEALEFFECT_H

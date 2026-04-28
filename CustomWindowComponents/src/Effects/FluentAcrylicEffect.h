#ifndef FLUENTACRYLICEFFECT_H
#define FLUENTACRYLICEFFECT_H

#include "Core/FluentGlobal.h"
#include <QObject>
#include <QColor>
#include <QRect>

class QWidget;
class QPainter;

class FLUENT_EXPORT FluentAcrylicEffect : public QObject {
    Q_OBJECT
    Q_PROPERTY(int blurRadius READ blurRadius WRITE setBlurRadius NOTIFY effectChanged)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY effectChanged)
    Q_PROPERTY(QColor tintColor READ tintColor WRITE setTintColor NOTIFY effectChanged)
    Q_PROPERTY(qreal tintOpacity READ tintOpacity WRITE setTintOpacity NOTIFY effectChanged)
    Q_PROPERTY(qreal noiseOpacity READ noiseOpacity WRITE setNoiseOpacity NOTIFY effectChanged)

public:
    explicit FluentAcrylicEffect(QObject* parent = nullptr);

    int blurRadius() const;
    void setBlurRadius(int radius);

    qreal opacity() const;
    void setOpacity(qreal opacity);

    QColor tintColor() const;
    void setTintColor(const QColor& color);

    qreal tintOpacity() const;
    void setTintOpacity(qreal opacity);

    qreal noiseOpacity() const;
    void setNoiseOpacity(qreal opacity);

    void attachTo(QWidget* target);
    void detachFrom(QWidget* target);

    void paint(QPainter* painter, const QRect& rect);

signals:
    void effectChanged();

private:
    int m_blurRadius = 30;
    qreal m_opacity = 0.8;
    QColor m_tintColor;
    qreal m_tintOpacity = 0.4;
    qreal m_noiseOpacity = 0.03;
    QWidget* m_target = nullptr;
};

#endif // FLUENTACRYLICEFFECT_H

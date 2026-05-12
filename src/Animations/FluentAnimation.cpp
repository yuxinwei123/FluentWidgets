#include "FluentAnimation.h"
#include <QPointF>

QEasingCurve FluentAnimation::decelerate() {
    // Cubic ease-out: 进入时减速
    return QEasingCurve(QEasingCurve::OutCubic);
}

QEasingCurve FluentAnimation::accelerate() {
    // Cubic ease-in: 退出时加速
    return QEasingCurve(QEasingCurve::InCubic);
}

QEasingCurve FluentAnimation::standard() {
    // Fluent 标准曲线 (近似 WinUI3 的缓动)
    QEasingCurve curve;
    curve.setType(QEasingCurve::BezierSpline);
    // 控制点参考 Fluent Design 的标准缓动
    curve.addCubicBezierSegment(QPointF(0.8, 0.0), QPointF(0.2, 1.0), QPointF(1.0, 1.0));
    return curve;
}

QEasingCurve FluentAnimation::sharp() {
    QEasingCurve curve;
    curve.setType(QEasingCurve::BezierSpline);
    curve.addCubicBezierSegment(QPointF(0.8, 0.0), QPointF(0.6, 1.0), QPointF(1.0, 1.0));
    return curve;
}

QPropertyAnimation* FluentAnimation::create(
    QObject* target,
    const QByteArray& propertyName,
    const QVariant& endValue,
    int duration,
    const QEasingCurve& curve)
{
    auto* anim = new QPropertyAnimation(target, propertyName);
    anim->setEndValue(endValue);
    anim->setDuration(duration);
    anim->setEasingCurve(curve);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    return anim;
}

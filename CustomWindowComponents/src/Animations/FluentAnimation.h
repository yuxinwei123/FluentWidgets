#ifndef FLUENTANIMATION_H
#define FLUENTANIMATION_H

#include "Core/FluentGlobal.h"
#include <QEasingCurve>
#include <QPropertyAnimation>

class FLUENT_EXPORT FluentAnimation {
public:
    // Fluent Design 官方缓动曲线
    static QEasingCurve decelerate();    // 进入动效
    static QEasingCurve accelerate();    // 退出动效
    static QEasingCurve standard();      // 标准动效
    static QEasingCurve sharp();         // 强烈动效

    // 时长常量 (ms)
    static constexpr int fastDuration = 150;
    static constexpr int normalDuration = 250;
    static constexpr int slowDuration = 400;
    static constexpr int entranceDuration = 300;
    static constexpr int exitDuration = 200;

    // 快速创建属性动画
    static QPropertyAnimation* create(
        QObject* target,
        const QByteArray& propertyName,
        const QVariant& endValue,
        int duration = normalDuration,
        const QEasingCurve& curve = standard()
    );
};

#endif // FLUENTANIMATION_H

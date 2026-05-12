#ifndef FLUENTCONFIG_H
#define FLUENTCONFIG_H

#include "FluentGlobal.h"
#include <QObject>

class FLUENT_EXPORT FluentConfig : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool animationEnabled READ animationEnabled WRITE setAnimationEnabled NOTIFY configChanged)
    Q_PROPERTY(bool nativeBlur READ nativeBlur WRITE setNativeBlur NOTIFY configChanged)
    Q_PROPERTY(qreal devicePixelRatio READ devicePixelRatio NOTIFY configChanged)

public:
    static FluentConfig& instance();

    bool animationEnabled() const;
    void setAnimationEnabled(bool enabled);

    bool nativeBlur() const;
    void setNativeBlur(bool enabled);

    qreal devicePixelRatio() const;

signals:
    void configChanged();

private:
    FluentConfig();
    ~FluentConfig() override;
    FluentConfig(const FluentConfig&) = delete;
    FluentConfig& operator=(const FluentConfig&) = delete;

    bool m_animationEnabled = true;
    bool m_nativeBlur = true;
};

#endif // FLUENTCONFIG_H

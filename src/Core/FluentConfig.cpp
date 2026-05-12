#include "FluentConfig.h"
#include <QApplication>

FluentConfig& FluentConfig::instance() {
    static FluentConfig cfg;
    return cfg;
}

FluentConfig::FluentConfig() = default;
FluentConfig::~FluentConfig() = default;

bool FluentConfig::animationEnabled() const { return m_animationEnabled; }
void FluentConfig::setAnimationEnabled(bool enabled) {
    if (m_animationEnabled != enabled) {
        m_animationEnabled = enabled;
        emit configChanged();
    }
}

bool FluentConfig::nativeBlur() const { return m_nativeBlur; }
void FluentConfig::setNativeBlur(bool enabled) {
    if (m_nativeBlur != enabled) {
        m_nativeBlur = enabled;
        emit configChanged();
    }
}

qreal FluentConfig::devicePixelRatio() const {
    return qApp ? qApp->devicePixelRatio() : 1.0;
}

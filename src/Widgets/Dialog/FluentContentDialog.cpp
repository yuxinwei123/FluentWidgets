#include "FluentContentDialog.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include "Widgets/Buttons/FluentButton.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QCloseEvent>
#include <QApplication>
#include <QScreen>

FluentContentDialog::FluentContentDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
    applyTheme();

    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged,
            this, [this]() { applyTheme(); update(); });
}

QString FluentContentDialog::title() const { return m_title; }
void FluentContentDialog::setTitle(const QString& title) {
    m_title = title;
    if (m_titleLabel) m_titleLabel->setText(title);
}

QString FluentContentDialog::content() const { return m_content; }
void FluentContentDialog::setContent(const QString& content) {
    m_content = content;
    if (m_contentLabel) m_contentLabel->setText(content);
}

FluentContentDialog::DialogType FluentContentDialog::dialogType() const { return m_type; }
void FluentContentDialog::setDialogType(DialogType type) {
    m_type = type;
    applyTheme();
    update();
}

FluentContentDialog::PopupPosition FluentContentDialog::popupPosition() const { return m_popupPosition; }
void FluentContentDialog::setPopupPosition(PopupPosition position) {
    m_popupPosition = position;
    update();
}

QSize FluentContentDialog::dialogSize() const { return m_dialogSize; }
void FluentContentDialog::setDialogSize(const QSize& size) {
    m_dialogSize = size;
    if (m_dialogPanel) m_dialogPanel->setFixedSize(size);
    update();
}
void FluentContentDialog::setDialogSize(int w, int h) {
    setDialogSize(QSize(w, h));
}

int FluentContentDialog::margin() const { return m_margin; }
void FluentContentDialog::setMargin(int margin) {
    m_margin = margin;
    update();
}

void FluentContentDialog::setPositiveButton(const QString& text) {
    if (m_positiveBtn) m_positiveBtn->setText(text);
}

void FluentContentDialog::setNegativeButton(const QString& text) {
    if (m_negativeBtn) {
        m_negativeBtn->setText(text);
        m_negativeBtn->setVisible(true);
    }
}

void FluentContentDialog::setCloseButtonVisible(bool visible) {
    if (m_closeBtn) m_closeBtn->setVisible(visible);
}

qreal FluentContentDialog::opacity() const { return m_opacity; }
void FluentContentDialog::setOpacity(qreal opacity) {
    if (qFuzzyCompare(m_opacity, opacity)) return;
    m_opacity = opacity;
    if (m_backgroundWidget) {
        m_backgroundWidget->setStyleSheet(
            QString("background-color: rgba(0,0,0,%1);").arg(0.5 * opacity, 0, 'f', 2));
    }
    if (m_dialogPanel) {
        m_dialogPanel->setWindowOpacity(opacity);
    }
    emit opacityChanged();
}

int FluentContentDialog::exec() {
    showWithAnimation();
    m_resultCode = 0;
    return QDialog::exec();
}

void FluentContentDialog::open() {
    showWithAnimation();
    QDialog::open();
}

void FluentContentDialog::paintEvent(QPaintEvent* /*event*/) {
    // 由 m_dialogPanel 自绘背景
}

QRect FluentContentDialog::calcPanelGeometry() const {
    int pw = m_dialogSize.width();
    int ph = m_dialogSize.height();
    int w = width();
    int h = height();
    int m = m_margin;

    int x = 0, y = 0;

    switch (m_popupPosition) {
    case PopupPosition::Center:
        x = (w - pw) / 2;
        y = (h - ph) / 2;
        break;
    case PopupPosition::TopCenter:
        x = (w - pw) / 2;
        y = m;
        break;
    case PopupPosition::BottomCenter:
        x = (w - pw) / 2;
        y = h - ph - m;
        break;
    case PopupPosition::LeftCenter:
        x = m;
        y = (h - ph) / 2;
        break;
    case PopupPosition::RightCenter:
        x = w - pw - m;
        y = (h - ph) / 2;
        break;
    case PopupPosition::TopLeft:
        x = m;
        y = m;
        break;
    case PopupPosition::TopRight:
        x = w - pw - m;
        y = m;
        break;
    case PopupPosition::BottomLeft:
        x = m;
        y = h - ph - m;
        break;
    case PopupPosition::BottomRight:
        x = w - pw - m;
        y = h - ph - m;
        break;
    }

    return QRect(x, y, pw, ph);
}

void FluentContentDialog::resizeEvent(QResizeEvent* event) {
    QDialog::resizeEvent(event);
    if (m_backgroundWidget) {
        m_backgroundWidget->setGeometry(0, 0, width(), height());
    }
    if (m_dialogPanel) {
        m_dialogPanel->setGeometry(calcPanelGeometry());
    }
}

void FluentContentDialog::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        reject();
        return;
    }
    QDialog::keyPressEvent(event);
}

void FluentContentDialog::closeEvent(QCloseEvent* event) {
    hideWithAnimation();
    event->accept();
}

QColor FluentContentDialog::accentColorForType() const {
    auto* theme = FluentThemeManager::instance().currentTheme();
    switch (m_type) {
    case DialogType::Info:    return theme->primaryColor();
    case DialogType::Warning: return theme->warningColor();
    case DialogType::Error:   return theme->errorColor();
    case DialogType::Success: return theme->successColor();
    }
    return theme->primaryColor();
}

QString FluentContentDialog::iconCharForType() const {
    switch (m_type) {
    case DialogType::Info:    return u8"ℹ";
    case DialogType::Warning: return u8"⚠";
    case DialogType::Error:   return u8"✕";
    case DialogType::Success: return u8"✓";
    }
    return u8"ℹ";
}

void FluentContentDialog::showWithAnimation() {
    if (parentWidget()) {
        resize(parentWidget()->size());
    } else {
        QScreen* screen = QApplication::primaryScreen();
        if (screen) {
            resize(screen->availableGeometry().size() * 0.6);
        }
    }

    auto* anim = new QPropertyAnimation(this, "opacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(200);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &QAbstractAnimation::finished, anim, &QObject::deleteLater);
    anim->start();
}

void FluentContentDialog::hideWithAnimation() {
    auto* anim = new QPropertyAnimation(this, "opacity");
    anim->setStartValue(m_opacity);
    anim->setEndValue(0.0);
    anim->setDuration(150);
    anim->setEasingCurve(QEasingCurve::InCubic);
    connect(anim, &QAbstractAnimation::finished, anim, &QObject::deleteLater);
    anim->start();
}

void FluentContentDialog::setupUI() {
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowModal, true);

    // 不使用布局，手动定位子控件（遮罩层 + 对话框面板）
    // 这样 setGeometry 才能生效，支持多种弹出位置

    // 遮罩层
    m_backgroundWidget = new QWidget(this);
    m_backgroundWidget->setStyleSheet("background-color: rgba(0,0,0,0.50);");
    m_backgroundWidget->lower();

    // 对话框面板
    m_dialogPanel = new QWidget(this);
    m_dialogPanel->setObjectName("fluentDialogPanel");
    m_dialogPanel->setFixedSize(m_dialogSize);

    auto* panelLayout = new QVBoxLayout(m_dialogPanel);
    panelLayout->setContentsMargins(12, 8, 12, 8);
    panelLayout->setSpacing(4);

    // 标题
    m_titleLabel = new QLabel(m_dialogPanel);
    m_titleLabel->setObjectName("fluentDialogTitle");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    panelLayout->addWidget(m_titleLabel);

    // 内容
    m_contentLabel = new QLabel(m_dialogPanel);
    m_contentLabel->setObjectName("fluentDialogContent");
    m_contentLabel->setWordWrap(true);
    m_contentLabel->setAlignment(Qt::AlignCenter);
    m_contentLabel->setTextFormat(Qt::PlainText);
    panelLayout->addWidget(m_contentLabel);

    // 按钮行
    auto* btnRow = new QHBoxLayout();
    btnRow->setSpacing(6);
    btnRow->addStretch();

    m_negativeBtn = new FluentButton(u8"取消", FluentButtonVariant::Secondary, m_dialogPanel);
    m_negativeBtn->setVisible(false);
    connect(m_negativeBtn, &FluentButton::clicked, this, [this]() {
        m_resultCode = 0;
        reject();
        emit negativeClicked();
    });

    m_positiveBtn = new FluentButton(u8"确定", FluentButtonVariant::Primary, m_dialogPanel);
    connect(m_positiveBtn, &FluentButton::clicked, this, [this]() {
        m_resultCode = 1;
        accept();
        emit positiveClicked();
    });

    btnRow->addWidget(m_negativeBtn);
    btnRow->addWidget(m_positiveBtn);
    btnRow->addStretch();
    panelLayout->addLayout(btnRow);
}

void FluentContentDialog::applyTheme() {
    auto* theme = FluentThemeManager::instance().currentTheme();

    m_dialogPanel->setStyleSheet(
        QString("#fluentDialogPanel {"
                "  background-color: %1;"
                "  border: 1px solid %2;"
                "  border-radius: 8px;"
                "}"
                "#fluentDialogTitle {"
                "  color: %3;"
                "  font-size: 14px;"
                "  font-weight: bold;"
                "}"
                "#fluentDialogContent {"
                "  color: %4;"
                "  font-size: 12px;"
                "}")
        .arg(theme->backgroundColor().name())
        .arg(theme->borderColor().name())
        .arg(theme->textColorPrimary().name())
        .arg(theme->textColorSecondary().name())
    );

    if (m_positiveBtn) {
        if (m_type == DialogType::Error) {
            m_positiveBtn->setVariant(FluentButtonVariant::Danger);
        } else {
            m_positiveBtn->setVariant(FluentButtonVariant::Primary);
        }
    }

    if (m_titleLabel && !m_title.isEmpty()) m_titleLabel->setText(m_title);
    if (m_contentLabel && !m_content.isEmpty()) m_contentLabel->setText(m_content);
}

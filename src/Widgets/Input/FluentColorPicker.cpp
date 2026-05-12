#include "FluentColorPicker.h"
#include "Core/FluentTheme.h"
#include "Core/FluentThemeManager.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QScreen>

// ========== 默认预设色板 ==========

static QVector<QColor> defaultPresetColors() {
    return QVector<QColor> {
        QColor("#000000"), QColor("#434343"), QColor("#666666"), QColor("#999999"), QColor("#B7B7B7"),
        QColor("#CCCCCC"), QColor("#D9D9D9"), QColor("#EFEFEF"), QColor("#F3F3F3"), QColor("#FFFFFF"),
        QColor("#980000"), QColor("#FF0000"), QColor("#FF9900"), QColor("#FFFF00"), QColor("#00FF00"),
        QColor("#00FFFF"), QColor("#4A86E8"), QColor("#0000FF"), QColor("#9900FF"), QColor("#FF00FF"),
        QColor("#E6B8AF"), QColor("#F4CCCC"), QColor("#FCE5CD"), QColor("#FFF2CC"), QColor("#D9EAD3"),
        QColor("#D0E0E3"), QColor("#C9DAF8"), QColor("#CFE2F3"), QColor("#D9D2E9"), QColor("#EAD1DC"),
        QColor("#DD7E6B"), QColor("#EA9999"), QColor("#F9CB9C"), QColor("#FFE599"), QColor("#B6D7A8"),
        QColor("#A2C4C9"), QColor("#A4C2F4"), QColor("#9FC5E8"), QColor("#B4A7D6"), QColor("#D5A6BD"),
        QColor("#CC4125"), QColor("#E06666"), QColor("#F6B26B"), QColor("#FFD966"), QColor("#93C47D"),
        QColor("#76A5AF"), QColor("#6D9EEB"), QColor("#6FA8DC"), QColor("#8E7CC3"), QColor("#C27BA0"),
        QColor("#A61C00"), QColor("#CC0000"), QColor("#E69138"), QColor("#F1C232"), QColor("#6AA84F"),
        QColor("#45818E"), QColor("#3C78D8"), QColor("#3D85C6"), QColor("#674EA7"), QColor("#A64D79"),
    };
}

// ========== ColorGrid ==========

ColorGrid::ColorGrid(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    m_colors = defaultPresetColors();
}

void ColorGrid::setColors(const QVector<QColor>& colors) {
    m_colors = colors;
    update();
    updateGeometry();
}

void ColorGrid::setSelectedColor(const QColor& color) {
    m_selectedColor = color;
    update();
}

QSize ColorGrid::sizeHint() const {
    int rows = (m_colors.size() + m_columns - 1) / m_columns;
    int w = m_columns * (m_cellSize + m_spacing) - m_spacing;
    int h = rows * (m_cellSize + m_spacing) - m_spacing;
    return QSize(w, h);
}

void ColorGrid::paintEvent(QPaintEvent* /*event*/) {
    auto* theme = FluentThemeManager::instance().currentTheme();
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    for (int i = 0; i < m_colors.size(); ++i) {
        int row = i / m_columns;
        int col = i % m_columns;
        qreal x = col * (m_cellSize + m_spacing);
        qreal y = row * (m_cellSize + m_spacing);
        QRectF rect(x, y, m_cellSize, m_cellSize);

        bool isHover = (i == m_hoverIndex);
        bool isSelected = (m_colors[i] == m_selectedColor);

        // 悬停/选中边框
        if (isSelected) {
            p.setPen(QPen(theme->primaryColor(), 2.5));
            p.setBrush(Qt::NoBrush);
            p.drawRoundedRect(rect.adjusted(-2, -2, 2, 2), 4, 4);
        } else if (isHover) {
            p.setPen(QPen(theme->textColorSecondary(), 1.5));
            p.setBrush(Qt::NoBrush);
            p.drawRoundedRect(rect.adjusted(-1, -1, 1, 1), 3, 3);
        }

        // 色块
        p.setPen(Qt::NoPen);
        p.setBrush(m_colors[i]);
        p.drawRoundedRect(rect, 3, 3);

        // 白色/浅色色块加边框
        if (m_colors[i].lightness() > 240) {
            p.setPen(QPen(theme->dividerColor(), 1));
            p.setBrush(Qt::NoBrush);
            p.drawRoundedRect(rect, 3, 3);
        }
    }
}

void ColorGrid::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        for (int i = 0; i < m_colors.size(); ++i) {
            int row = i / m_columns;
            int col = i % m_columns;
            QRect rect(col * (m_cellSize + m_spacing), row * (m_cellSize + m_spacing),
                       m_cellSize, m_cellSize);
            if (rect.contains(event->pos())) {
                m_selectedColor = m_colors[i];
                emit colorClicked(m_colors[i]);
                update();
                return;
            }
        }
    }
}

void ColorGrid::mouseMoveEvent(QMouseEvent* event) {
    int newHover = -1;
    for (int i = 0; i < m_colors.size(); ++i) {
        int row = i / m_columns;
        int col = i % m_columns;
        QRect rect(col * (m_cellSize + m_spacing), row * (m_cellSize + m_spacing),
                   m_cellSize, m_cellSize);
        if (rect.contains(event->pos())) {
            newHover = i;
            break;
        }
    }
    if (newHover != m_hoverIndex) {
        m_hoverIndex = newHover;
        setCursor(newHover >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
    }
}

void ColorGrid::leaveEvent(QEvent* /*event*/) {
    m_hoverIndex = -1;
    setCursor(Qt::ArrowCursor);
    update();
}

// ========== AlphaSlider ==========

AlphaSlider::AlphaSlider(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(20);
    setMouseTracking(true);
}

void AlphaSlider::setColor(const QColor& color) {
    m_color = color;
    update();
}

int AlphaSlider::alpha() const { return m_alpha; }

void AlphaSlider::setAlpha(int alpha) {
    m_alpha = qBound(0, alpha, 255);
    update();
}

QSize AlphaSlider::sizeHint() const { return QSize(240, 20); }

void AlphaSlider::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF r = QRectF(0, 0, width(), height()).adjusted(2, 2, -2, -2);

    // 棋盘格背景（表示透明）
    int checkSize = 6;
    for (int y = 0; y < r.height(); y += checkSize) {
        for (int x = 0; x < r.width(); x += checkSize) {
            bool light = ((x / checkSize) + (y / checkSize)) % 2 == 0;
            p.fillRect(QRectF(r.x() + x, r.y() + y, checkSize, checkSize),
                       light ? QColor(255, 255, 255) : QColor(204, 204, 204));
        }
    }

    // 渐变叠加
    QLinearGradient grad(r.left(), 0, r.right(), 0);
    QColor transparent = m_color;
    transparent.setAlpha(0);
    QColor opaque = m_color;
    opaque.setAlpha(255);
    grad.setColorAt(0, transparent);
    grad.setColorAt(1, opaque);
    p.fillRect(r, grad);

    // 滑块
    qreal ratio = m_alpha / 255.0;
    qreal thumbX = r.left() + ratio * r.width();
    QRectF thumbRect(thumbX - 5, r.top() - 1, 10, r.height() + 2);
    p.setPen(QPen(Qt::white, 1.5));
    p.setBrush(m_color);
    p.drawRoundedRect(thumbRect, 3, 3);

    // 外框
    p.setPen(QPen(QColor(180, 180, 180), 1));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(r, 3, 3);
}

void AlphaSlider::mousePressEvent(QMouseEvent* event) {
    m_dragging = true;
    QRectF r = QRectF(0, 0, width(), height()).adjusted(2, 2, -2, -2);
    qreal ratio = qBound(0.0, (event->pos().x() - r.left()) / r.width(), 1.0);
    m_alpha = qRound(ratio * 255);
    emit alphaChanged(m_alpha);
    update();
}

void AlphaSlider::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        QRectF r = QRectF(0, 0, width(), height()).adjusted(2, 2, -2, -2);
        qreal ratio = qBound(0.0, (event->pos().x() - r.left()) / r.width(), 1.0);
        m_alpha = qRound(ratio * 255);
        emit alphaChanged(m_alpha);
        update();
    }
}

// ========== HexInput ==========

HexInput::HexInput(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(28);
    setFocusPolicy(Qt::StrongFocus);
}

void HexInput::setColor(const QColor& color) {
    if (color.alpha() < 255) {
        m_text = color.name(QColor::HexArgb);
    } else {
        m_text = color.name(QColor::HexRgb);
    }
    update();
}

QColor HexInput::color() const {
    return QColor(m_text);
}

QSize HexInput::sizeHint() const { return QSize(100, 28); }

void HexInput::paintEvent(QPaintEvent* /*event*/) {
    auto* theme = FluentThemeManager::instance().currentTheme();
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF r(0, 0, width(), height());
    QColor border = m_hasFocus ? theme->primaryColor() : theme->borderColor();
    p.setPen(QPen(border, m_hasFocus ? 1.5 : 1));
    p.setBrush(theme->backgroundColor());
    p.drawRoundedRect(r, 4, 4);

    p.setFont(theme->captionFont());
    p.setPen(theme->textColorPrimary());
    p.drawText(r.adjusted(8, 0, -4, 0), Qt::AlignLeft | Qt::AlignVCenter, m_text);
}

void HexInput::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QColor c(m_text);
        if (c.isValid()) {
            emit colorSubmitted(c);
        }
    } else if (event->key() == Qt::Key_Backspace) {
        if (!m_text.isEmpty()) {
            m_text.chop(1);
            update();
        }
    } else if (event->key() == Qt::Key_Delete) {
        m_text.clear();
        update();
    } else {
        QString ch = event->text();
        if (!ch.isEmpty() && ch.size() == 1) {
            QChar c = ch[0].toUpper();
            if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || c == '#') {
                m_text += c;
                update();
            }
        }
    }
}

void HexInput::focusInEvent(QFocusEvent* /*event*/) {
    m_hasFocus = true;
    update();
}

void HexInput::focusOutEvent(QFocusEvent* /*event*/) {
    m_hasFocus = false;
    update();
}

// ========== FluentColorPicker ==========

FluentColorPicker::FluentColorPicker(QWidget* parent)
    : FluentWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumHeight(HEIGHT);
    setMinimumWidth(160);
    setMouseTracking(true);
    m_presetColors = defaultPresetColors();

    // 弹出面板
    m_popup = new QWidget(nullptr);
    m_popup->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    m_popup->installEventFilter(this);

    auto* popupLayout = new QVBoxLayout(m_popup);
    popupLayout->setContentsMargins(12, 12, 12, 12);
    popupLayout->setSpacing(10);

    // 色板标签
    auto* gridLabel = new QLabel(u8"预设颜色", m_popup);
    popupLayout->addWidget(gridLabel);

    // 色板网格
    m_colorGrid = new ColorGrid(m_popup);
    popupLayout->addWidget(m_colorGrid);

    // 透明度滑块
    m_alphaSlider = new AlphaSlider(m_popup);
    m_alphaSlider->hide();
    popupLayout->addWidget(m_alphaSlider);

    // HEX 输入
    auto* hexRow = new QHBoxLayout();
    auto* hexLabel = new QLabel(u8"HEX:", m_popup);
    m_hexInput = new HexInput(m_popup);
    hexRow->addWidget(hexLabel);
    hexRow->addWidget(m_hexInput, 1);
    popupLayout->addLayout(hexRow);

    // 连接信号
    connect(m_colorGrid, &ColorGrid::colorClicked, this, [this](const QColor& color) {
        setColor(color);
    });

    connect(m_alphaSlider, &AlphaSlider::alphaChanged, this, [this](int alpha) {
        QColor c = m_color;
        c.setAlpha(alpha);
        m_color = c;
        m_hasColor = true;
        emit colorChanged(m_color);
        update();
    });

    connect(m_hexInput, &HexInput::colorSubmitted, this, [this](const QColor& color) {
        setColor(color);
    });

    connect(&FluentThemeManager::instance(), &FluentThemeManager::themeChanged, this, [this]() {
        update();
        updatePopupTheme();
    });
}

FluentColorPicker::~FluentColorPicker() {
    if (m_popup) {
        m_popup->hide();
        m_popup->deleteLater();
    }
}

// === 属性 ===

QColor FluentColorPicker::color() const { return m_color; }
void FluentColorPicker::setColor(const QColor& color) {
    if (m_color != color || !m_hasColor) {
        m_color = color;
        m_hasColor = color.isValid();
        if (m_colorGrid) m_colorGrid->setSelectedColor(color);
        if (m_alphaSlider) { m_alphaSlider->setColor(color); m_alphaSlider->setAlpha(color.alpha()); }
        if (m_hexInput) m_hexInput->setColor(color);
        emit colorChanged(color);
        update();
    }
}

void FluentColorPicker::clear() {
    m_hasColor = false;
    m_color = QColor();
    update();
}

bool FluentColorPicker::showAlpha() const { return m_showAlpha; }
void FluentColorPicker::setShowAlpha(bool on) {
    if (m_showAlpha != on) {
        m_showAlpha = on;
        if (m_alphaSlider) m_alphaSlider->setVisible(on);
        emit showAlphaChanged();
    }
}

QString FluentColorPicker::placeholder() const { return m_placeholder; }
void FluentColorPicker::setPlaceholder(const QString& text) {
    if (m_placeholder != text) { m_placeholder = text; emit placeholderChanged(); update(); }
}

void FluentColorPicker::setPresetColors(const QVector<QColor>& colors) {
    m_presetColors = colors;
    if (m_colorGrid) m_colorGrid->setColors(colors);
}

QVector<QColor> FluentColorPicker::presetColors() const { return m_presetColors; }

QSize FluentColorPicker::sizeHint() const { return QSize(200, HEIGHT); }
QSize FluentColorPicker::minimumSizeHint() const { return QSize(160, HEIGHT); }

// === 布局 ===

QRect FluentColorPicker::previewRect() const {
    return QRect(LEFT_PADDING, (height() - PREVIEW_SIZE) / 2, PREVIEW_SIZE, PREVIEW_SIZE);
}

QRect FluentColorPicker::textRect() const {
    return QRect(LEFT_PADDING + PREVIEW_SIZE + PREVIEW_MARGIN, 0,
                 width() - LEFT_PADDING - PREVIEW_SIZE - PREVIEW_MARGIN - RIGHT_PADDING, height());
}

QString FluentColorPicker::colorText() const {
    if (!m_hasColor || !m_color.isValid()) return QString();
    if (m_showAlpha && m_color.alpha() < 255) {
        return m_color.name(QColor::HexArgb);
    }
    return m_color.name(QColor::HexRgb);
}

// === 绘制 ===

void FluentColorPicker::paintFluent(QPainter* painter) {
    auto* theme = FluentThemeManager::instance().currentTheme();

    QRectF bgRect(0, 0, width(), height());
    painter->setRenderHint(QPainter::Antialiasing);

    QColor borderColor = hasFocus() ? theme->primaryColor() : theme->borderColor();
    qreal borderWidth = hasFocus() ? 2.0 : 1.0;

    if (isHovered() && !hasFocus()) {
        borderColor = theme->primaryColor().lighter(150);
    }

    painter->setPen(QPen(borderColor, borderWidth));
    painter->setBrush(theme->cardColor());
    painter->drawRoundedRect(bgRect.adjusted(0.5, 0.5, -0.5, -0.5), 6, 6);

    // 底部强调线
    if (hasFocus()) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(theme->primaryColor());
        painter->drawRoundedRect(QRectF(2, height() - 2.5, width() - 4, 2), 1, 1);
    }

    // 颜色预览色块
    QRect prevR = previewRect();
    if (m_hasColor && m_color.isValid()) {
        // 棋盘格背景
        painter->setPen(Qt::NoPen);
        int cs = 4;
        for (int y = 0; y < prevR.height(); y += cs) {
            for (int x = 0; x < prevR.width(); x += cs) {
                bool light = ((x / cs) + (y / cs)) % 2 == 0;
                painter->setBrush(light ? QColor(255, 255, 255) : QColor(204, 204, 204));
                painter->drawRect(prevR.x() + x, prevR.y() + y, cs, cs);
            }
        }
        painter->setBrush(m_color);
        painter->drawRoundedRect(prevR, 4, 4);
    } else {
        painter->setBrush(theme->dividerColor());
        painter->drawRoundedRect(prevR, 4, 4);
    }

    // 文本
    QFont textFont = theme->bodyFont();
    painter->setFont(textFont);

    QString display = colorText();
    if (!display.isEmpty()) {
        painter->setPen(theme->textColorPrimary());
        painter->drawText(textRect(), Qt::AlignLeft | Qt::AlignVCenter, display);
    } else if (!m_placeholder.isEmpty()) {
        painter->setPen(theme->textColorDisabled());
        painter->drawText(textRect(), Qt::AlignLeft | Qt::AlignVCenter, m_placeholder);
    }
}

// === 弹出 ===

void FluentColorPicker::showPopup() {
    if (m_popupVisible) return;

    if (m_hasColor && m_color.isValid()) {
        m_colorGrid->setSelectedColor(m_color);
        m_alphaSlider->setColor(m_color);
        m_alphaSlider->setAlpha(m_color.alpha());
        m_hexInput->setColor(m_color);
    }
    updatePopupTheme();

    QPoint globalPos = mapToGlobal(QPoint(0, height() + 4));
    m_popup->adjustSize();

    QScreen* screen = QApplication::screenAt(globalPos);
    if (screen) {
        QRect screenRect = screen->availableGeometry();
        if (globalPos.y() + m_popup->height() > screenRect.bottom()) {
            globalPos = mapToGlobal(QPoint(0, -m_popup->height() - 4));
        }
        if (globalPos.x() + m_popup->width() > screenRect.right()) {
            globalPos.setX(screenRect.right() - m_popup->width() - 4);
        }
    }

    m_popup->move(globalPos);
    m_popup->show();
    m_popupVisible = true;
    update();
}

void FluentColorPicker::hidePopup() {
    if (m_popup) m_popup->hide();
    m_popupVisible = false;
    update();
}

void FluentColorPicker::updatePopupTheme() {
    if (!m_popup) return;
    auto* theme = FluentThemeManager::instance().currentTheme();
    m_popup->setAutoFillBackground(true);
    QPalette pal = m_popup->palette();
    pal.setColor(QPalette::Window, theme->cardColor());
    m_popup->setPalette(pal);

    m_popup->setStyleSheet(QString(
        "QWidget { background-color: %1; }"
        "QLabel { color: %2; font-size: 12px; }"
    ).arg(theme->cardColor().name(), theme->textColorSecondary().name()));
}

// === 事件 ===

void FluentColorPicker::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (m_popupVisible) hidePopup();
        else showPopup();
    }
    FluentWidget::mousePressEvent(event);
}

void FluentColorPicker::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_Space:
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if (m_popupVisible) hidePopup();
        else showPopup();
        break;
    case Qt::Key_Escape:
        if (m_popupVisible) hidePopup();
        break;
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        clear();
        break;
    default:
        FluentWidget::keyPressEvent(event);
    }
}

void FluentColorPicker::focusInEvent(QFocusEvent* event) {
    FluentWidget::focusInEvent(event);
    update();
}

void FluentColorPicker::focusOutEvent(QFocusEvent* event) {
    FluentWidget::focusOutEvent(event);
    QTimer::singleShot(150, this, [this]() {
        if (!hasFocus() && m_popupVisible && !m_popup->hasFocus()) {
            QWidget* focusW = QApplication::focusWidget();
            if (!m_popup->isAncestorOf(focusW)) {
                hidePopup();
            }
        }
    });
    update();
}

bool FluentColorPicker::eventFilter(QObject* obj, QEvent* event) {
    if (obj == m_popup) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            QPoint globalPos = me->globalPos();
            QPoint localPos = m_popup->mapFromGlobal(globalPos);
            if (!m_popup->rect().contains(localPos)) {
                hidePopup();
                return true;
            }
        } else if (event->type() == QEvent::Hide) {
            m_popupVisible = false;
            update();
        }
    }
    return FluentWidget::eventFilter(obj, event);
}

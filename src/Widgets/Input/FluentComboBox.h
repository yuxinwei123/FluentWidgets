#ifndef FLUENTCOMBOBOX_H
#define FLUENTCOMBOBOX_H

#include "Widgets/Base/FluentWidget.h"
#include "Core/FluentGlobal.h"
#include <QStringList>
#include <QVariant>
#include <QIcon>
// Fluent UI 风格下拉选择控件
// 支持: 下拉列表、可编辑模式、搜索过滤、多列显示、主题适配

class FLUENT_EXPORT FluentComboBox : public FluentWidget {
    friend void paintPopup(QWidget* popup, FluentComboBox* combo, int hoveredRow, int scrollOffset);
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString currentText READ currentText NOTIFY currentTextChanged)
    Q_PROPERTY(bool editable READ editable WRITE setEditable NOTIFY editableChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit FluentComboBox(QWidget* parent = nullptr);
    ~FluentComboBox() override;

    // 项操作
    void addItem(const QString& text, const QVariant& userData = QVariant());
    void addItem(const QIcon& icon, const QString& text, const QVariant& userData = QVariant());
    void addItems(const QStringList& texts);
    void insertItem(int index, const QString& text, const QVariant& userData = QVariant());
    void removeItem(int index);
    void clear();

    int count() const;
    int currentIndex() const;
    void setCurrentIndex(int index);

    QString currentText() const;
    QString itemText(int index) const;
    QVariant itemData(int index) const;
    QIcon itemIcon(int index) const;

    // 可编辑
    bool editable() const;
    void setEditable(bool on);

    // 占位文字
    QString placeholder() const;
    void setPlaceholder(const QString& text);

    // 弹出列表最大可见项数
    int maxVisibleItems() const;
    void setMaxVisibleItems(int n);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void paintFluent(QPainter* painter) override;

signals:
    void currentIndexChanged(int index);
    void currentTextChanged(const QString& text);
    void editableChanged();
    void countChanged();
    void activated(int index);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    // 弹出列表
    void showPopup();
    void hidePopup();
    void updatePopupGeometry();
    QRect arrowRect() const;

    // 数据结构
    struct ComboItem {
        QString text;
        QVariant userData;
        QIcon icon;
    };
    QList<ComboItem> m_items;
    int m_currentIndex = -1;
    bool m_editable = false;
    QString m_placeholder;
    int m_maxVisibleItems = 10;

    // 编辑
    QString m_editText;
    int m_cursorPos = 0;
    int m_scrollOffset = 0;
    int m_selectionStart = 0;
    int m_selectionEnd = 0;
    bool m_selecting = false;
    bool m_cursorBlink = true;
    int m_blinkTimerId = 0;

    // 弹出列表
    QWidget* m_popup = nullptr;
    int m_popupHoveredRow = -1;
    int m_scrollOffsetY = 0;
    int m_maxPopupHeight = 0;

    // 交互
    bool m_pressed = false;
    bool m_popupVisible = false;

    // 常量
    enum { HEIGHT = 32, ARROW_WIDTH = 28, BORDER_RADIUS = 4,
           LEFT_PADDING = 10, POPUP_ITEM_HEIGHT = 32, POPUP_BORDER_RADIUS = 8,
           SCROLL_BAR_WIDTH = 6 };
};

#endif // FLUENTCOMBOBOX_H

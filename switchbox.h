#ifndef SWITCHBOX_H
#define SWITCHBOX_H

#include <QObject>
#include <QCheckBox>

class SwitchBox : public QCheckBox {
    Q_OBJECT
    Q_PROPERTY(QColor switchColorOn READ switchColorOn WRITE setSwitchColorOn)
    Q_PROPERTY(QColor switchColorOff READ switchColorOff WRITE setSwitchColorOff)
    Q_PROPERTY(QColor switchButtonColor READ switchButtonColor WRITE setSwitchButtonColor)

public:
    explicit SwitchBox(QWidget *parent = nullptr)
        : QCheckBox(parent),
        m_on(Qt::blue),
        m_off(Qt::gray),
        m_button(Qt::white) {}

    QColor switchColorOn() const { return m_on; }
    void setSwitchColorOn(const QColor &c) { m_on = c; }

    QColor switchColorOff() const { return m_off; }
    void setSwitchColorOff(const QColor &c) { m_off = c; }

    QColor switchButtonColor() const { return m_button; }
    void setSwitchButtonColor(const QColor &c) { m_button = c; }

private:
    QColor m_on;
    QColor m_off;
    QColor m_button;
};

#endif // SWITCHBOX_H

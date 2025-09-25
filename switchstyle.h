#ifndef SWITCHSTYLE_H
#define SWITCHSTYLE_H

#include <QProxyStyle>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QPainter>
#include <QRect>
#include <QColor>

class SwitchStyle : public QProxyStyle {
public:
    using QProxyStyle::QProxyStyle;

    // QSS から色を取得（プロパティベース）
    QColor getSwitchColor(const QWidget *widget, bool checked) const {
        const char *propName = checked ? "switchColorOn" : "switchColorOff";
        QVariant colorProp = widget ? widget->property(propName) : QVariant();
        if (colorProp.canConvert<QColor>())
            return colorProp.value<QColor>();
        return checked ? QColor("#1E90FF") : QColor("#AAAAAA"); // デフォルト
    }

    QColor getButtonColor(const QWidget *widget) const {
        QVariant colorProp = widget ? widget->property("switchButtonColor") : QVariant();
        if (colorProp.canConvert<QColor>())
            return colorProp.value<QColor>();
        return Qt::white; // デフォルト
    }

    QRect subElementRect(SubElement element,
                         const QStyleOption *option,
                         const QWidget *widget) const override {
        if (element == SE_CheckBoxIndicator) {
            // チェックボックス全体の矩形
            QRect fullRect = option->rect;

            // インジケータのサイズ（例：40x20）
            int w = 40;
            int h = 20;

            // fullRect の右端にインジケータを配置
            int x = fullRect.right() - w;
            int y = fullRect.top() + (fullRect.height() - h) / 2;

            return QRect(x, y, w, h);
        }
        return QProxyStyle::subElementRect(element, option, widget);
    }

    QSize sizeFromContents(ContentsType type,
                           const QStyleOption *option,
                           const QSize &contentsSize,
                           const QWidget *widget) const override {
        QSize sz = QProxyStyle::sizeFromContents(type, option, contentsSize, widget);
        if (type == CT_CheckBox)
        {
            sz.setHeight(std::max(sz.height(), 24));
            sz.setWidth(std::max(sz.width(), 80)); // ← 幅を確保
        }
        return sz;
    }

    int pixelMetric(PixelMetric metric,
                    const QStyleOption *option,
                    const QWidget *widget) const override {
        if (metric == PM_IndicatorWidth) return 40;
        if (metric == PM_IndicatorHeight) return 20;
        return QProxyStyle::pixelMetric(metric, option, widget);
    }

    void drawPrimitive(PrimitiveElement element,
                       const QStyleOption *option,
                       QPainter *painter,
                       const QWidget *widget = nullptr) const override {
        if (element == PE_IndicatorCheckBox) {
            if (!option) return;
            QRect r = option->rect;
            painter->setRenderHint(QPainter::Antialiasing);

            bool checked = option->state & State_On;
            QColor trackColor = getSwitchColor(widget, checked);

            // 背景（トラック）
            painter->setBrush(trackColor);
            painter->setPen(Qt::NoPen);
            painter->drawRoundedRect(r, r.height()/2, r.height()/2);

            // つまみ（白い円）
            int knobDiameter = r.height() - 4;
            int x = checked ? r.right() - knobDiameter - 2 : r.left() + 2;
            QRect knobRect(x, r.top() + 2, knobDiameter, knobDiameter);

            painter->setBrush(getButtonColor(widget));
            painter->drawEllipse(knobRect);
            return;
        }
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};

#endif // SWITCHSTYLE_H

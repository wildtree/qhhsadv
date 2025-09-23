#ifndef ZCREDITROLL_H
#define ZCREDITROLL_H
#include <QWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>

class ZCreditRoll : QGraphicsView
{
public:
    ZCreditRoll(QWidget *parent = nullptr, const QString &credits = "") : QGraphicsView(parent) {
        setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        setAttribute(Qt::WA_DeleteOnClose);
        setStyleSheet("background-color: black;");

        scene = new QGraphicsScene(this);
        setScene(scene);

        textItem = new QGraphicsTextItem();
        textItem->setHtml(credits);
        //textItem = scene->addText(credits);
        textItem->setDefaultTextColor(Qt::white);
        QFont f("Sans", 16);
        textItem->setFont(f);
        scene->addItem(textItem);

        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        setRenderHint(QPainter::Antialiasing);
        setRenderHint(QPainter::TextAntialiasing);
        setAlignment(Qt::AlignCenter);
    }

    void startRoll(int durationMs = 20000, QWidget *parent = nullptr) {
        // 親ウィンドウに重ねる
        QRect r = parent->rect();
        QPoint topLeft = parent->mapToGlobal(QPoint(0,0));
        setGeometry(QRect(topLeft, r.size()));
        show();

        // テキスト幅をビュー幅に合わせてワードラップ
        textItem->setTextWidth(width() * 0.8); // 画面幅の80%に収める

        QRectF textRect = textItem->boundingRect();

        // 開始位置: 画面の下の外側
        qreal startY = height();
        // 終了位置: テキストが上に抜けきる
        qreal endY = -textRect.height();

        // X座標は中央に配置
        textItem->setPos((width() - textRect.width())/2.0, startY);

        // アニメーション
        QPropertyAnimation *anim = new QPropertyAnimation(textItem, "y", this);
        anim->setDuration(durationMs);
        anim->setStartValue(startY);
        anim->setEndValue(endY);
        anim->setEasingCurve(QEasingCurve::Linear);

        connect(anim, &QPropertyAnimation::finished, this, &QWidget::close);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

private:
    QGraphicsScene *scene;
    QGraphicsTextItem *textItem;
};

#endif // ZCREDITROLL_H

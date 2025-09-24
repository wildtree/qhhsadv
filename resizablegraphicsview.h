#ifndef RESIZABLEGRAPHICSVIEW_H
#define RESIZABLEGRAPHICSVIEW_H

#include <QGraphicsView>

class ResizableGraphicsView : public QGraphicsView {
public:
    using QGraphicsView::QGraphicsView;
protected:
    void resizeEvent(QResizeEvent *event) override {
        QGraphicsView::resizeEvent(event);
        if (scene() && !scene()->sceneRect().isEmpty()) {
            fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
        }
    }
};
#endif // RESIZABLEGRAPHICSVIEW_H

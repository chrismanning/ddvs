#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent=0) : QGraphicsView(parent)
    {
        setDragMode(ScrollHandDrag);
        setRenderHints(QPainter::Antialiasing|
                       QPainter::TextAntialiasing);
    }
public slots:
    void zoomIn() { scaleBy(1.1); }
    void zoomOut() { scaleBy(1.0 / 1.1); }
protected:
    void wheelEvent(QWheelEvent *event)
    {
        scaleBy(std::pow(4.0 / 3.0, (-event->delta() / 240.0)));
    }
private:
    void scaleBy(double factor) {
        scale(factor, factor);
    }
};

#endif // GRAPHICSVIEW_H

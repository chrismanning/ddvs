#ifndef ZOOMWIDGET_H
#define ZOOMWIDGET_H

#include <QWidget>
#include <QGraphicsView>

namespace Ui {
    class ZoomWidget;
}

class ZoomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ZoomWidget(QGraphicsView * view, QWidget *parent = 0);
    ~ZoomWidget();

private slots:
    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_horizontalSlider_valueChanged(int value);

private:
    Ui::ZoomWidget *ui;
    QGraphicsView * view;
};

#endif // ZOOMWIDGET_H

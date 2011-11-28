#include "zoomwidget.h"
#include "ui_zoomwidget.h"

ZoomWidget::ZoomWidget(QGraphicsView * view, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ZoomWidget)
{
    ui->setupUi(this);

    this->view = view;
}

ZoomWidget::~ZoomWidget()
{
    delete ui;
}

//zoom out
void ZoomWidget::on_toolButton_clicked()
{
    ui->horizontalSlider->setValue(ui->horizontalSlider->value()-10);
}

//zoom in
void ZoomWidget::on_toolButton_2_clicked()
{
    ui->horizontalSlider->setValue(ui->horizontalSlider->value()+10);
}

void ZoomWidget::on_horizontalSlider_valueChanged(int value)
{
    qreal zoom = value / (qreal) 50;
    qDebug("scalar: %f", zoom);
    view->scale(zoom,zoom);
}

#include "zoomwidget.h"
#include "ui_zoomwidget.h"

ZoomWidget::ZoomWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ZoomWidget)
{
    ui->setupUi(this);
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

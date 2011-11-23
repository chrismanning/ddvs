#include "additemdialog.h"
#include "ui_additemdialog.h"

AddItemDialog::AddItemDialog(QGraphicsScene *scene, QList<DataType*> *items, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddItemDialog)
{
    ui->setupUi(this);

    this->scene = scene;
    this->items = items;
}

AddItemDialog::~AddItemDialog()
{
    delete ui;
}

void AddItemDialog::on_buttonBox_accepted()
{
    DataType *item = new DataType;
    items->append(item);
    scene->addItem(item);
}

#include "additemdialog.h"
#include "ui_additemdialog.h"

AddItemDialog::AddItemDialog(QGraphicsScene *scene, QList<DataType*> *items, QStringList *types, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddItemDialog)
{
    ui->setupUi(this);

    this->scene = scene;
    this->items = items;

    //layout()->setSizeConstraint(QLayout::SetFixedSize);

    ui->typeComboBox->addItems(*types);
    //ui->buttonBox->setEnabled(false);
}

AddItemDialog::~AddItemDialog()
{
    delete ui;
    qDebug("Closing dialog...");
}

void AddItemDialog::on_buttonBox_accepted()
{
    DataType *item = new DataType(ui->typeComboBox->currentText(), ui->nameText->displayText(), ui->valueText->displayText());
    items->append(item);
    scene->addItem(item);
    this->close();
}

#include "additemdialog.h"
#include "ui_additemdialog.h"

AddItemDialog::AddItemDialog(QGraphicsScene *scene, QHash<QString,DataType*> *items, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddItemDialog)
{
    ui->setupUi(this);

    this->scene = scene;
    this->items = items;

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    ui->userTypeGroupBox->hide();
    ui->simpleTypeGroupBox->hide();
    ui->newTypeGroupBox->hide();
    //ui->buttonBox->setEnabled(false);
}

AddItemDialog::~AddItemDialog()
{
    delete ui;
    qDebug("Closing dialog...");
}

void AddItemDialog::on_buttonBox_accepted()
{
    QString typeName = ui->typeComboBox->currentText();
    if(ui->newTypeNameText->displayText().size() > 0) {
        typeName = ui->newTypeNameText->text();
    }
    DataType *item;
    if(typeName.startsWith("*")) {
        item = new Pointer(typeName, ui->nameText->displayText(), items->value(ui->valueText->displayText(),0));
        scene->addItem(((Pointer*)item)->link);
    }
    else
        item = new DataType(typeName, ui->nameText->displayText(), ui->valueText->displayText());
    items->insert(item->name, item);
    scene->addItem(item);
    this->close();
}

void AddItemDialog::on_typeComboBox_activated(int index)
{
    ui->userTypeGroupBox->hide();
    ui->simpleTypeGroupBox->hide();
    ui->newTypeGroupBox->hide();

    switch(index)
    {
        case 0://none
            break;
        case 1:
            ui->newTypeGroupBox->setVisible(true);
            break;
        case 2:
        case 3:
            ui->simpleTypeGroupBox->setVisible(true);
            break;
        default:
            ui->userTypeGroupBox->setVisible(true);
    }
}

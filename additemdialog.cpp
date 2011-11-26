#include "additemdialog.h"
#include "ui_additemdialog.h"

AddItemDialog::AddItemDialog(QGraphicsScene *scene, QList<DataType*> *items, QStringList *types, QWidget *parent) :
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
    ui->newMembersWidget->setTypes(types);
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
    QString typeName = ui->typeComboBox->currentText();
    if(ui->newTypeNameText->displayText().size() > 0) {
        typeName = ui->newTypeNameText->text();
    }
    DataType *item = new DataType(typeName, ui->nameText->displayText(), ui->valueText->displayText());
    items->append(item);
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
